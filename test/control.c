void assert(int expected, int actual);
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

  assert(18, ({int i = 0,j=0; int sum= 0;for (int i=0;i<10;i=i+1) {sum = i+j;j=j+1;} sum;}));

  assert(10, ({int i = 5,j=0; int sum= 0;for (int i=0;i<10;i=i+1) {sum = i+1;} sum;}));

  assert(0, ({int i = 5,j=0; int sum= 0; for (int i=0;i<10;i=i+1) {break; continue; sum = i+1;} sum;}));

  assert(3, ({int i = 0; for (; i < 10; i=i+1){if (i == 3) break;} i;}));
  assert(4, ({int i = 0; int j = 0; for (; i < 10; i=i+1){if (i > 3) continue; j=j+1;} j;}));
  assert(50, ({int i = 0, j = 0; while (i < 100) {if(i > 50) break; j = i; i=i+1;} j;}));
  assert(50, ({int i = 0, j = 0; do {if(i > 50) break; j = i; i=i+1;} while (i < 100); j;}));

  assert(3, ({ int i=5; for(i = 0;i<10;i=i+1) { if (i == 3) break; } i; }));
  assert(3, ({ int i=0; for(;i<10;i=i+1) { for (;;) break; if (i == 3) break; } i; }));

  assert(10, ({ int i=0; int j=0; for (;i<10;i=i+1) { if (i>5) continue; j=j+1; } i; }));
  assert(6, ({ int i=0; int j=0; for (;i<10;i=i+1) { if (i>5) continue; j=j+1; } j; }));

  assert(7, ({int a = 1; goto i; i: a = a+1; j:a = a +2; k : a = a+3; a;}));
  assert(6, ({int a = 1; goto b; a: a = a+1; b:a = a +2; c : a = a+3; a;}));
  assert(4, ({int a = 1; goto l; m: a = a+1; n:a = a +2; l : a = a+3; a;}));
  assert(3, ({int a = 1; switch (a) {case 1: {a=3;break;} case 2:{a=5;break;}default: break; } a;}));
  assert(9, ({int a = 1; switch (a) {case 1: {a=a+3;} case 2:{a=a+5;break;}default: break; } a;}));
  return 0;
}