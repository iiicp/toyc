void assert(int expected, int actual);
char add_char(char a, char b, char c,char d,char e,char f) {
  return a+b+c+d+e+f;
}

short add_short(short a, short b, short c,short d,short e,short f) {
  return a+b+c+d+e+f;
}

int add_int(int a, int b, int c,int d,int e,int f) {
  return a+b+c+d+e+f;
}

long add_long(long a, long b, long c,int d,long e,char f) {
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

int main() {
  assert(1+2+3+4+5+6, add_char(1,2,3,4,5,6));
  assert(1+2+3+4+5+6, add_short(1,2,3,4,5,6));
  assert(1+2+3+4+5+6, add_int(1,2,3,4,5,6));
  assert(1+2+3+4+5+6, add_long(1,2,3,4,5,6));
  assert(89, fib(sum(({int i = 4, j;}))));
  assert(17711, fib(sum(({int i = 4, j = 5; j * 2 - i;}))));
  return 0;
}