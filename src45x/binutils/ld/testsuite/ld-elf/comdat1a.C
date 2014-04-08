#include "comdat.H"

void a (int xx)
{
  test obj (xx);
  obj.print ();
  obj.increment ();
  obj.print ();
}

