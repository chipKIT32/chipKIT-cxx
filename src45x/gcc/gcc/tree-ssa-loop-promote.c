/* Promotion of shorter-than-word-size loop indices.
   Copyright (C) 2009 Free Software Foundation, Inc.
   
This file is part of GCC.
   
GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.
   
GCC is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
   
You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* This pass finds loop indices that are declared as
   shorter-than-word-size and replaces them with word-sized loop
   indices.  (It assumes that word-sized quantities are the most
   efficient type on which to do arithmetic.)  The loop optimization
   machinery has a difficult time seeing through the casts required to
   promote such indices to word-sized quantities for memory addressing
   and/or preserving the semantics of the source language (such as C).
   The transformation also helps eliminate unnecessary
   {sign,zero}-extensions required for the same.

   Although this is most naturally expressed as a loop optimization
   pass, we choose to place this pass some ways before the loop
   optimization passes proper, so that other scalar optimizations will
   run on our "cleaned-up" code.  This decision has the negative of
   requiring us to build and destroy all the loop optimization
   infrastructure.

   The algorithm is relatively simple.  For each single-exit loop, we
   identify the loop index variable.  If the loop index variable is
   shorter than the word size, then we have a candidate for promotion.
   We determine whether the scalar evolution of the loop index fits a
   particular pattern (incremented by 1, compared against a
   similarly-typed loop bound, and only modified by a single increment
   within the loop), as well as examining the uses of the loop index to
   ensure we are able to safely promote those uses (e.g. the loop index
   must not be stored to memory or passed to function calls).  If these
   conditions are satisfied, we create an appropriate word-sized type
   and replace all uses and defs of the loop index variable with the new
   variable.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"

#include "toplev.h"
#include "rtl.h"
#include "tm_p.h"
#include "hard-reg-set.h"
#include "obstack.h"
#include "basic-block.h"
#include "pointer-set.h"
#include "intl.h"

#include "tree.h"
#include "gimple.h"
#include "hashtab.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "cfgloop.h"
#include "flags.h"
#include "timevar.h"
#include "tree-pass.h"

struct promote_info {
  /* The loop being analyzed.  */
  struct loop *loop;

  /* The GIMPLE_COND controlling exit from the loop.  */
  gimple exit_expr;

  /* The loop index variable's SSA_NAME that is defined in a phi node in
     LOOP->HEADER.  Note that this SSA_NAME may be different than the
     one appearing in EXIT_EXPR.  */
  tree loop_index_name;

  /* The bound of the loop.  */
  tree loop_limit;

  /* Whether we've warned about things with
     warn_unsafe_loop_optimizations.  */
  bool warned;

  /* LOOP_INDEX_NAME's underlying VAR_DECL.  */
  tree var_decl;

  /* The types to which defs/uses of LOOP_INDEX_NAME are cast via
     NOP_EXPRs.  */
  VEC(tree, heap) *cast_types;

  /* The number of times we have seen a cast to the corresponding type
     (as determined by types_compatible_p) in CAST_TYPES.  */
  VEC(int, heap) *cast_counts;

  /* Whether LOOP_INDEX_NAME is suitable for promotion.  */
  bool can_be_promoted_p;

  /* If CAN_BE_PROMOTED_P, the promoted type.  */
  tree promoted_type;

  /* If CAN_BE_PROMOTED_P, the promoted VAR_DECL.  */
  tree promoted_var;
};

/* A set of `struct promote_info'.  */

static struct pointer_set_t *promotion_info;

/* A set of all potentially promotable SSA_NAMEs, used for quick
decision-making during analysis.  */

static struct pointer_set_t *promotable_names;

/* A map from SSA_NAMEs to the VAR_DECL to which they will be
   promoted.  */

static struct pointer_map_t *variable_map;

/* A set of the stmts that we have already rebuilt with promoted variables.  */

static struct pointer_set_t *promoted_stmts;

/* Phi nodes that we have to fix up after we've promoted everything else.  */

static struct pointer_set_t *phi_nodes_to_promote;


/* Add CASTED to PI->CAST_TYPES if we haven't seen CASTED before.  */

static void
add_casted_type (struct promote_info *pi, tree casted)
{
  int i;
  tree type;

  /* For this information to be useful later, CASTED must be wider than
     the type of the variable.  */
  if (TYPE_PRECISION (casted) <= TYPE_PRECISION (TREE_TYPE (pi->var_decl)))
    return;

  for (i = 0; VEC_iterate (tree, pi->cast_types, i, type); i++)
    if (types_compatible_p (casted, type))
      {
       int c = VEC_index(int, pi->cast_counts, i);
       VEC_replace(int, pi->cast_counts, i, ++c);
       return;
      }

  /* Haven't see the type before.  */
  VEC_safe_push (tree, heap, pi->cast_types, casted);
  VEC_safe_push (int, heap, pi->cast_counts, 1);
}

/* Return the most-casted-to type in PI->CAST_TYPES.  Return an
   appropriately signed variant of size_type_node if the variable wasn't
   cast in some fashion.  */

static tree
choose_profitable_promoted_type (struct promote_info *pi)
{
  int i;
  int count;
  tree type = NULL_TREE;
  int maxuse = -1;

  for (i = 0; VEC_iterate (int, pi->cast_counts, i, count); i++)
    if (count > maxuse)
      {
       maxuse = count;
       type = VEC_index (tree, pi->cast_types, i);
      }

  if (type == NULL_TREE)
    {
      if (dump_file)
       {
         fprintf (dump_file, "Warning, failed to find upcast type for ");
         print_generic_expr (dump_file, pi->loop_index_name, 0);
         fprintf (dump_file, "\n");
       }
      return (TYPE_UNSIGNED (TREE_TYPE (pi->var_decl))
             ? size_type_node
             : signed_type_for (size_type_node));
    }
  else
    return signed_type_for (type);
}

/* Intuit the loop index for LOOP from PHI.  There must be a path that
   only goes through NOP_EXPRs or CONVERT_EXPRs from the result of PHI
   to one of the operands of COND.  If such a path cannot be found,
   return NULL_TREE.  If LIMIT is not NULL and a path can be found,
   store the other operand of COND into LIMIT.  */

static tree
find_promotion_candidate_from_phi (struct loop *loop, gimple cond,
                                  gimple phi, tree *limit)
{
  tree op0, op1;
  tree result, candidate;

  result = candidate = PHI_RESULT (phi);
  /* Must be an integer variable.  */
  if (TREE_CODE (TREE_TYPE (candidate)) != INTEGER_TYPE)
    return NULL_TREE;

  op0 = gimple_cond_lhs (cond);
  op1 = gimple_cond_rhs (cond);

  /* See if there's a path from CANDIDATE to an operand of COND.  */
  while (true)
    {
      use_operand_p use;
      imm_use_iterator iui;
      gimple use_stmt = NULL;

      if (candidate == op0)
       {
         if (limit) *limit = op1;
         break;
       }
      if (candidate == op1)
       {
         if (limit) *limit = op0;
         break;
       }

      /* Find a single use in the loop header.  Give up if there's
        multiple ones.  */
      FOR_EACH_IMM_USE_FAST (use, iui, candidate)
       {
         gimple stmt = USE_STMT (use);

         if (gimple_bb (stmt) == loop->header)
           {
             if (use_stmt)
               {
                 if (dump_file)
                   {
                     fprintf (dump_file, "Rejecting ");
                     print_generic_expr (dump_file, candidate, 0);
                     fprintf (dump_file, " because it has multiple uses in the loop header (bb #%d).\n",
                              loop->header->index);
                     fprintf (dump_file, "first use: ");
                     print_gimple_stmt (dump_file, use_stmt, 0, 0);
                     fprintf (dump_file, "\nsecond use: ");
                     print_gimple_stmt (dump_file, stmt, 0, 0);
                     fprintf (dump_file, "\n(possibly more, but unanalyzed)\n");
                   }
                 return NULL_TREE;
               }
             else
               use_stmt = stmt;
           }
       }

      /* No uses in the loop header, bail.  */
      if (use_stmt == NULL)
       return NULL_TREE;

      if (gimple_code (use_stmt) != GIMPLE_ASSIGN
         || TREE_CODE (gimple_assign_lhs (use_stmt)) != SSA_NAME
         || (gimple_assign_rhs_code (use_stmt) != NOP_EXPR
             && gimple_assign_rhs_code (use_stmt) != CONVERT_EXPR))
       {
         if (dump_file)
           {
             fprintf (dump_file, "Rejecting ");
             print_generic_expr (dump_file, candidate, 0);
             fprintf (dump_file, " because of use in ");
             print_gimple_stmt (dump_file, use_stmt, 0, 0);
             fprintf (dump_file, "\n");
           }
         return NULL_TREE;
       }

      candidate = gimple_assign_lhs (use_stmt);
    }

  /* CANDIDATE is now what we believe to be the loop index variable.  There
     are two possibilities:

     - CANDIDATE is not the "true" loop index variable, but rather is a
       promoted version of RESULT, done for purposes of satisfying a
       language's semantics;

     - CANDIDATE is the "true" loop index variable.  */
  if (!types_compatible_p (TREE_TYPE (result), TREE_TYPE (candidate)))
    candidate = result;

  /* The type of candidate must be "short" to consider promoting it.  */
  if (TREE_CODE (TREE_TYPE (candidate)) != INTEGER_TYPE
      || TYPE_PRECISION (TREE_TYPE (candidate)) >= TYPE_PRECISION (size_type_node))
    return NULL_TREE;

  return candidate;
}

/* Find the loop index variable of LOOP.  LOOP's exit is controlled by
   the COND_EXPR EXPR.  IF we can't determine what the loop index
   variable is, or EXPR does not appear to be analyzable, then return
   NULL_TREE.  */

static tree
find_promotion_candidate (struct loop *loop, gimple cond, tree *limit)
{
  tree candidate = NULL_TREE;
  gimple_stmt_iterator gsi;

  switch (gimple_cond_code (cond))
    {
    case GT_EXPR:
    case GE_EXPR:
    case NE_EXPR:
    case LT_EXPR:
    case LE_EXPR:
      break;

    default:
      return NULL_TREE;
    }

  /* We'd like to examine COND and intuit the loop index variable from
     there.  Instead, we're going to start from the phi nodes in BB and
     attempt to work our way forwards to one of the operands of COND,
     since starting from COND might yield an upcast loop index.  If we
     find multiple phi nodes whose results reach COND, then give up.  */
  for (gsi = gsi_start_phis (loop->header); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple phi = gsi_stmt (gsi);
      tree t = find_promotion_candidate_from_phi (loop, cond, phi, limit);

      if (t == NULL_TREE)
       continue;
      else if (candidate == NULL_TREE)
       candidate = t;
      else
       {
         if (dump_file)
           {
             fprintf (dump_file, "Can't find a candidate from ");
             print_gimple_stmt (dump_file, cond, 0, 0);
             fprintf (dump_file, "\n  because too many phi node results reach the condition.\n");
           }
         return NULL_TREE;
       }
    }

  return candidate;
}

/* Return true if X is something that could be promoted.  */

static bool
could_be_promoted (tree x)
{
  return (TREE_CODE (x) == INTEGER_CST
         || (TREE_CODE (x) == SSA_NAME
             && pointer_set_contains (promotable_names, x)));
}

/* Examine the RHS of STMT's suitability with respect to being able to
   promote VAR.  */

static bool
check_rhs_for_promotability (struct promote_info *pi, tree var, gimple stmt,
                            bool is_assign)
{
  enum tree_code subcode = gimple_assign_rhs_code (stmt);

  bool ok = true;

  switch (subcode)
    {
    case PLUS_EXPR:
    case MINUS_EXPR:
    case MULT_EXPR:
    case EQ_EXPR:
    case NE_EXPR:
    case LT_EXPR:
    case LE_EXPR:
    case GT_EXPR:
    case GE_EXPR:
      {
       tree op0 = gimple_assign_rhs1 (stmt);
       tree op1 = gimple_assign_rhs2 (stmt);

       ok = ((op0 == var && could_be_promoted (op1))
             || (op1 == var && could_be_promoted (op0)));
       break;
      }
    case COND_EXPR:
      if (gimple_expr_type (stmt) == NULL
         || gimple_expr_type (stmt) == void_type_node)
       ok = true;
      else
       /* This is conservative; it's possible that these sorts of nodes
          could be promoted, but we'd have to be very careful about
          checking in which parts of the COND_EXPR the promotable
          variable(s) are.  */
       ok = false;
      break;
    case SSA_NAME:
      {
       tree expr = gimple_assign_rhs1 (stmt);
       ok = (expr == var || could_be_promoted (expr));
      }
      break;
    case INTEGER_CST:
      break;
    case NOP_EXPR:
    case CONVERT_EXPR:
      if (!is_assign)
       {
         add_casted_type (pi, gimple_expr_type (stmt));
         break;
       }
      /* Fallthrough.  */
    default:
      ok = false;
      break;
    }

  return ok;
}

/* Analyze the loop index VAR for promotability.  The rules for
   promotability are:

   For uses:

   - The underlying variable may be used in NOP_EXPRs.

   - The underlying variable may be used in simple arithmmetic
     expressions so long as the other parts are potentially promotable
     variables or constants (so we don't go willy-nilly on promoting
     things).

   - The underlying variable may not be stored to memory.

   - All uses must occur inside the loop.

   For defs:

   - The underlying variable may not be loaded from memory; and

   - The underlying variable may only be formed from expressions
     involving potentially promotable varibles or constants.

   Note that defs may occur outside of the loop; we do this to handle
   initial conditions before entering the loop.  */

static void
analyze_loop_index_uses (tree var, struct promote_info *pi)
{
  imm_use_iterator iui;
  use_operand_p use;
  gimple bad_stmt = NULL;
  const char *reason = NULL;

  FOR_EACH_IMM_USE_FAST (use, iui, var)
    {
      basic_block bb;
      gimple use_stmt = USE_STMT (use);

      /* Uses must exist only within the loop.  */
      bb = gimple_bb (use_stmt);

      if (dump_file)
       {
         fprintf (dump_file, "Checking ");
         print_gimple_stmt (dump_file, use_stmt, 0, 0);
         fprintf (dump_file, "\n");
       }

      if (!flow_bb_inside_loop_p (pi->loop, bb))
       {
         bad_stmt = use_stmt;
         reason = " is involved in stmt outside loop ";
         break;
       }

      /* We cannot store the index to memory.  */
      if (gimple_references_memory_p (use_stmt))
       {
         bad_stmt = use_stmt;
         reason = " is stored to memory in ";
         break;
       }

      if (gimple_code (use_stmt) == GIMPLE_CALL)
       {
         /* We cannot pass the variable to a function.  */
         bad_stmt = use_stmt;
         reason = " is passed to function in ";
         break;
       }
      else if (gimple_code (use_stmt) == GIMPLE_ASSIGN)
       {
         tree lhs = gimple_assign_lhs (use_stmt);

         if (!check_rhs_for_promotability (pi, var, use_stmt,
                                           /*is_assign=*/false))
           {
             bad_stmt = use_stmt;
             reason = " is involved in non-promotable expression ";
             break;
           }
         else if ((TREE_CODE_CLASS (gimple_assign_rhs_code (use_stmt)) == tcc_binary
                   || gimple_assign_rhs_code (use_stmt) == SSA_NAME)
                  && !could_be_promoted (lhs))
           {
             bad_stmt = use_stmt;
             reason = " is being assigned to non-promotable variable ";
             break;
           }
       }
      else if (gimple_code (use_stmt) != GIMPLE_COND
              && gimple_code (use_stmt) != GIMPLE_PHI)
       {
         /* Use of the variable in some statement we don't know how to
            analyze.  */
         bad_stmt = use_stmt;
         reason = " is used in unanalyzable expression in ";
         break;
       }
    }

  if (bad_stmt && reason)
    {
      if (dump_file)
       {
         fprintf (dump_file, "Loop index ");
         print_generic_expr (dump_file, var, 0);
         fprintf (dump_file, "%s", reason);
         print_gimple_stmt (dump_file, bad_stmt, 0, 0);
         fprintf (dump_file, "\n");
       }
      pi->can_be_promoted_p = false;
    }
}

/* Check that the uses and def of VAR, defined in STMT, conform to the
   rules given above.  */

static bool
analyze_loop_index (tree var, gimple stmt, void *data)
{
  struct promote_info *pi = (struct promote_info *) data;

  if (dump_file)
    {
      fprintf (dump_file, "Analyzing loop index ");
      print_generic_expr (dump_file, var, 0);
      fprintf (dump_file, " defined in ");
      print_gimple_stmt (dump_file, stmt, 0, 0);
      fprintf (dump_file, "\n");
    }

  /* Check the definition.  */
  switch (gimple_code (stmt))
    {
    case GIMPLE_PHI:
      /* Phi nodes are OK.  */
      break;

    case GIMPLE_ASSIGN:
      if (!check_rhs_for_promotability (pi, var, stmt,
                                       /*is_assign=*/true))
       break;
      /* Fallthrough.  */

    default:
      /* Something we can't handle or the variable is being loaded from
        memory.  */
      pi->can_be_promoted_p = false;
      goto done;
    }

  if (gimple_code (stmt) == GIMPLE_PHI)
    {
      unsigned int i;

      for (i = 0; i < gimple_phi_num_args (stmt); i++)
       {
         tree arg = PHI_ARG_DEF (stmt, i);

         if (TREE_CODE (arg) == SSA_NAME)
           pointer_set_insert (promotable_names, arg);
       }

      analyze_loop_index_uses (PHI_RESULT (stmt), pi);
    }
  else
    analyze_loop_index_uses (var, pi);

  /* Only worth continuing if we think the loop index can be
     promoted.  */
 done:
  if (dump_file)
    {
      fprintf (dump_file, "Done analyzing ");
      print_generic_expr (dump_file, var, 0);
      fprintf (dump_file, " defined in ");
      print_gimple_stmt (dump_file, stmt, 0, 0);
      fprintf (dump_file, "...%s to analyze\n\n",
              pi->can_be_promoted_p ? "continuing" : "not continuing");
    }
  return !pi->can_be_promoted_p;
}

/* Determine whether T is an INTEGER_CST or a single-use SSA_NAME
   defined as the result of a NOP_EXPR or CONVERT_EXPR.  Return the
   operand of the NOP_EXPR or CONVERT_EXPR if so.  */

static tree
upcast_operand_p (tree t)
{
  gimple def;

  if (TREE_CODE (t) == INTEGER_CST)
    return t;

  if (TREE_CODE (t) != SSA_NAME
      || !has_single_use (t))
    return NULL_TREE;

  def = SSA_NAME_DEF_STMT (t);
  if (gimple_code (def) != GIMPLE_ASSIGN)
    return NULL_TREE;

  if (gimple_assign_rhs_code (def) != CONVERT_EXPR
      && gimple_assign_rhs_code (def) != NOP_EXPR)
    return NULL_TREE;

  return gimple_assign_rhs1 (def);
}

/* Check for the idiom:

     short x, y;
     unsigned short x.2, y.2, tmp;
     ...
     x.2 = (unsigned short) x;
     y.2 = (unsigned short) y;
     tmp = x.2 + y.2;
     x = (short) tmp;

   which is generated by convert for avoiding signed arithmetic
   overflow.  RHS is TMP in the above statement.  If RHS is
   defined via such an idiom, store x and y into *OP0 and *OP1,
   respectively.  We permit y.2 to be a constant if necessary.  */

static bool
signed_arithmetic_overflow_idiom_p (tree rhs, tree *op0, tree *op1)
{
  gimple op_stmt = SSA_NAME_DEF_STMT (rhs);
  tree x2, y2;
  bool yes = false;
  enum tree_code code;

  if (!has_single_use (rhs)
      || gimple_code (op_stmt) != GIMPLE_ASSIGN)
    goto done;

  /* This could probably profitably be expanded to consider
     MINUS_EXPR, MULT_EXPR, etc.  */
  code = gimple_assign_rhs_code (op_stmt);
  if (code != PLUS_EXPR)
    goto done;
  x2 = gimple_assign_rhs1 (op_stmt);
  y2 = gimple_assign_rhs2 (op_stmt);

  x2 = upcast_operand_p (x2);
  if (x2 == NULL_TREE)
    goto done;
  y2 = upcast_operand_p (y2);
  if (y2 == NULL_TREE)
    goto done;

  *op0 = x2;
  *op1 = y2;
  yes = true;

 done:
  return yes;
}

/* Simple wrapper around flow_bb_inside_loop_p that handles NULL
   statements and initial definitions of variables.  */

static bool
stmt_in_loop_p (gimple t, struct loop *loop)
{
  basic_block bb;

  if (t == NULL)
    return false;

  bb = gimple_bb (t);
  if (bb == NULL)
    return false;

  return flow_bb_inside_loop_p (loop, bb);
}

/* The loop index should have a specific usage pattern:

   - It should be defined in a phi node with two incoming values:

     LI_phi = PHI (LI_out, LI_in)

   - One incoming value, LI_out, should be from outside the loop.

   - The other incoming value, LI_in, should be defined thusly:

     LI_in = LI_phi + increment

   - increment should be 1.  We permit other increments with
     -funsafe-loop-optimizations.

   - Finally, in the comparison to exit the loop, the loop index must be
     compared against a variable that has a type at least as precise as
     the loop index's type.  For instance, something like:

       char limit;
       short i;

       for (i = 0; i < limit; i++) ... 

     would not be permitted.  */

static bool
analyze_loop_index_definition_pattern (struct promote_info *pi)
{
  gimple phi = SSA_NAME_DEF_STMT (pi->loop_index_name);
  bool ok = false, warn = false;
  tree in0, in1;
  bool inside0, inside1;
  gimple def0, def1;
  tree op0, op1, increment = NULL_TREE;

  if (gimple_code (phi) != GIMPLE_PHI
      || gimple_phi_num_args (phi) != 2)
    goto done;

  in0 = PHI_ARG_DEF (phi, 0);
  in1 = PHI_ARG_DEF (phi, 1);

  /* Figure out which value comes from outside the loop.  */
  def0 = TREE_CODE (in0) == SSA_NAME ? SSA_NAME_DEF_STMT (in0) : NULL;
  def1 = TREE_CODE (in1) == SSA_NAME ? SSA_NAME_DEF_STMT (in1) : NULL;

  inside0 = stmt_in_loop_p (def0, pi->loop);
  inside1 = stmt_in_loop_p (def1, pi->loop);

  if (inside0 && inside1)
    goto done;
  else if (inside0)
    {
      tree t = in0;
      gimple g;
      in0 = in1;
      in1 = t;
      g = def0;
      def0 = def1;
      def1 = g;
    }
  else if (!inside1)
    goto done;

  /* IN0 comes from outside the loop, IN1 from inside.  Analyze IN1.  */
  if (gimple_code (def1) != GIMPLE_ASSIGN)
    goto done;

  switch (gimple_assign_rhs_code (def1))
    {
    case CONVERT_EXPR:
    case NOP_EXPR:
      if (!signed_arithmetic_overflow_idiom_p (gimple_assign_rhs1 (def1),
                                              &op0, &op1))
       goto done;
      goto plus;
    case PLUS_EXPR:
      op0 = gimple_assign_rhs1 (def1);
      op1 = gimple_assign_rhs2 (def1);
    plus:
      {
       bool op0_li = op0 == PHI_RESULT (phi);
       bool op1_li = op1 == PHI_RESULT (phi);
       if (op0_li && op1_li)
         /* This is weird, and definitely is not a case we can support
            for promotion.  */
         goto done;
       else if (op0_li)
         increment = op1;
       else if (op1_li)
         increment = op0;
       else
         goto done;
       break;
      }
    default:
      break;
    }


  /* Check that the exit condition for the loop is OK.  */
  {
    enum tree_code code = gimple_cond_code (pi->exit_expr);

    op0 = gimple_cond_lhs (pi->exit_expr);
    op1 = gimple_cond_rhs (pi->exit_expr);

    if (op0 == pi->loop_limit)
      {
       tree t = op0;
       op0 = op1;
       op1 = t;
       code = swap_tree_comparison (code);
      }

    if (code != LT_EXPR && code != LE_EXPR)
      goto done;

    if (!types_compatible_p (TREE_TYPE (pi->loop_index_name),
                            TREE_TYPE (pi->loop_limit)))
      {
       switch (TREE_CODE (pi->loop_limit))
         {
         case INTEGER_CST:
           if (!int_fits_type_p (pi->loop_limit,
                                 TREE_TYPE (pi->loop_index_name)))
             goto done;
           break;
         case SSA_NAME:
           {
             tree v = pi->loop_limit;
             gimple def = SSA_NAME_DEF_STMT (v);

             /* Backtrack through CONVERT_EXPRs and/or NOP_EXPRs to
                determine if the variables "started out" as the same
                type.  */
             while (gimple_code (def) == GIMPLE_ASSIGN)
               {
                 enum tree_code rhs_code = gimple_assign_rhs_code (def);

                 if (rhs_code != NOP_EXPR && rhs_code != CONVERT_EXPR)
                   break;

                 v = gimple_assign_rhs1 (def);
                 def = SSA_NAME_DEF_STMT (v);
               }
             /* Permit comparisons between non-compatible types with
                flag_unsafe_loop_optimizations, since we can assume the
                loop index does not overflow.  */
             if (types_compatible_p (TREE_TYPE (pi->loop_index_name),
                                     TREE_TYPE (v))
                 || flag_unsafe_loop_optimizations)
               break;
             /* Fallthrough.  */
           default:
             goto done;
           }
         }
      }
  }

  if (increment == NULL_TREE)
    goto done;
  if (TREE_CODE (increment) != INTEGER_CST
      || compare_tree_int (increment, 1) != 0)
    {
      warn = true;
      if (!flag_unsafe_loop_optimizations)
       goto done;
    }

  ok = true;
 done:
  if (warn && !pi->warned)
    {
      pi->warned = true;
      /* We can promote unsigned indices only if -funsafe-loop-optimizations
        is in effect, since the user might be depending on the modulo
        wraparound behavior of unsigned types.  */
      if (warn_unsafe_loop_optimizations)
       {
         const char *wording;

         wording = (flag_unsafe_loop_optimizations
                    ? N_("assuming that the loop counter does not overflow")
                    : N_("cannot optimize loop, the loop counter may overflow"));
         warning (OPT_Wunsafe_loop_optimizations, "%s", gettext (wording));
       }
    }

  return ok;
}

/* Analyze the loop associated with PI_ to see if its loop index can be
   promoted.  */

static bool
analyze_loop (const void *pi_, void *data)
{
  struct promote_info *pi = CONST_CAST (struct promote_info *,
                                       (const struct promote_info *) pi_);
  bool *changed = (bool *) data;

  /* We previously determined we can't promote this; go ahead and
     continue iterating.  */
  if (pi->loop_index_name == NULL_TREE)
    return true;

  /* Assume we can always promote the loop index, even if it doesn't
     exist.  */
  pi->can_be_promoted_p = true;

  if (dump_file)
    {
      fprintf (dump_file, "Analyzing ");
      print_generic_expr (dump_file, pi->loop_index_name, 0);
      fprintf (dump_file, "\n");
    }

  if (pi->loop_index_name
      && analyze_loop_index_definition_pattern (pi))
    {
      /* Clear any previously gathered information.  */
      VEC_truncate (tree, pi->cast_types, 0);
      VEC_truncate (int, pi->cast_counts, 0);

      walk_use_def_chains (pi->loop_index_name, analyze_loop_index, pi, false);
    }
  else
    pi->can_be_promoted_p = false;

  /* If we determined the loop index is used in strange ways, clear it
     so we don't examine it again.  */
  if (!pi->can_be_promoted_p)
    pi->loop_index_name = NULL_TREE;

  /* Let our caller know whether to re-do the analysis.  */
  *changed = *changed || !pi->can_be_promoted_p;
  /* Continue if PI is promotable.  */
  return pi->can_be_promoted_p;
}

/* Add PI_->LOOP_INDEX_NAME to the set of variables, DATA, that we are
   considering for promotion.  */

static bool
add_variable (const void *pi_, void *data ATTRIBUTE_UNUSED)
{
  const struct promote_info *pi = (const struct promote_info *) pi_;
  struct pointer_set_t *pset = (struct pointer_set_t *) data;
  int presentp;

  if (pi->loop_index_name != NULL_TREE)
    {
      presentp = pointer_set_insert (pset, pi->loop_index_name);
      gcc_assert (!presentp);
    }

  /* Continue traversal.  */
  return true;
}

/* For each promotable variable:

   - create a new, promoted VAR_DECL;

   - walk through all the uses and defs and create new statements using
     the promoted variables.  We don't create new phi nodes; post-pass
     SSA update will handle those for us.  */

/* Make dump files readable.  */
#define PROMOTED_VAR_SUFFIX ".promoted"

/* Create a variable NAME with TYPE and do the necessary work to inform
   the SSA machinery about it.  */

static tree
create_pli_var (tree type, char *name)
{
  tree var = create_tmp_var (type, name);
  create_var_ann (var);
  mark_sym_for_renaming (var);
  add_referenced_var (var);
  return var;
}

/* Associate the SSA_NAME VAR with the promoted variable DATA.  */

static bool
associate_name_with_var (tree var, gimple def_stmt, void *data)
{
  tree promoted_var = (tree) data;
  void **p;

  gcc_assert (promoted_var != NULL_TREE);

  if (gimple_code (def_stmt) == GIMPLE_PHI)
    var = PHI_RESULT (def_stmt);

  p = pointer_map_insert (variable_map, var);

  if (!*p)
    {
      if (dump_file)
       {
         fprintf (dump_file, "Associating ");
         print_generic_expr (dump_file, var, 0);
         fprintf (dump_file, " with ");
         print_generic_expr (dump_file, promoted_var, 0);
         fprintf (dump_file, "\n\n");
       }
      *(tree *)p = promoted_var;
    }

  /* Continue traversal.  */
  return false;
}

/* Create a promoted variable for the variable from PI_.  */

static bool
create_promoted_variable (const void *pi_, void *data ATTRIBUTE_UNUSED)
{
  struct promote_info *pi = CONST_CAST (struct promote_info *,
                                       (const struct promote_info *) pi_);

  if (pi->can_be_promoted_p)
    {
      tree type = choose_profitable_promoted_type (pi);
      tree orig_name = DECL_NAME (pi->var_decl);
      size_t id_len = IDENTIFIER_LENGTH (orig_name);
      size_t name_len = id_len + strlen (PROMOTED_VAR_SUFFIX) + 1;
      char *name;

      name = (char *) alloca (name_len);
      strcpy (name, IDENTIFIER_POINTER (orig_name));
      strcpy (name + id_len, PROMOTED_VAR_SUFFIX);

      pi->promoted_type = type;
      pi->promoted_var = create_pli_var (type, name);

      if (dump_file)
       {
         fprintf (dump_file, "Created new variable ");
         print_generic_expr (dump_file, pi->promoted_var, 0);
         fprintf (dump_file, " to stand in for ");
         print_generic_expr (dump_file, pi->loop_index_name, 0);
         fprintf (dump_file, "\n\n");
       }

      walk_use_def_chains (pi->loop_index_name,
                          associate_name_with_var,
                          pi->promoted_var, false);
    }

  /* Continue traversal.  */
  return true;
}

/* Rebuild T with newly promoted variables; STMT is the original
   statement in which T appeared and may be equivalent to T.  TYPE is
   non-null when rebuilding the rhs of a GIMPLE_ASSIGN and indicates the
   type of the lhs.  */

static tree
rebuild_tree_with_promotion (tree t, gimple stmt, tree type,
                            gimple_stmt_iterator gsi,
                            struct promote_info *pi)
{
  tree op0, op1;

  switch (TREE_CODE (t))
    {
    case NOP_EXPR:
    case CONVERT_EXPR:
      {
       tree pvar = rebuild_tree_with_promotion (TREE_OPERAND (t, 0), stmt, type, gsi, pi);

       if (types_compatible_p (type, TREE_TYPE (pvar)))
         return pvar;
       else
         return build1 (TREE_CODE (t), type, pvar);
      }
    case INTEGER_CST:
      {
       return build_int_cst_wide (pi->promoted_type,
                                  TREE_INT_CST_LOW (t),
                                  TREE_INT_CST_HIGH (t));
      }
    case COND_EXPR:
      {
       tree orig_op0 = TREE_OPERAND (t, 0);
       op0 = rebuild_tree_with_promotion (orig_op0, stmt, type, gsi, pi);
       gcc_assert (orig_op0 != op0);
       TREE_OPERAND (t, 0) = op0;
       return t;
      }
    case PLUS_EXPR:
    case MINUS_EXPR:
    case MULT_EXPR:
      type = pi->promoted_type;
      goto binary_expr;
    case EQ_EXPR:
    case NE_EXPR:
    case LT_EXPR:
    case LE_EXPR:
    case GT_EXPR:
    case GE_EXPR:
      type = TREE_TYPE (t);
    binary_expr:
      op0 = TREE_OPERAND (t, 0);
      op1 = TREE_OPERAND (t, 1);
      op0 = rebuild_tree_with_promotion (op0, stmt, type, gsi, pi);
      op1 = rebuild_tree_with_promotion (op1, stmt, type, gsi, pi);
      return build2 (TREE_CODE (t), type, op0, op1);
    case SSA_NAME:
      {
       void **p = pointer_map_contains (variable_map, t);

       if (p == NULL)
         {
           /* This is unexpected, but it does happen if we were dealing
              with COND_EXPRs and such.  Just go ahead and create a
              temporary for it.  */
           if (types_compatible_p (TREE_TYPE (t), pi->promoted_type)
               || SSA_NAME_DEF_STMT (t) == stmt)
             return t;
           else
             goto insert_cast;
         }
       else
         return *(tree *)p;
      }
    case VAR_DECL:
      return t;
    default:
    insert_cast:
      {
       gimple cast;
       tree tmp, nop;
       tree to_upcast = t;

       /* If we are dealing with a memory reference, then we can't have
          wrap it in a NOP_EXPR; we need to load the value from memory
          first, then convert it.  */
       if (!is_gimple_reg (to_upcast))
         {
           tree tmp = create_pli_var (TREE_TYPE (to_upcast),
                                      CONST_CAST (char *, "loadtmp"));
           gimple stmt = gimple_build_assign (tmp, to_upcast);
           gsi_insert_before (&gsi, stmt, GSI_SAME_STMT);
           to_upcast = tmp;
         }

       tmp = create_pli_var (pi->promoted_type,
                             CONST_CAST (char *, "promotetmp"));
       nop = build1 (NOP_EXPR, pi->promoted_type, to_upcast);
       cast = gimple_build_assign (tmp, nop);
       if (dump_file)
         {
           fprintf (dump_file, "Inserting cast ");
           print_gimple_stmt (dump_file, cast, 0, 0);
           fprintf (dump_file, " prior to ");
           print_gimple_stmt (dump_file, stmt, 0, 0);
           fprintf (dump_file, "\n");
         }
       gsi_insert_before (&gsi, cast, GSI_SAME_STMT);
       return tmp;
      }
    }
}

/* Split E and place STMT in the block created by doing so.  */

static void
insert_along_edge (gimple stmt, edge e)
{
  basic_block bb = split_edge (e);
  gimple_stmt_iterator gsi;

  gsi = gsi_last_bb (bb);
  gsi_insert_after (&gsi, stmt, GSI_NEW_STMT);
}

/* Rebuild STMT, which contains uses or a def of the promotable variable
   associated with PI.  */

static void
rebuild_with_promotion (gimple stmt, struct promote_info *pi)
{
  gimple_stmt_iterator gsi;

  if (pointer_set_insert (promoted_stmts, stmt))
    return;

  if (dump_file)
    {
      fprintf (dump_file, "Rebuilding stmt ");
      print_gimple_stmt (dump_file, stmt, 0, 0);
      fprintf (dump_file, "\n");
    }

  gsi = gsi_for_stmt (stmt);
  
  switch (gimple_code (stmt))
    {
    case GIMPLE_ASSIGN:
      {
       enum tree_code subcode = gimple_assign_rhs_code (stmt);
       enum tree_code newcode = subcode;
       tree lhs = gimple_assign_lhs (stmt);
       tree rhs1 = gimple_assign_rhs1 (stmt);
       tree rhs2 = gimple_assign_rhs2 (stmt);
       tree x, y;
       void **v;

       /* If we are defining a promotable variable, check for special
          idioms.  */
       v = pointer_map_contains (variable_map, lhs);
       if (v != NULL
           && *(tree *)v == pi->promoted_var
           && (subcode == NOP_EXPR || subcode == CONVERT_EXPR)
           && signed_arithmetic_overflow_idiom_p (rhs1, &x, &y))
         {
           void **xp;
           void **yp;
           if (TYPE_PRECISION (TREE_TYPE (rhs1))
               >= TYPE_PRECISION (pi->promoted_type))
             goto done;

           /* It's possible that we've already promoted the operands of
              one or both of the NOP_EXPRs.  In that case, we can
              bypass the logic below and go straight to rebuilding the
              rhs that we really want to transform.  */
           if (TREE_CODE (x) == VAR_DECL
               || TREE_CODE (y) == VAR_DECL)
             goto build_fake;
           xp = pointer_map_contains (variable_map, x);
           yp = pointer_map_contains (variable_map, y);

           /* Nothing to see here.  */
           if (!types_compatible_p (TREE_TYPE (x),
                                    TREE_TYPE (y))
               || (xp == NULL && yp == NULL))
             goto done;
           x = (xp == NULL ? NULL_TREE : *(tree *)xp);
           y = (yp == NULL ? NULL_TREE : *(tree *)yp);

           if (x != pi->promoted_var && y != pi->promoted_var)
             goto done;

         build_fake:
           newcode = PLUS_EXPR;
           rhs1 = x;
           rhs2 = y;
           if (dump_file)
             {
               fprintf (dump_file, "Substituting ");
               print_generic_expr (dump_file, x, 0);
               fprintf (dump_file, " + ");
               print_generic_expr (dump_file, y, 0);
               fprintf (dump_file, " for rhs of original statement\n");
             }

         done:
           ;
         }

       lhs = rebuild_tree_with_promotion (lhs, stmt, NULL, gsi, pi);
       rhs1 = rebuild_tree_with_promotion (rhs1, stmt, NULL, gsi, pi);
       if (rhs2)
         rhs2 = rebuild_tree_with_promotion (rhs2, stmt, NULL, gsi, pi);

       if (newcode != subcode)
         {
           gimple newstmt = gimple_build_assign_with_ops (newcode,
                                                          lhs, rhs1, rhs2);
	   gsi_insert_before (&gsi, newstmt, GSI_SAME_STMT);
           stmt = newstmt;
         }
       else
         {
           gimple_assign_set_lhs (stmt, lhs);
           gimple_assign_set_rhs1 (stmt, rhs1);
           if (rhs2)
             gimple_assign_set_rhs2 (stmt, rhs2);
         }
      }
      break;
    case GIMPLE_COND:
      {
       tree lhs = gimple_cond_lhs (stmt);
       tree rhs = gimple_cond_rhs (stmt);

       lhs = rebuild_tree_with_promotion (lhs, stmt, NULL, gsi, pi);
       rhs = rebuild_tree_with_promotion (rhs, stmt, NULL, gsi, pi);

       gimple_cond_set_lhs (stmt, lhs);
       gimple_cond_set_rhs (stmt, rhs);
      }
      break;
    case GIMPLE_PHI:
      /* Save for later rebuilding.  Any rebuilding of a phi node will
	 cause an incoming edge to be split, which means that the phi
	 node may be reallocated, which means that keeping track of
	 which statements have been rebuilt (by pointer equality) breaks
	 down.  */
      gsi = gsi_for_stmt (stmt);
      pointer_set_insert (phi_nodes_to_promote, gsi_stmt_ptr (&gsi));
      break;
    default:
      gcc_unreachable ();
    }

  if (dump_file)
    {
      fprintf (dump_file, "Converted stmt ");
      print_gimple_stmt (dump_file, stmt, 0, 0);
      fprintf (dump_file, "\n\n");
    }
  update_stmt (stmt);
}

/* Helper function for promote_variable that walks over use/def
   chains.  */

static bool
promote_variable_1 (tree var, gimple stmt, void *data)
{
  struct promote_info *pi = (struct promote_info *) data;
  imm_use_iterator imi;
  gimple use_stmt;

  rebuild_with_promotion (stmt, pi);

  if (gimple_code (stmt) == GIMPLE_PHI)
    var = PHI_RESULT (stmt);

  if (could_be_promoted (var))
    FOR_EACH_IMM_USE_STMT (use_stmt, imi, var)
      {
	rebuild_with_promotion (use_stmt, pi);
      }

  return false;
}

/* Convert all uses and defs of PI_->LOOP_INDEX_NAME as linked by
   use-def chains to uses and defs of PI_->PROMOTED_VAR.  */

static bool
promote_variable (const void *pi_, void *data ATTRIBUTE_UNUSED)
{
  const struct promote_info *pi = (const struct promote_info *) pi_;

  if (pi->can_be_promoted_p)
    {
      walk_use_def_chains (pi->loop_index_name, promote_variable_1,
                          CONST_CAST (struct promote_info *, pi), false);
    }

  /* Continue traversal.  */
  return true;
}

static bool
promote_phi_node (const void *phi_, void *data ATTRIBUTE_UNUSED)
{
  const gimple *cphi = (const gimple *) phi_;
  gimple_stmt_iterator gsi = gsi_for_stmt (*cphi);
  gimple *phi_p = gsi_stmt_ptr (&gsi);
  gimple phi = *phi_p;
  unsigned int i;
  bool promoted_result = could_be_promoted (PHI_RESULT (phi));

  /* There are two cases we have to be concerned about:

     - If the result can be promoted, then any incoming INTEGER_CSTs
       need to be assigned to the newly promoted variable, rather than
       the promotable variable;

     - If the result cannot be promoted, then we need to check if any of
       the arguments are promotable.  For each argument that is, we need
       to insert a truncating assignment to convert from the type of the
       promoted variable to the type of the PHI_RESULT.

     In both cases, we have to regrab the phi node after we've inserted
     the new assignment along the appropriate edge, since
     insert_along_edge might have reallocated the phi node.  */
  for (i = 0; i < gimple_phi_num_args (phi); i++)
    {
      tree var = gimple_phi_arg_def (phi, i);
      edge e = gimple_phi_arg_edge (phi, i);
      gimple assign = NULL;

      if (TREE_CODE (var) == INTEGER_CST && promoted_result)
	{
	  tree promoted, cst;
	  void **p;
	  
	  p = pointer_map_contains (variable_map, PHI_RESULT (phi));
	  gcc_assert (p);
	  promoted = (tree) *p;
	  cst = build_int_cst_wide (TREE_TYPE (promoted),
				    TREE_INT_CST_LOW (var),
				    TREE_INT_CST_HIGH (var));

	  assign = gimple_build_assign (promoted, cst);
	}
      else if (!promoted_result
	       && TREE_CODE (var) == SSA_NAME
	       && could_be_promoted (var))
	{
	  tree promoted, t, name;
	  void **p;

	  p = pointer_map_contains (variable_map, var);
	  gcc_assert (p);
	  promoted = (tree) *p;

	  if (dump_file)
	    {
	      fprintf (dump_file, "Inserting downcast of promoted variable ");
	      print_generic_expr (dump_file, promoted, 0);
	      fprintf (dump_file, "\n");
	    }

	  t = create_pli_var (TREE_TYPE (PHI_RESULT (phi)),
			      CONST_CAST (char *, "promote_cast_tmp"));
	  assign = gimple_build_assign_with_ops (CONVERT_EXPR,
						 t, promoted,
						 NULL_TREE);

	  name = make_ssa_name (t, assign);
	  gimple_assign_set_lhs (assign, name);
	  SET_PHI_ARG_DEF (phi, i, name);
	}

      if (assign != NULL)
	{
	  insert_along_edge (assign, e);
	  phi = *phi_p;
	}
    }

  /* Continue traversal.  */
  return true;
}

/* Free PI_ and its associated data.  */

static bool
free_pi_entries (const void *pi_, void *data ATTRIBUTE_UNUSED)
{
  struct promote_info *pi = CONST_CAST (struct promote_info *,
                                       (const struct promote_info *) pi_);

  VEC_free (tree, heap, pi->cast_types);
  VEC_free (int, heap, pi->cast_counts);
  free (pi);

  /* Continue traversal.  */
  return true;
}

/* Collect information about variables that we believe to be loop
   indices in PROMOTION_INFO.  */

static void
collect_promotion_candidates (void)
{
  loop_iterator li;
  struct loop *loop;

  FOR_EACH_LOOP (li, loop, 0)
    {
      basic_block header = loop->header;
      gimple exit_cond = last_stmt (header);
      
      if (exit_cond && gimple_code (exit_cond) == GIMPLE_COND)
       {
         tree loop_index;
         tree limit = NULL_TREE;
         tree decl;
         struct promote_info *pi;

         loop_index = find_promotion_candidate (loop, exit_cond, &limit);
         if (loop_index == NULL_TREE)
           continue;
         decl = SSA_NAME_VAR (loop_index);
         if (TREE_ADDRESSABLE (decl))
           continue;

         if (dump_file)
           {
             fprintf (dump_file, "Found loop index ");
             print_generic_expr (dump_file, loop_index, 0);
             fprintf (dump_file, " involved in ");
             print_gimple_stmt (dump_file, exit_cond, 0, 0);
             fprintf (dump_file, "\n\n");
           }

         pi = XCNEW (struct promote_info);
         pi->loop = loop;
         pi->exit_expr = exit_cond;
         pi->loop_index_name = loop_index;
         pi->loop_limit = limit;
         pi->var_decl = decl;
         /* We think so, anyway...  */
         pi->can_be_promoted_p = true;
         pointer_set_insert (promotion_info, pi);
       }
      else if (dump_file)
       {
         fprintf (dump_file, "\nSkipping analysis of loop %d (header bb #%d)\n",
                  loop->num, loop->header->index);
         if (exit_cond)
           {
             fprintf (dump_file, "Exit condition was ");
             print_gimple_stmt (dump_file, exit_cond, 0, 0);
             fprintf (dump_file, "\n");
           }
       }
    }
}

/* Free memory associated with global variables that we used.  */

static void
pli_cleanup (void)
{
  if (phi_nodes_to_promote)
    {
      pointer_set_destroy (phi_nodes_to_promote);
      phi_nodes_to_promote = NULL;
    }
  if (promoted_stmts)
    {
      pointer_set_destroy (promoted_stmts);
      promoted_stmts = NULL;
    }
  if (variable_map)
    {
      pointer_map_destroy (variable_map);
      variable_map = NULL;
    }
  if (promotable_names)
    {
      pointer_set_destroy (promotable_names);
      promotable_names = NULL;
    }
  if (promotion_info)
    {
      pointer_set_traverse (promotion_info, free_pi_entries, NULL);
      pointer_set_destroy (promotion_info);
      promotion_info = NULL;
    }
}

/* The guts of the pass.  */

static unsigned int
promote_short_indices (void)
{
  bool did_something = false;
  bool changed;
  size_t max_iterations, i, n_promoted;

  promotion_info = pointer_set_create ();
  collect_promotion_candidates ();

  if (dump_file)
    fprintf (dump_file, "Found %d candidates for promotion\n",
            (int) pointer_set_n_elements (promotion_info));

  /* Nothing to do.  */
  if (pointer_set_n_elements (promotion_info) == 0)
    goto cleanup;

  /* We have information about which variables are loop index variables.
     We now need to determine the promotability of the loop indices.
     Since the promotability of loop indices may depend on other loop
     indices, we need to repeat this until we reach a fixed point.  */
  changed = true;
  max_iterations = pointer_set_n_elements (promotion_info);
  i = 0;

  promotable_names = pointer_set_create ();

  while (changed)
    {
      changed = false;
      pointer_set_clear (promotable_names);
      pointer_set_traverse (promotion_info, add_variable,
                           promotable_names);
      n_promoted = pointer_set_n_elements (promotable_names);

      if (dump_file)
       fprintf (dump_file, "\nIteration %d, have %d variables to consider\n",
                (int) i, (int) n_promoted);

      if (n_promoted == 0)
       break;
      gcc_assert (i < max_iterations);
      pointer_set_traverse (promotion_info, analyze_loop, &changed);
      i++;
    }

  if (dump_file)
    fprintf (dump_file, "Promoting %d variables\n",
            (int) n_promoted);

  if (n_promoted != 0)
    {
      did_something = true;
      variable_map = pointer_map_create ();
      promoted_stmts = pointer_set_create ();
      phi_nodes_to_promote = pointer_set_create ();
      pointer_set_traverse (promotion_info, create_promoted_variable, NULL);
      pointer_set_traverse (promotion_info, promote_variable, NULL);
      pointer_set_traverse (phi_nodes_to_promote, promote_phi_node, NULL);
    }

 cleanup:
  pli_cleanup ();
  return did_something ? TODO_update_ssa : 0;
}

/* Entry point for the short loop index promotion pass.  */

static unsigned int
tree_short_index_promotion (void)
{
  unsigned int changed = 0;

  /* Initialize all the necessary loop infrastructure.  */
  loop_optimizer_init (LOOPS_HAVE_PREHEADERS | LOOPS_HAVE_SIMPLE_LATCHES | LOOPS_HAVE_RECORDED_EXITS);
  add_noreturn_fake_exit_edges ();
  connect_infinite_loops_to_exit ();

  if (number_of_loops () > 1)
    changed = promote_short_indices ();

  /* Tear down loop optimization infrastructure.  */
  remove_fake_exit_edges ();
  free_numbers_of_iterations_estimates ();
  loop_optimizer_finalize ();

  return changed;
}

static bool
gate_short_index_promotion (void)
{
  return optimize > 0 && flag_promote_loop_indices;
}

struct gimple_opt_pass pass_promote_indices =
{
  {
    GIMPLE_PASS,
    "promoteshort",                    /* name */
    gate_short_index_promotion,                /* gate */
    tree_short_index_promotion,                /* execute */
    NULL,                              /* sub */
    NULL,                              /* next */
    0,                                 /* static_pass_number */
    TV_TREE_LOOP_PROMOTE,              /* tv_id */
    PROP_cfg | PROP_ssa,               /* properties_required */
    0,                                 /* properties_provided */
    0,                                 /* properties_destroyed */
    0,                                 /* todo_flags_start */
    TODO_dump_func | TODO_verify_loops 
    | TODO_ggc_collect                 /* todo_flags_finish */
  }
};
