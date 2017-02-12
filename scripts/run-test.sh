#!/bin/sh

if [ -z "$1" ]; then
 echo "\nDumbass, give me a test name.\n"
 exit 1 
fi

if [ -f "test/${1}.c" ]; then
 TN="build/test/${1}.run"
elif [ -f "test/eval/${1}.c" ]; then
 TN="build/test/eval/${1}.run"
elif [ -f "test/common/${1}.c" ]; then
 TN="build/test/common/${1}.run"
elif [ -f "test/asm/${1}.c" ]; then
 TN="build/test/asm/${1}.run"
else
 echo "\nWhat about you giving me an existing test, stupid?\n"
 exit 2;
fi

CC=clang CFLAGS="-std=c89 -O0 -Wextra -Wno-unused-parameter" N_TEST=1 \
   N_DEBUG=1  ATEST_PREFIX=../atest/dist/ make ${TN}

./${TN} | sed -r 's/^([EF])\t([a-zA-Z_()0-9 ]+)\t([^\t]+)\t([^\t]+)\t[0-9]+\t[0-9]+\t(.+)/[\1] \2   (\3:\4)\n\5\n/ '
