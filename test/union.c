void assert(int expected, int actual);
int main() {

  assert(1, ({union {char c;} s;  sizeof(s);}));
  assert(4, ({union {char c; int a;} s;  sizeof(s);}));
  assert(8, ({union {char c; int a; long b;} s;  sizeof(s);}));
  assert(16, ({union {char c[3]; int a[4]; long b;} s;  sizeof(s);}));
  assert(16, ({union {char c[3]; int a[4]; long b; struct {char c; int a; long b;} s;} s;  sizeof(s);}));

  assert(2, ({union {char c;} s;  s.c = 2; s.c;}));
  assert(2, ({union {char c; int a;} s;  s.c = 1; s.a = 2; s.a;}));
  assert(2, ({union {char c; int a;} s;  s.c = 1; s.a = 2; s.c;}));
  assert(3, ({union {char c; int a; long b;} s;  s.c = 1; s.a = 2; s.b = 3; s.c;}));
  assert(3, ({union {char c; int a; long b;} s;  s.c = 1; s.a = 2; s.b = 3; s.a;}));
  assert(3, ({union {char c; int a; long b;} s;  s.c = 1; s.a = 2; s.b = 3; s.b;}));
  assert(4, ({union {char c[3]; int a[4]; long b;} s; s.c[0] = 1; s.c[1] = 2; s.c[2] = 3; s.a[0] = 4; s.c[0];}));
  assert(3, ({union {char c[3]; int a[4]; long b; struct {char c; int a; long b;} s;} s; s.s.c = 4; s.c[0] = 3; s.s.c;}));

  return 0;
}