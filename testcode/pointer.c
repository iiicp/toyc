int main() {
  assert(3, ({ int x=3; *&x; }));
  assert(3, ({ int x=3; int *y=&x; int **z=&y; **z; }));

  assert(5, ({ int x=3; int *y = &x; *y = 5; x;}));
  assert(5, ({ int x=3; int y = 4; int *z = &x; *(z+1) = 5; y;}));
  assert(5, ({ int x=3; int y = 4; int *z = &y; *(z-1) = 5; x;}));
  assert(1, ({ int x=3; int y = 4; int *z = &y; int *k = &x; z-k;}));
  assert(-1, ({ int x=3; int y = 4; int *z = &x; int *k = &y; z-k;}));
  assert(5, ({ int x=3; int y = 4; *(&y-1) = 5; x;}));

  assert(8, ({ int x=3; int *y = &x;  sizeof(x);}));
  assert(8, ({ int x=3; int *y = &x;  sizeof(y);}));
  assert(8, ({ int x=3; int *y = &x;  sizeof y;}));
  assert(8, ({ int x=3; int *y = &x;  sizeof 10;}));
  assert(8, ({ int x=3; int *y = &x;  sizeof -10;}));
  return 0;
}