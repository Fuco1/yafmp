#include "state.h"

State* makeState(const char* pattern) {
    State* s = malloc(sizeof(State));
    int len = strlen(pattern);
    s->pattern = malloc(sizeof(char)*(len + 1));
    strcpy(s->pattern, pattern);
    s->patternLen = len;
    s->currentIndex = 0;
    s->currentResult = 0;
    s->results = makeResults();
    return s;
}

void destroyState(State** s) {
    if (*s) {
        free((*s)->pattern);
        destroyResults(&(*s)->results);
        free(*s);
    }
    *s = NULL;
}

void printState(State* s) {
    Result* cur = s->results->head;
    for (int i = 0; i < s->results->length; i++) {
        printResult(cur, s->patternLen);
        cur = cur->next;
    }
}

void printStateLisp(State* s) {
    int len = strlen(s->pattern);
    printf("(quote \n");
    Result* cur = s->results->head;
    for (int i = 0; i < s->results->length; i++) {
        if (i == 0) {
            printf(" (");
        } else {
            printf("  ");
        }
        printf("(:index %d :score %d :matches (", cur->index, cur->score);
        for (int j = 0; j < len; j++) {
            printf("%d", cur->matches[j]);
            if (j < len - 1) {
                printf(" ");
            }
        }
        printf("))");
        if (i < s->results->length - 1) {
            printf("\n");
        }
        cur = cur->next;
    }
    printf("))\n");
}
