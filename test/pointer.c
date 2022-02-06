void assert(int expected, int actual);
int main() {
  assert(3, ({ int x=3; *&x; }));
  assert(3, ({ int x=3; int *y=&x; int **z=&y; **z; }));

  assert(5, ({ int x=3; int *y = &x; *y = 5; x;}));
  assert(5, ({ int x=3; int y = 4; int *z = &x; *(z-1) = 5; y;}));
  // 4 + 4 + 8 = 16

  assert(5, ({ int x=3; int y = 4; int *z = &y; *(z+1) = 5; x;}));
  assert(5, ({ int x=3; int y = 4; int *z = &y; *(1+z) = 5; x;}));
  assert(5, ({ int x=3; int y = 4; int *z = &x; *(-1+z) = 5; y;}));
  assert(1, ({ int x=3; int y = 4; int *z = &y; int *k = &x; k-z;}));
  assert(-1, ({ int x=3; int y = 4; int *z = &x; int *k = &y; k-z;}));
  assert(5, ({ int x=3; int y = 4; *(&y+1) = 5; x;}));

  assert(4, ({ int x=3; int *y = &x;  sizeof(x);}));
  assert(8, ({ int x=3; int *y = &x;  sizeof(y);}));
  assert(8, ({ int x=3; int *y = &x;  sizeof y;}));
  assert(8, ({ int x=3; int *y = &x;  sizeof 10;}));
  assert(8, ({ int x=3; int *y = &x;  sizeof -10;}));

  assert(1, ({char x; sizeof(x);}));
  assert(2, ({short x; sizeof(x);}));
  assert(4, ({int x; sizeof(x);}));
  assert(8, ({long x; sizeof(x);}));

  assert(-23, ({int x = 4567456745; char *p = &x; *p;}));
  assert(-37, ({int x = 4567456745; char *p = &x; *(p+1);}));
  assert(61, ({int x = 4567456745; char *p = &x; *(p+2);}));
  assert(16, ({int x = 4567456745; char *p = &x; *(p+3);}));

  assert(-9239, ({int x = 4567456745; short *p = &x; *p;}));
  assert(4157, ({int x = 4567456745; short *p = &x; *(p+1);}));

  return 0;
}