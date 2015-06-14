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
#include "state.h"

typedef struct {
    int server;
    char* pattern;
} Options;

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

    Result* result = NULL;
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
            goto end;
        }
        endCol = lastMatchIndex;
        updateMaxArray(max[r], t[r], leni);
    }

    result = makeResult(lenp, t[0][max[0][0]]);

    int c = 0;
    for (int i = 0; i < lenp; i++) {
        c = max[i][c];
        result->matches[i] = c;
        c++;
    }

    end:
    free(t);
    free(max);
    return result;
}

void processLine(const char* line, int len, void* userData) {
    if (len == 0) return;

    State* state = (State*) userData;

    int* heatmap = makeHeatmap(line, "/");
    Result* res = score(line, state->pattern, heatmap);

    if (res != NULL) {
        res->index = state->currentIndex;
        addResult(state->results, res);
        state->currentResult++;
    }
    state->currentIndex++;

    free(heatmap);
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

void processOnTheFly(Options* opt) {
    char* input = NULL;
    size_t bufferLen = 0;
    int len;

    State* state = makeState(opt->pattern);

    int lastResult = 0;
    while ((len = getline(&input, &bufferLen, stdin)) != -1) {
        len--;
        input[len] = '\0';
        processLine(input, len, state);
        if (state->currentResult > lastResult) {
            printResult(state->results->last, state->patternLen);
        }
        lastResult = state->currentResult;
    }

    free(input);
    destroyState(&state);
}

LineBuffer* readLines() {
    LineBuffer* lb = makeLineBuffer(4000);
    char* input = NULL;
    size_t bufferLen = 0;
    int len;

    int n;
    scanf("%d\n", &n);

    for (int i = 0; i < n; i++) {
        len = getline(&input, &bufferLen, stdin);
        // remove the trailing newline
        len--;
        input[len] = '\0';
        appendLine(lb, input, len);
    }

    free(input);
    return lb;
}

/**
 * Read pattern from stdin if not specified as an option
 *
 * @param Options
 */
void readPattern(Options* opt) {
    int len;
    size_t bufferLen = 0;
    if (opt->pattern == NULL) {
        bufferLen = 0;
        len = getline(&opt->pattern, &bufferLen, stdin);
        opt->pattern[len-1] = '\0';
    }
}

void processServer(Options* opt) {
    LineBuffer* lb = readLines();
    readPattern(opt);

    State* state = makeState(opt->pattern);

    withLineBuffer(lb, &processLine, state);

    printState(state);

    destroyLineBuffer(&lb);
    destroyState(&state);
}

// add --print-index, --print-score, --print-matches, --print-line
// add sorting (by score, desc/asc)
static struct option longOptions[] = {
    {"pattern", required_argument, 0, 'e'},
    {"server", no_argument, 0, 's'},
    {0, 0, 0, 0}
};

int main(int argc, char** argv) {

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

    if (opt.server == 0 && opt.pattern == NULL) {
        printf("When not in server mode, the pattern must be specified using -e\n");
        return -1;
    }

    if (opt.server == 1) {
        processServer(&opt);
    } else {
        processOnTheFly(&opt);
    }

    // free state and all associated buffers
    free(opt.pattern);
    return 0;
}
