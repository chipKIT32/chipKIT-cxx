/* Local static variable elimination pass.
   Copyright (C) 2007 Free Software Foundation, Inc.
   Contributed by Nathan Froyd <froydnj@codesourcery.com>

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

/* Converting static function-local variables to automatic variables.

   The motivating example is a function like:

   void
   foo (unsigned n)
   {
     static int var;
     unsigned i;

     for (i = 0; i != n; i++)
       {
         var = ...	

         do other things with var...
       }
   }

   Because VAR is static, doing things like code motion to loads and
   stores of VAR is difficult.  Furthermore, accesses to VAR are
   inefficient.  This pass aims to recognize the cases where it is not
   necessary for VAR to be static and modify the code so that later
   passes will do the appropriate optimizations.

   The criteria for a static function-local variable V in a function F
   being converted to an automatic variable are:

   1. F does not call setjmp; and
   2. V's address is never taken; and
   3. V is not declared volatile; and
   4. V is not used in any nested function;
   5. V is not an aggregate value (union, struct, array, etc.); and
   6. Every use of V is defined along all paths leading to the use.

   NOTE: For ease of implementation, we currently treat a function call
   as killing all previous definitions of static variables, since we
   could have:

   static void
   foo (...)
   {
     static int x;

     x = ...;       (1)

    f (...);        (2)

     ... = x;       (3)
   }

   The use at (3) needs to pick up a possible definition made by the
   call at (2).  If the call at (2) does not call back into 'foo',
   then the call is not a killing call.  We currently treat it as
   though it is.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"

#include "rtl.h"
#include "tm_p.h"
#include "hard-reg-set.h"
#include "obstack.h"
#include "basic-block.h"

#include "tree.h"
#include "gimple.h"
#include "hashtab.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "flags.h"
#include "timevar.h"
#include "tree-pass.h"

struct rls_decl_info
{
  /* The variable declaration.  */
  tree orig_var;

  /* Its index in rls_block_local_data.  */
  int index;

  /* Whether we can optimize this variable.  */
  bool optimizable_p;

  /* The new variable declaration, if we can optimize away the staticness
     of 'orig_var'.  */
  tree new_var;
};

/* Filled with 'struct rls_decl_info'; keyed off ORIG_VAR.  */
static htab_t static_variables;

struct rls_stmt_info
{
  /* The variable declaration.  */
  tree var;

  /* The statement in which we found a def or a use of the variable.  */
  gimple stmt;

  /* Whether STMT represents a use of VAR.  */
  bool use_p;

  /* A bitmap whose entries denote what variables have been defined
     when execution arrives at STMT.  This field is only used when
     USE_P is true.  */
  sbitmap defined;
};

/* Filled with 'struct rls_stmt_info'; keyed off STMT.  */
static htab_t defuse_statements;

static struct
{
  /* The number of static variables we found.  */
  size_t n_statics;

  /* The number of optimizable variables we found.  */
  size_t n_optimizable;
} stats;

struct rls_block_dataflow_data {
  /* A bitmap whose entries denote what variables have been defined on
     entry to this block.  */
  sbitmap defined_in;

  /* A bitmap whose entries denote what variables have been defined on
     exit from this block.  */
  sbitmap defined_out;
};

/* Parameters for the 'static_variables' hash table.  */

static hashval_t
rls_hash_decl_info (const void *x)
{
  return htab_hash_pointer
    ((const void *) ((const struct rls_decl_info *) x)->orig_var);
}

static int
rls_eq_decl_info (const void *x, const void *y)
{
  const struct rls_decl_info *a = (const struct rls_decl_info *) x;
  const struct rls_decl_info *b = (const struct rls_decl_info *) y;

  return a->orig_var == b->orig_var;
}

static void
rls_free_decl_info (void *info)
{
  free (info);
}

/* Parameters for the 'defuse_statements' hash table.  */

static hashval_t
rls_hash_use_info (const void *x)
{
  return htab_hash_pointer
    ((const void *) ((const struct rls_stmt_info *) x)->stmt);
}

static int
rls_eq_use_info (const void *x, const void *y)
{
  const struct rls_stmt_info *a = (const struct rls_stmt_info *) x;
  const struct rls_stmt_info *b = (const struct rls_stmt_info *) y;

  return a->stmt == b->stmt;
}

static void
rls_free_use_info (void *info)
{
  struct rls_stmt_info *stmt_info = (struct rls_stmt_info *) info;

  if (stmt_info->defined)
    sbitmap_free (stmt_info->defined);

  free (stmt_info);
}

/* Initialize data structures and statistics.  */

static void
rls_init (void)
{
  basic_block bb;

  /* We expect relatively few static variables, hence the small
     initial size for the hash table.  */
  static_variables = htab_create (8, rls_hash_decl_info,
                                  rls_eq_decl_info, rls_free_decl_info);

  /* We expect quite a few statements.  */
  defuse_statements = htab_create (128, rls_hash_use_info,
                                   rls_eq_use_info, rls_free_use_info);

  FOR_ALL_BB (bb)
    {
      struct rls_block_dataflow_data *data;

      data = XNEW (struct rls_block_dataflow_data);
      memset (data, 0, sizeof (*data));
      bb->aux = data;
    }

  stats.n_statics = 0;
  stats.n_optimizable = 0;
}

/* Free data structures.  */

static void
rls_done (void)
{
  basic_block bb;

  htab_delete (static_variables);
  htab_delete (defuse_statements);

  FOR_ALL_BB (bb)
    {
      struct rls_block_dataflow_data *data
	= (struct rls_block_dataflow_data *) bb->aux;

      gcc_assert (data);

      if (data->defined_in)
	sbitmap_free (data->defined_in);
      if (data->defined_out)
	sbitmap_free (data->defined_out);
      free (data);
      bb->aux = NULL;
    }
}


/* Doing the initial work to find static variables.  */

/* Examine the defining statement for VAR and determine whether it is a
   static variable we could potentially optimize.  If so, stick in it
   in the 'static_variables' hashtable.

   STMT is the statement in which a definition or use of VAR occurs.
   USE_P indicates whether VAR is used or defined in STMT.  Enter STMT
   into 'defuse_statements' as well for use during dataflow
   analysis.  */

static void
maybe_discover_new_declaration (tree var, gimple stmt, bool use_p)
{
  tree def_stmt = SSA_NAME_VAR (var);

  if (TREE_CODE (def_stmt) == VAR_DECL
      && DECL_CONTEXT (def_stmt) != NULL_TREE
      && TREE_CODE (DECL_CONTEXT (def_stmt)) == FUNCTION_DECL
      /* We cannot optimize away a static used in multiple functions (as
	 might happen in C++).  */
      && !DECL_NONLOCAL(def_stmt)
      && TREE_STATIC (def_stmt)
      /* We cannot optimize away aggregate statics, as we would have to
	 prove that definitions of every field of the aggregate dominate
	 uses.  */
      && !AGGREGATE_TYPE_P (TREE_TYPE (def_stmt))
      /* GCC doesn't normally treat vectors as aggregates; we need to,
	 though, since a user could use intrinsics to read/write
	 particular fields of the vector, thereby treating it as an
	 array.  */
      && TREE_CODE (TREE_TYPE (def_stmt)) != VECTOR_TYPE
      /* We cannot optimize statics that were defined in another
	 function.  The static may be non-optimizable in its original
	 function, but optimizable when said function is inlined due to
	 DCE of its uses.  (e.g. the only use was in a return statement
	 and the function is inlined in a void context.)  */
      && DECL_CONTEXT (def_stmt) == current_function_decl
      && !TREE_ADDRESSABLE (def_stmt)
      && !TREE_THIS_VOLATILE (def_stmt))
    {
      struct rls_decl_info dummy;
      void **slot;

      dummy.orig_var = def_stmt;
      slot = htab_find_slot (static_variables, &dummy, INSERT);

      if (*slot == NULL)
        {
          /* Found a use or a def of a new declaration.  */
          struct rls_decl_info *info = XNEW (struct rls_decl_info);

          info->orig_var = def_stmt;
          info->index = stats.n_statics++;
          /* Optimistically assume that we can optimize.  */
          info->optimizable_p = true;
          info->new_var = NULL_TREE;
          *slot = (void *) info;
        }

      /* Enter the statement into DEFUSE_STATEMENTS.  */
      {
        struct rls_stmt_info dummy;
        struct rls_stmt_info *info;

        dummy.stmt = stmt;
        slot = htab_find_slot (defuse_statements, &dummy, INSERT);

        /* We should never insert the same statement into the
           hashtable twice.  */
        gcc_assert (*slot == NULL
		    || ((struct rls_stmt_info *)(*slot))->stmt == stmt);

	if (*slot != NULL && ((struct rls_stmt_info *)(*slot))->stmt == stmt)
	  return;

        info = XNEW (struct rls_stmt_info);
        info->var = def_stmt;
        info->stmt = stmt;
        if (dump_file)
          {
            fprintf (dump_file, "entering as %s ", use_p ? "use" : "def");
            print_gimple_stmt (dump_file, stmt, 0, TDF_DETAILS | TDF_VOPS);
          }
        info->use_p = use_p;
        /* We don't know how big to make the bitmap yet.  */
        info->defined = NULL;
        *slot = (void *) info;
      }
    }
}

/* Grovel through all the statements in the program, looking for
   SSA_NAMEs whose SSA_NAME_VAR is a VAR_DECL.  We look at both use and
   def SSA_NAMEs.  */

static void
find_static_nonvolatile_declarations (void)
{
  basic_block bb;

  FOR_EACH_BB (bb)
    {
      gimple_stmt_iterator i;

      for (i = gsi_start_bb (bb); !gsi_end_p (i); gsi_next (&i))
        {
          tree var;
          ssa_op_iter iter;
	  gimple stmt = gsi_stmt (i);

	  /* If there's a call expression in STMT, then previous passes
	     will have determined if the call transitively defines some
	     static variable.  However, we need more precise
	     information--we need to know whether static variables are
	     live out after the call.

	     Since we'll never see something like:

	       staticvar = foo (bar, baz);

	     in GIMPLE (the result of the call will be assigned to a
	     normal, non-static local variable which is then assigned to
	     STATICVAR in a subsequent statement), don't bother finding
	     new declarations if we see a GIMPLE_CALL.
	     
	     In a similar fashion, asm statements that clobber memory
	     will appear to define static variables.  In general,
	     however, assuming that asm statements define static
	     variables would cause us to see that in the following
	     situation:

	       static int foo = 0;

	       __asm__ (... : "memory");
	       foo++;

	     foo could be unstaticized because the asm has "defined"
	     foo.  This is likely false.  (Even if the asm does happen
	     to define foo--and only foo--that situation would be
	     sufficiently unusual that not optimizing it seems OK.)  */
	  if (gimple_code (stmt) != GIMPLE_CALL
	      && gimple_code (stmt) != GIMPLE_ASM)
	    FOR_EACH_SSA_TREE_OPERAND (var, stmt, iter, SSA_OP_ALL_DEFS)
	      {
		maybe_discover_new_declaration (var, stmt, false);
	      }

          FOR_EACH_SSA_TREE_OPERAND (var, stmt, iter, SSA_OP_ALL_USES)
            {
              maybe_discover_new_declaration (var, stmt, true);
            }
        }
    }
}


/* Determining if we have anything to optimize.  */

/* Examine *SLOT (which is a 'struct rls_decl_info *') to see whether
   the associated variable is optimizable.  If it is, create a new,
   non-static declaration for the variable; this new variable will be
   used during a subsequent rewrite of the function.  */

#define NEW_VAR_PREFIX ".unstatic"

static int
maybe_create_new_variable (void **slot, void *data ATTRIBUTE_UNUSED)
{
  struct rls_decl_info *info = (struct rls_decl_info *) *slot;
  tree id_node = DECL_NAME (info->orig_var);
  size_t id_len = IDENTIFIER_LENGTH (id_node);
  size_t name_len = id_len + strlen (NEW_VAR_PREFIX) + 1;
  char *name;

  /* Don't create a new variable multiple times.  */
  gcc_assert (!info->new_var);

  /* Tie the new name to the old one to aid debugging dumps.  */
  name = (char *) alloca (name_len);
  strcpy (name, IDENTIFIER_POINTER (id_node));
  strcpy (name + id_len, NEW_VAR_PREFIX);
  info->new_var = create_tmp_var (TREE_TYPE (info->orig_var), name);

  if (dump_file)
    {
      fprintf (dump_file, "new variable ");
      print_generic_expr (dump_file, info->new_var, 0);
      fprintf (dump_file, "\n");
    }

  /* Inform SSA about this new variable.  */
  create_var_ann (info->new_var);
  mark_sym_for_renaming (info->new_var);
  /* We need to make sure we rebuild bits for the original variable,
     such as virtual operands attached to statements.  */
  mark_sym_for_renaming (info->orig_var);
  add_referenced_var (info->new_var);

  /* Always continue scanning.  */
  return 1;
}

#undef NEW_VAR_PREFIX

/* Traverse the 'defuse_statements' hash table.  For every use,
   determine if the associated variable is defined along all paths
   leading to said use.  Remove the associated variable from
   'static_variables' if it is not.  */

static int
check_definedness (void **slot, void *data ATTRIBUTE_UNUSED)
{
  struct rls_stmt_info *info = (struct rls_stmt_info *) *slot;
  struct rls_decl_info dummy;
  struct rls_decl_info *decl;

  /* We don't need to look at definitions.  Continue scanning.  */
  if (!info->use_p)
    return 1;

  if (dump_file)
    {
      fprintf (dump_file, "defined bitmap for ");
      print_gimple_stmt (dump_file, info->stmt, 0, 0);
      fprintf (dump_file, " : ");
      dump_sbitmap (dump_file, info->defined);
    }

  dummy.orig_var = info->var;
  slot = htab_find_slot (static_variables, &dummy, NO_INSERT);

  gcc_assert (slot);

  decl = (struct rls_decl_info *) *slot;

  if (dump_file)
    {
      fprintf (dump_file, "with index %d for var ", decl->index);
      print_generic_expr (dump_file, decl->orig_var, 0);
      fprintf (dump_file, "\n\n");
    }

  if (decl->optimizable_p && !TEST_BIT (info->defined, decl->index))
    {
      if (dump_file)
	{
	  fprintf (dump_file, "not optimizing ");
	  print_generic_expr (dump_file, decl->orig_var, 0);
	  fprintf (dump_file, " due to uncovered use in ");
	  print_gimple_stmt (dump_file, info->stmt, 0, 0);
	  fprintf (dump_file, "\n");
	}

      decl->optimizable_p = false;
      stats.n_optimizable--;
    }

  /* Continue scan.  */
  return 1;
}

/* Check all statements in 'defuse_statements' to see if all the
   statements that use a static variable have that variable defined
   along all paths leading to the statement.  Once that's done, go
   through and create new, non-static variables for any static variables
   that can be optimized.  */

static size_t
determine_optimizable_statics (void)
{
  htab_traverse (defuse_statements, check_definedness, NULL);

  htab_traverse (static_variables, maybe_create_new_variable, NULL);

  return stats.n_optimizable;
}

/* Look at STMT to see if we have uses or defs of a static variable.
   STMT is passed in DATA.  Definitions of a static variable are found
   by the presence of a V_MUST_DEF, while uses are found by the presence
   of a VUSE.  */

static int
unstaticize_variable (void **slot, void *data)
{
  struct rls_decl_info *info = (struct rls_decl_info *) *slot;
  gimple stmt = (gimple) data;
  tree vdef;
  tree vuse;
  int continue_scan = 1;

  /* Can't do anything to this variable.  */
  if (!info->optimizable_p)
    goto done;

  /* Check for virtual definitions first.  */
  vdef = SINGLE_SSA_TREE_OPERAND (stmt, SSA_OP_VDEF);

  if (vdef != NULL
      && ZERO_SSA_OPERANDS (stmt, SSA_OP_DEF)
      && gimple_code (stmt) == GIMPLE_ASSIGN
      && TREE_CODE (gimple_assign_lhs (stmt)) == VAR_DECL
      && gimple_assign_lhs(stmt) == info->orig_var)
    {
      /* Make the statement define the new name.  The new name has
         already been marked for renaming, so no need to do that
         here.  */
      gimple_assign_set_lhs (stmt, info->new_var);
      if (dump_file)
	{
	  fprintf (dump_file, "found virtual definition!\n");
	  print_gimple_stmt (dump_file, stmt, 0, TDF_VOPS | TDF_DETAILS);
	  fprintf (dump_file, "\n");
	}
      continue_scan = 0;
      goto done;
    }

  /* Check for virtual uses.  */
  vuse = SINGLE_SSA_TREE_OPERAND (stmt, SSA_OP_VUSE);

  if (vuse != NULL
      && gimple_code (stmt) == GIMPLE_ASSIGN
      && gimple_assign_rhs_code (stmt) == VAR_DECL
      && gimple_assign_rhs1 (stmt) == info->orig_var)
    {
      /* Make the statement use the new name.  */
      gimple_assign_set_rhs1 (stmt, info->new_var);
      if (dump_file)
	{
	  fprintf (dump_file, "found virtual use!\n");
	  print_gimple_stmt (dump_file, stmt, 0, TDF_VOPS | TDF_DETAILS);
	  fprintf (dump_file, "\n");
	}
      continue_scan = 0;
      goto done;
    }

 done:
  if (!continue_scan)
    {
      /* None of the other optimizable static variables can occur
         in this statement.  Stop the scan.  */
      update_stmt (stmt);

      if (dump_file)
	{
	  fprintf (dump_file, "updated stmt\n");
	  print_gimple_stmt (dump_file, stmt, 0, TDF_VOPS | TDF_DETAILS);
	}
    }

  return continue_scan;
}

/* Determine if we have any static variables we can optimize.  If so,
   replace any defs or uses of those variables in their defining/using
   statements.  */

static void
maybe_remove_static_from_declarations (void)
{
  size_t n_optimizable = determine_optimizable_statics ();
  basic_block bb;

  if (n_optimizable)
    /* Replace any optimizable variables with new, non-static variables.  */
    FOR_EACH_BB (bb)
      {
        gimple_stmt_iterator gsi;

        for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
          {
            gimple stmt = gsi_stmt (gsi);

            htab_traverse (static_variables, unstaticize_variable, stmt);
          }
      }
}

/* Callback for htab_traverse to initialize the bitmap for *SLOT, which
   is a 'struct rls_stmt_info'.  */

static int
initialize_statement_dataflow (void **slot, void *data ATTRIBUTE_UNUSED)
{
  struct rls_stmt_info *info = (struct rls_stmt_info *) *slot;

  gcc_assert (!info->defined);

  if (info->use_p)
    {
      info->defined = sbitmap_alloc (stats.n_statics);
      /* Assume defined along all paths until otherwise informed.  */
      sbitmap_ones (info->defined);
    }

  /* Continue traversal.  */
  return 1;
}

/* We have N_STATICS static variables to consider.  Go through all the
   blocks and all the use statements to initialize their bitmaps.  */

static void
initialize_block_and_statement_dataflow (size_t n_statics)
{
  basic_block bb;

  FOR_ALL_BB (bb)
    {
      struct rls_block_dataflow_data *data
	= (struct rls_block_dataflow_data *) bb->aux;

      gcc_assert (data);

      data->defined_in = sbitmap_alloc (n_statics);
      sbitmap_zero (data->defined_in);
      data->defined_out = sbitmap_alloc (n_statics);
      sbitmap_zero (data->defined_out);
    }

  htab_traverse (defuse_statements, initialize_statement_dataflow, NULL);
}

/* Apply the individual effects of the stmts in BB to update the
   dataflow analysis information for BB.  */

static void
compute_definedness_for_block (basic_block bb)
{
  bool changed_p = false;
  struct rls_block_dataflow_data *data
	= (struct rls_block_dataflow_data *) bb->aux;
  gimple_stmt_iterator gsi;

  sbitmap_copy (data->defined_out, data->defined_in);

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);

      if (gimple_code (stmt) == GIMPLE_CALL)
	/* If there's a call expression in STMT, then previous passes
	   will have determined if the call transitively defines some
	   static variable.  However, we need more precise
	   information--we need to know whether static variables are
	   live out after the call.  In the absence of such information,
	   simply declare that all static variables are clobbered by the
	   call.  A better analysis would be interprocedural and compute
	   the liveness information we require, but for now, we're being
	   pessimistic.  */
	sbitmap_zero (data->defined_out);
      else 
	{
	  struct rls_stmt_info dummy;
	  void **slot;

	  /* See if this statement uses or defines a static variable.  */
	  dummy.stmt = stmt;
	  slot = htab_find_slot (defuse_statements, &dummy, NO_INSERT);

	  /* Check for uses.  */
	  if (slot)
	    {
	      struct rls_stmt_info *info = (struct rls_stmt_info *) *slot;

	      if (info->use_p)
		{
		  gcc_assert (info->defined);

		  /* Found a statement that uses a function-local static
		     variable.  Copy the current state of definedness.  */
		  sbitmap_copy (info->defined, data->defined_out);
		}
	      else
		{
		  struct rls_decl_info dummy;
		  struct rls_decl_info *decl;

		  gcc_assert (!info->defined);

		  /* Found a statement that defines a function-local static
		     variable.  Look up the associated variable's information
		     and mark it as defined in the block.  */
		  dummy.orig_var = info->var;
		  slot = htab_find_slot (static_variables, &dummy, NO_INSERT);

		  gcc_assert (slot);

		  decl = (struct rls_decl_info *) *slot;

		  SET_BIT (data->defined_out, decl->index);
		  changed_p |= true;
		}
	    }
	}
    }
}

/* Solve the dataflow equations:

   DEFINED_IN(b) = intersect DEFINED_OUT(p) for p in preds(b)
   DEFINED_OUT(b) = VARIABLES_DEFINED (b, DEFINED_IN (b))

   via a simple iterative solver.  VARIABLES_DEFINED is computed by
   'compute_definedness_for_block'.  */

static void
compute_definedness (void)
{
  basic_block bb;
  bool changed_p;
  sbitmap tmp_bitmap = sbitmap_alloc (stats.n_statics);

  /* Compute initial sets.  */
  FOR_EACH_BB (bb)
    {
      compute_definedness_for_block (bb);
    }

  /* Iterate.  */
  do {
    changed_p = false;

    FOR_EACH_BB (bb)
      {
        edge e;
        edge_iterator ei;
        struct rls_block_dataflow_data *data
	  = (struct rls_block_dataflow_data *) bb->aux;
        bool bitmap_changed_p = false;

        sbitmap_ones (tmp_bitmap);

        gcc_assert (data);

        /* We require information about whether a variable was defined
           over all paths leading to a particular use.  Therefore, we
           intersect the DEFINED sets of all predecessors.  */
        FOR_EACH_EDGE (e, ei, bb->preds)
          {
            struct rls_block_dataflow_data *pred_data
	      = (struct rls_block_dataflow_data *) e->src->aux;

            gcc_assert (pred_data);

            sbitmap_a_and_b (tmp_bitmap, tmp_bitmap, pred_data->defined_out);
          }

        bitmap_changed_p = !sbitmap_equal (tmp_bitmap, data->defined_in);

        if (bitmap_changed_p)
          {
            sbitmap_copy (data->defined_in, tmp_bitmap);
            compute_definedness_for_block (bb);
          }

        changed_p |= bitmap_changed_p;
      }
  } while (changed_p);

  sbitmap_free (tmp_bitmap);
}

static unsigned int
execute_rls (void)
{
  rls_init ();

  find_static_nonvolatile_declarations ();

  /* Can we optimize anything?  */
  if (stats.n_statics != 0)
    {
      stats.n_optimizable = stats.n_statics;

      if (dump_file)
        fprintf (dump_file, "found %d static variables to consider\n",
                 stats.n_statics);

      initialize_block_and_statement_dataflow (stats.n_statics);

      compute_definedness ();

      maybe_remove_static_from_declarations ();

      if (dump_file)
        fprintf (dump_file, "removed %d static variables\n",
                 stats.n_optimizable);
    }

  rls_done ();

  if (stats.n_optimizable > 0)
    return TODO_rebuild_alias | TODO_update_ssa;
  else
    return 0;
}

static bool
gate_rls (void)
{
  return (flag_remove_local_statics != 0
          && !cfun->calls_setjmp
          && !cgraph_node (current_function_decl)->ever_was_nested);
}

struct gimple_opt_pass pass_remove_local_statics =
{
  {
    GIMPLE_PASS,
    "remlocstatic",               /* name */
    gate_rls,                     /* gate */
    execute_rls,                  /* execute */
    NULL,                         /* sub */
    NULL,                         /* next */
    0,                            /* static_pass_number */
    TV_TREE_RLS,                  /* tv_id */
    PROP_cfg | PROP_ssa,          /* properties_required */
    0,                            /* properties_provided */
    0,                            /* properties_destroyed */
    0,                            /* todo_flags_start */
    TODO_dump_func | TODO_verify_ssa | TODO_verify_stmts
    | TODO_rebuild_alias | TODO_update_ssa /* todo_flags_finish */
  }
};
