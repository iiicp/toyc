# -*- coding: utf-8 -*-
import os
import sys

CurDir = os.getcwd()
BuildDir = CurDir + "/../cmake-build-debug/"
Exe = "./ccc "

def build():
    if not os.path.exists(BuildDir):
        os.mkdir(BuildDir)

    os.chdir(BuildDir)
    os.system("cmake .. && make")


def testOne(filename):
    print('---------------')
    print('start {0} ...'.format(filename))
    path = CurDir + "/" + filename;
    cmd1 = Exe + path + "> tmp.s";
    cmd2 = "clang tmp.s ../test/common.c -o tmp.out"
    cmd3 = "./tmp.out"
    if (os.system(cmd1) != 0):
        exit()
    if (os.system(cmd2) != 0):
        exit()
    if (os.system(cmd3) != 0):
        exit()
    print('{0} ok!'.format(filename))

def testAll():
    file = ['arith.c', 'array.c', 'control.c', 'function.c',
            'pointer.c', 'struct.c', 'union.c', 'scope.c', 'declaration.c']

    for f in file:
        testOne(f)

    print('all done!!!')

if __name__ == '__main__':
    print ('开始测试: ', str(sys.argv))
    build()
    os.chdir(BuildDir)
    argLen = len(sys.argv)
    if argLen == 1:
        testAll()
    else:
        testOne(sys.argv[1])

