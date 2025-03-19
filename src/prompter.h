#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_STR_LENGTH
#define MAX_STR_LENGTH 2048
#endif

// if we are compiling on Windows compile these functions
#ifdef _WIN32

// fake readline function
char* readline(char* prompt) 
{
    printf(prompt);

    char* cpy = (char*)malloc(MAX_STR_LENGTH);
    fgets(cpy, MAX_STR_LENGTH, stdin);
    cpy[strlen(cpy) - 1] = '\0';

    return cpy;
}

// fake add_history function
void add_history(char* unused) {}

// otherwise include the editline headers
#else

#include <editline/readline.h>
#include <editline/history.h>

#endif