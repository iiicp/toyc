void assert(int expected, int actual);

int main() {
  assert(24, ({ int *x[3]; sizeof(x); }));
  assert(8, ({ int (*x)[3]; sizeof(x); }));
  assert(3, ({ int *x[3]; int y; x[0]=&y; y=3; x[0][0]; }));
  assert(4, ({ int x[3]; int (*y)[3]=x; y[0][0]=4; x[0]; }));
  assert(5, ({int a[3][4]; int (*p)[4] = &a[1]; p[0][0] = 5; a[1][0];}));
  return 0;
}