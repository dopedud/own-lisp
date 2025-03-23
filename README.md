# Lispy, A Programming Language from Scratch (written in C)

## About

This is the git repository for Lispy; a programming language created from scratch (well not really, it is written in
C). A functional programming language with the primary feature of treating everything as lists.

## Compilation Notes

`cc -std=c11 -Wall (name of program(s)) -o (name of executable)`

`gcc -std=c11 -Wall (name of program(s)) -o (name of executable)`

common use:

- `gcc -g -std=c11 -Wall main.c -o main`

- `gcc -g -std=c11 -Wall mpc/mpc.c definitions.c constdest.c printer.c reader.c builtins.c evaluator.c main.c -o lispy`