add(a,b,c,d,e,f) {
  return a+b+c+d+e+f;
}

ret() {
  return 100;
}

sum(n) {
  res = 0;
  for (i=0;i<=n;i=i+1) {
    res = res + i;
  }
  return res;
}

fib(n) {
    if (n <= 1) {
      return 1;
    }
    return fib(n-1) + fib(n-2);
}

prog() {
  return fib(20);
}