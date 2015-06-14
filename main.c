#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include "iarray.h"
#include "group.h"
#include "heatmap.h"
#include "linebuffer.h"
#include "result.h"

void printArray(int* array, int len) {
    for (int i = 0; i < len; i++) {
        printf("%5d", array[i]);
    }
    printf("\n");
}

void printTable(int rc,  int cc, int (*t)[cc]) {
    for (int r = 0; r < rc; r++) {
        printArray(t[r], cc);
    }
    printf("\n");
}

#define MIN_PENALTY -9999;

void updateMaxArray(int* max, int* row, int len) {
    int cm = MIN_PENALTY;
    int cmi = -1;
    for (int i = len - 1; i >= 0 ; i--) {
        if (row[i] > cm) {
            cm = row[i];
            cmi = i;
        }
        max[i] = cmi;
    }
}

int smartCompare(char in, char p) {
    if (isupper(p)) {
        return in == p;
    }
    char inl = tolower(in);
    char pl = tolower(p);
    return inl == pl;
}

Result* score(const char* input, const char* pattern, const int* heatmap) {
    int leni = strlen(input);
    int lenp = strlen(pattern);
    int (*t)[leni] = malloc(sizeof(int) * leni * lenp);
    int (*max)[leni] = malloc(sizeof(int) * leni * (lenp + 1));

    for (int r = 0; r < lenp; r++) {
        for (int c = 0; c < leni; c++) {
            t[r][c] = MIN_PENALTY;
            max[r][c] = -1;
        }
    }
    for (int c = 0; c < leni; c++) {
        max[lenp][c] = -1;
    }

    int endCol = leni;
    for (int r = lenp - 1; r >= 0; r--) {
        int lastMatchIndex = -1;
        for (int c = endCol - 1; c >= 0; c--) {
            //if (input[c] == pattern[r]) {
            if (smartCompare(input[c], pattern[r])) {
                if (lastMatchIndex == -1) {
                    lastMatchIndex = c;
                }
                int prev = 0;
                if (max[r+1][c] != -1) {
                    prev = t[r+1][max[r+1][c]];
                }
                t[r][c] = heatmap[c] + prev;
            }
        }
        // no possible match
        if (lastMatchIndex == -1) {
            return NULL;
        }
        endCol = lastMatchIndex;
        updateMaxArray(max[r], t[r], leni);
    }

    Result* r = makeResult(lenp, t[0][max[0][0]]);

    int c = 0;
    for (int i = 0; i < lenp; i++) {
        c = max[i][c];
        r->matches[i] = c;
        c++;
    }

    free(t);
    free(max);
    return r;
}

// TODO: move to separate file, add constructor and destructor
typedef struct {
    char* pattern;
    int currentIndex;
    int currentResult;
    Result* results;
} State;

void processLine(const char* line, int len, void* userData) {
    State* state = (State*) userData;

    int* heatmap = makeHeatmap(line, "/");
    Result* s = score(line, state->pattern, heatmap);

    if (s != NULL) {
        Result* r = &state->results[state->currentResult];
        r->index = state->currentIndex;
        r->score = s->score;
        r->matches = s->matches;
        // do not free s->matches, it is shared with r!
        free(s);
        state->currentResult++;
    }
    state->currentIndex++;

    free(heatmap);
}

void printState(State* s) {
    int len = strlen(s->pattern);
    for (int i = 0; i < s->currentResult; i++) {
        printf("%d:%d:", s->results[i].index, s->results[i].score);
        for (int j = 0; j < len; j++) {
            printf("%d", s->results[i].matches[j]);
            if (j < len - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}

void printStateLisp(State* s) {
    int len = strlen(s->pattern);
    printf("(quote \n");
    for (int i = 0; i < s->currentResult; i++) {
        if (i == 0) {
            printf(" (");
        } else {
            printf("  ");
        }
        printf("(:index %d :score %d :matches (", s->results[i].index, s->results[i].score);
        for (int j = 0; j < len; j++) {
            printf("%d", s->results[i].matches[j]);
            if (j < len - 1) {
                printf(" ");
            }
        }
        printf("))");
        if (i < s->currentResult - 1) {
            printf("\n");
        }
    }
    printf("))\n");
}

/*
   We have two modes of operation.

   First (default) is the "grep" mode, where we specify the pattern as
   an argument on the command line and then read the stdin for
   patterns.  Instead of storing everything in memory we should
   resolve the lines right away so we won't overflow on large files.

   Second mode is a "server" mode.  First we tell the program how many
   candidates to expect and then we feed it that many lines.

   Afterwards, each line is a new pattern to match against the list of
   candidates read previously.  This is useful when the program is
   called from another program to do interactive
   filtering/narrowing---we save the time marshalling the candidates
   back and forth.
 */

static struct option longOptions[] = {
    {"pattern", required_argument, 0, 'e'},
    {"server", no_argument, 0, 's'},
    {0, 0, 0, 0}
};

typedef struct {
    int server;
    char* pattern;
} Options;
int main(int argc, char** argv) {
    // [43 -43 -44 -45 -46 40 -46 -47 -48 37 -49 -50 -51 79 -7 -8 -9 76 -10 -11 -11]
    // [43 -43 -44 -45 -46 40 -46 -47 -48 37 -49 -50 -51 79 76 73 -13 -14 -15 70 -16 -17 -17]
    //char input[] = "AbcBbcCccabcAbcabcabcAbcBc";
    /* char input[] = "foo--bar-baz/qax-flux"; */
    /*             // "foo--bar-baz/qax-flux" */

    Options opt = {
        0,
        NULL
    };

    int optInd;
    int c;
    while (1) {
        c = getopt_long(argc, argv, "se:", longOptions, &optInd);

        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            break;
        case 's':
            opt.server = 1;
            break;
        case 'e':
            opt.pattern = malloc(sizeof(char) * (strlen(optarg) + 1));
            strcpy(opt.pattern, optarg);
            break;
        case '?':
            break;
        default:
            abort();
        }
    }

    LineBuffer* lb = makeLineBuffer(4000);
    char* input = NULL;
    size_t bufferLen = 0;
    int len;
    // TODO: write a version of getline which returns strings without the trailing \n
    while ((len = getline(&input, &bufferLen, stdin)) > 1) {
        // remove the trailing newline
        if (len > 1) {
            len--;
            input[len] = '\0';
        }
        appendLine(lb, input, len);
    }

    // read the pattern
    len = getline(&input, &bufferLen, stdin);
    len--;
    input[len] = '\0';

    State state;
    Result results[lb->numberOfLines];
    state.pattern = input;
    state.results = results;
    state.currentResult = 0;
    state.currentIndex = 0;

    withLineBuffer(lb, &processLine, &state);
    // TODO: add an option to sort the results on score instead of index
    printStateLisp(&state);

    // free state and all associated buffers
    free(input);
    free(opt.pattern);
    opt.pattern = NULL;
    destroyLineBuffer(&lb);
    return 0;
}
