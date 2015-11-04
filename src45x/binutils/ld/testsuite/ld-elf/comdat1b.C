#include "comdat.H"

void b (int xx)
{
  test obj (xx);
  obj.print ();
  obj.increment ();
  obj.print ();
}
