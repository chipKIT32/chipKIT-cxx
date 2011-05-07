/* { dg-do link } */
/* { dg-options "-O -fpromote-loop-indices" } */

static int head = 0;

int
main ()
{
  char x, y;

  for (x = head; x < 2; x++)
    {
      y = x;
      switch (head)
	{
	case 0:
	  y = 0;
	default:
	  head = ++y;
	}
    }
  return head;
}
