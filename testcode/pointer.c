int main() {
  assert(3, ({ int x=3; *&x; }));
  assert(3, ({ int x=3; int *y=&x; int **z=&y; **z; }));

  assert(5, ({ int x=3; int *y = &x; *y = 5; x;}));
  assert(5, ({ int x=3; int y = 4; int *z = &x; *(z+8) = 5; y;}));
  assert(5, ({ int x=3; int y = 4; int *z = &y; *(z-8) = 5; x;}));
  return 0;
}