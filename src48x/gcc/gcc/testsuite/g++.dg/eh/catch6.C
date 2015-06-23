using namespace std;

extern "C" void abort ();

class except1 {
public:
    explicit except1 (int _i) {}
};

class except2 {
public:
    explicit except2 (int _i) {}
};

void
bar (int i)
{
  if (i & 1)
    throw except1 (i);
  throw except2 (i);
}

void
f1 (int i)
{
  try { bar (i); }
  catch (const except1& ex) { ; }
}

void
f2 (int i)
{
  try { f1 (i); }
  catch (const except2& ex) { ;; }
}

void
foo (int i)
{
  try { f2 (i); }
  catch (...)
    {
      abort ();
    }
}

int
main ()
{
  int i;
  for (i = 0; i < 50; i++)
    foo (i);
}
