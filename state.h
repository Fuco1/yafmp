#ifndef STATE_H
#define STATE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "result.h"

typedef struct {
    char* pattern;
    int patternLen;
    int currentIndex;
    int currentResult;
    Results* results;
} State;

State* makeState(const char* pattern);

void destroyState(State** s);

void printState(State* s);

void printStateLisp(State* s);

#endif /* STATE_H */
