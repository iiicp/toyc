void assert(int expected, int actual);
int main(){
  assert(1, ({int a = 1; { int a; a = 5; }  a;}));
  assert(5, ({int a = 1; { a = 5; }  a;}));
  return 0;
}