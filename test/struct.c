void assert(int expected, int actual);
int main() {

  assert(1, ({struct {char c;} s;  sizeof(s);}));

  assert(8, ({struct {char c; int a;} s;  sizeof(s);}));
  assert(16, ({struct {char c; int a; long b;} s;  sizeof(s);}));
  assert(32, ({struct {char c[3]; int a[4]; long b;} s;  sizeof(s);}));
  assert(48, ({struct {char c[3]; int a[4]; long b; struct {char c; int a; long b;} s;} s;  sizeof(s);}));

  assert(0, ({struct {} s; sizeof s;}));

  assert(5, ({struct {char c;} s;  s.c = 5; s.c;}));

  assert(12, ({struct {char c; int a;} s;  s.c = 3; s.a = 4; s.a * s.c;}));
  assert(1, ({struct {char c; int a; long b;} s;  s.c = 1; s.a = 2; s.b = 3; s.c;}));
  assert(2, ({struct {char c; int a; long b;} s;  s.c = 1; s.a = 2; s.b = 3; s.a;}));
  assert(3, ({struct {char c; int a; long b;} s;  s.c = 1; s.a = 2; s.b = 3; s.b;}));
  assert(16, ({struct {char c[3]; int a[4]; long b;} s;  sizeof(s.a);}));
  assert(1, ({struct {char c[3]; int a[4]; long b;} s;  s.a[0] = 1; s.a[1] = 2; s.a[2] = 3; s.a[3] = 4; s.a[0];}));
  assert(2, ({struct {char c[3]; int a[4]; long b;} s;  s.a[0] = 1; s.a[1] = 2; s.a[2] = 3; s.a[3] = 4; s.a[1];}));
  assert(3, ({struct {char c[3]; int a[4]; long b;} s;  s.a[0] = 1; s.a[1] = 2; s.a[2] = 3; s.a[3] = 4; s.a[2];}));
  assert(4, ({struct {char c[3]; int a[4]; long b;} s;  s.a[0] = 1; s.a[1] = 2; s.a[2] = 3; s.a[3] = 4; s.a[3];}));

  assert(1, ({struct {char c[3]; int a[4]; long b; struct {char c; int a; long b;} s;} s;  s.s.c = 1; s.s.a = 2; s.s.b = 3; s.s.c;}));
  assert(2, ({struct {char c[3]; int a[4]; long b; struct {char c; int a; long b;} s;} s;  s.s.c = 1; s.s.a = 2; s.s.b = 3; s.s.a;}));
  assert(3, ({struct {char c[3]; int a[4]; long b; struct {char c; int a; long b;} s;} s;  s.s.c = 1; s.s.a = 2; s.s.b = 3; s.s.b;}));


  assert(5, ({struct A {char c;}; struct A s; s.c = 5; s.c;}));
  assert(30, ({struct A {char c, d;}; struct A s; s.c = 5; s.d = 6; s.c * s.d;}));
  assert(5, ({struct A {char c;} s; s.c = 5; s.c;}));
  assert(5, ({struct A {char c;} s; struct A *p = &s;  p->c = 5; s.c;}));
  assert(30, ({struct A {char c, d;} s; struct A *p = &s; p->c = 5; p->d = 6; s.c * s.d;}));
  return 0;
}