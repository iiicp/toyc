int add(int a, int b, int c,int d,int e,int f) {
  return a+b+c+d+e+f;
}

int ret() {
  return 100;
}

int sum(int n) {
  int res = 0, i = 0;
  for (i=0;i<=n;i=i+1) {
    res = res + i;
  }
  return res;
}

int fib(int n) {
    if (n <= 1) {
      return 1;
    }
    return fib(n-1) + fib(n-2);
}

int prog() {
  return fib(sum(4));
}