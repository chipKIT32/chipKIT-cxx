/* We used to ICE due to badness while rebuilding phi nodes.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Inserting downcast" 1 "promoteshort" } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

int bar (void);

unsigned char
foo (int a)
{
  unsigned char i, ret = 0;

  for (i = 0; i < 8; i++)
      if (bar() && bar())
        {
          if (a)
            ret = i;
        }

  return ret;
}

