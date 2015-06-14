#ifndef RESULT_H
#define RESULT_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Result {
    int index;
    int score;
    int* matches;
    struct Result* next;
} Result;

typedef struct {
    int length;
    Result* head;
    Result* last;
} Results;

Result* makeResult(int len, int score);

void destroyResult(Result** r);

Results* makeResults();

void destroyResults(Results** r);

void addResult(Results* results, Result* result);

void printResult(Result* result, int len);

#endif /* RESULT_H */
