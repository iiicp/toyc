100天开发c编译器

build step:
  1. cd c100/
  2. mkdir cmake-build-debug
  3. cd cmake-build-debug 
  4. cmake ..
  5. make
  6. ./c100 ../testcode/test.c > tmp.s
  7. clang tmp.s ../src/driver.c -o tmp.out
  8. ./tmp.out

day1: 支持编译5+1-3*4/2   
day2: 支持编译5+(1-3)*4/2   
day3: 支持编译a=3;5+a*4/2-a;      
day4: 支持错误提示, eg 输入"4+3"会提示 ';' expected    
day5: 编译 a=3;a==3;a!=3;a>3;a>=3;a<3;a<=3;    
day6: 能编译如下if语句 
a=3; if (a!=4) a = 5;     
a=3; if (a!=4) a = 5; else a = 6;     
a=3; if (a!=4) {a = 5;a = a + a;} else {a = a*a; a = a + 10;}
day7: 编译 a=0; b=1; while (a < 10) {a = a+1; b=a+b;} b;
day8: 支持文件读入，编译do-while和for语句
day9: 支持函数定义, 现在能编译如下语句
day10: 支持函数调用，支持递归，能编译
day11: 支持类型，函数类型，变量声明语句，能编译正式的c代码了
day12: 调整诊断代码，支持如下的错误诊断.
../testcode/test.c:12:18     res = res + i;a;
                                           ^undefined variable

