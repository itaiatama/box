@echo off

set CC=clang
set CFLAGS=-Wall -Wextra -pedantic -std=c11 -ggdb
set SRC=./src/main.c ./src/value.c ./src/box.c ./src/lex.c ./src/pack.c
set OUT=box.exe

%CC% %CFLAGS% %SRC% -o %OUT% -D _CRT_SECURE_NO_WARNINGS