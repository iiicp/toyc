int main()
{
  assert(5, ({int a = 4; if(1) a=5; a;}));
  assert(5, ({int a = 4; if(1) a=5; else a=6; a;}));
  assert(5, ({int a = 4; if(2>=1) a=5; else a=6; a;}));
  assert(6, ({int a = 4; if(0) a=5; else a=6; a;}));
  assert(6, ({int a = 4; if(1-1) a=5; else a=6; a;}));
  assert(6, ({int a = 4; if(1<1) a=5; else a=6; a;}));
  assert(6, ({int a = 4; if(1>=2) a=5; else a=6; a;}));
  assert(25, ({int a = 4; if(a) {a=5;a=a*a;} else a=6; a;}));
  assert(7, ({int a = 4; if(a) {a=5;a=a*a;} else a=6; a=7; a;}));
  assert(25, ({int a = 4; if(a) {a=5;a=a*a;} else {a=6; a=7;} a;}));
  assert(6, ({int a = 4; if(1>=2); else a=6; a;}));


  assert(0, ({int a=10; while (a) a = a-1; a;}));
  assert(10, ({int i=10; int s = 0; while (i) {i=i-1;s=s+1;} s;}));
  assert(45, ({int i=10; int s = 0; while (i) {i=i-1;s=s+i;} s;}));
  assert(4950, ({int i=0; int s = 0; while (i<100) {s=s+i;i=i+1;} s;}));

  assert(0, ({int a=10; do a = a-1; while (a); a;}));
  assert(10, ({int i=10; int s = 0; do {i=i-1;s=s+1;} while (i);  s;}));
  assert(45, ({int i=10; int s = 0; do {i=i-1;s=s+i;} while (i);  s;}));
  assert(4950, ({int i=0; int s = 0; do {s=s+i;i=i+1;} while (i<100);  s;}));

  assert(18, ({int i = 0,j=0; int sum= 0;for (i=0;i<10;i=i+1) {sum = i+j;j=j+1;} sum;}));
  assert(18, ({int i = 0,j=0; int sum= 0;for (;i<10;i=i+1) {sum = i+j;j=j+1;} sum;}));
  assert(18, ({int i = 0,j=0; int sum= 0;for (;i<10;) {sum = i+j;j=j+1;i=i+1;} sum;}));

  return 0;
}