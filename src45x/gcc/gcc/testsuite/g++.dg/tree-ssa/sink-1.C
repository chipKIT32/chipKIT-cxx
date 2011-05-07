/* { dg-do run } */
/* { dg-options "-O1" } */

class A {
 public:
  A() {}
  virtual ~A() {}  
  void * dostuff();
  
  virtual int dovirtual() = 0;
};


class B : public A {
 public:
  B() {}
  int dovirtual() { return 0;}
  virtual ~B() {};
}; 

class C : public B {
 public:
  C() {}
  virtual ~C() {};
};

void* A::dostuff()
{
  return (void*)dovirtual();
}

/* tree-ssa-sink was sinking the inlined destructor for STUFF out of
   the first inner block and into the second one, where it was ending up
   after the inlined constructor for STUFF2.  This is bad because
   cfgexpand aliases STUFF and STUFF2 to the same storage at -O1
   (i.e., without -fstrict-aliasing), with the result that STUFF2's
   vtable was getting trashed. */

int main() {
  {
    B stuff;
    stuff.dostuff();
  }
  {
    C stuff2;
    stuff2.dostuff();
  }
  return 0;
}

