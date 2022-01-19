int main() {
  assert(3, ({ int x=3; *&x; }));
  assert(3, ({ int x=3; int *y=&x; int **z=&y; **z; }));
  return 0;
}