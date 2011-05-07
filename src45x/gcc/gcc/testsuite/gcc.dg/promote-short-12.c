/* We used to miss getting both assignments to ret correct.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Inserting downcast" 2 "promoteshort" } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

int bar (void);

unsigned char
foo (int a)
{
  unsigned char i, j, ret = 0;

  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if (bar() && bar())
        {
          if (a)
            ret = i;
          else
            ret = j;
        }

  return ret;
}

