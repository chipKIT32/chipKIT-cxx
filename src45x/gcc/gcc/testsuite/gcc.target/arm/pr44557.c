/* { dg-do compile } */
/* { dg-options "-mthumb -O1 -march=armv5te -fno-omit-frame-pointer -fno-forward-propagate" }  */
/* { dg-require-effective-target arm_thumb1_ok } */

struct S
{
  short x, y;
};

void foo (struct S *p, struct S *q, char *t, int n)
{
  struct S *c, d;
  int x = 1;

  while (n--)
    {
      if (*t && p)
      c = p;
      q->x = d.x + c->x + c->y;
      if (x)
      {
        x = 0;
	   d.x += c->x;
	   }
    }
}
