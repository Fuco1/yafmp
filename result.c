#include "result.h"

Result* makeResult(int len, int score) {
    Result* r =  malloc(sizeof(Result));
    r->matches = malloc(sizeof(int) * len);
    r->score = score;
    r->next = NULL;
    return r;
}

void destroyResult(Result** r) {
    if (*r) {
        free((*r)->matches);
        free(*r);
    }
    *r = NULL;
}

Results* makeResults() {
    Results* r = malloc(sizeof(Results));
    r->length = 0;
    r->head = NULL;
    r->last = NULL;
    return r;
}

void destroyResults(Results** r) {
    if (*r) {
        Result* cur = (*r)->head;
        while (cur) {
            Result* next = cur->next;
            destroyResult(&cur);
            cur = next;
        }
        free(*r);
    }
    *r = NULL;
}

void addResult(Results* results, Result* result) {
    if (results->head == NULL) {
        results->head = result;
        results->last = result;
    } else {
        results->last->next = result;
        results->last = result;
    }
    results->length++;
}

void printResult(Result* result, int len) {
    printf("%d:%d:", result->index, result->score);
    for (int j = 0; j < len; j++) {
        printf("%d", result->matches[j]);
        if (j < len - 1) {
            printf(",");
        }
    }
    printf("\n");
}
