class test {
 public:
  int x;
  test () { x = 0; }
  test (int xx) { x = xx; }
  int increment () { return ++x; }
  void print ();
};

extern test *maketest (int);

extern void a (int);
extern void b (int);

