#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iarray.h"
#include "group.h"
#include "heatmap.h"
#include "linebuffer.h"

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

int score(const char* input, const char* pattern, const int* heatmap) {
    int leni = strlen(input);
    int lenp = strlen(pattern);
    int (*t)[leni] = malloc(sizeof(int) * leni * lenp);
    int* max = malloc(sizeof(int) * leni);

    for (int i = 0; i < leni; i++) { max[i] = -1; }
    for (int r = 0; r < lenp; r++) {
        for (int c = 0; c < leni; c++) {
            t[r][c] = MIN_PENALTY;
        }
    }

    int endCol = leni;
    for (int r = lenp - 1; r >= 0; r--) {
        printArray(max, leni);
        int lastMatchIndex = -1;
        for (int c = endCol - 1; c >= 0; c--) {
            if (input[c] == pattern[r]) {
                if (lastMatchIndex == -1) {
                    lastMatchIndex = c;
                }
                int prev = 0;
                if (max[c] != -1) {
                    prev = t[r+1][max[c]];
                }
                t[r][c] = heatmap[c] + prev;
            }
        }
        // no possible match
        if (lastMatchIndex == -1) {
            return -1;
        }
        endCol = lastMatchIndex;
        updateMaxArray(max, t[r], leni);
        printArray(t[r], leni);
    }
    printf("\n");
    printTable(lenp, leni, t);
    free(t);
    free(max);
    return 0;
}

void processLine(const char* line, int len, void* userData) {
    int* heatmap = makeHeatmap(line, "/");
    printArray(heatmap, len);
    printf("\n");
    char* pattern = (char*) userData;
    score(line, pattern, heatmap);
}

int main() {
    // [43 -43 -44 -45 -46 40 -46 -47 -48 37 -49 -50 -51 79 -7 -8 -9 76 -10 -11 -11]
    // [43 -43 -44 -45 -46 40 -46 -47 -48 37 -49 -50 -51 79 76 73 -13 -14 -15 70 -16 -17 -17]
    //char input[] = "AbcBbcCccabcAbcabcabcAbcBc";
    /* char input[] = "foo--bar-baz/qax-flux"; */
    /*             // "foo--bar-baz/qax-flux" */

    LineBuffer* lb = makeLineBuffer(4000);
    char* input = NULL;
    size_t bufferLen = 0;
    int len;
    while ((len = getline(&input, &bufferLen, stdin)) > 1) {
        // remove the trailing newline
        if (len > 1) {
            len--;
            input[len] = '\0';
        }
        appendLine(lb, input, len);
    }

    printLineBuffer(lb);

    printf("\n");
    char pattern[] = "bbu";
    withLineBuffer(lb, &processLine, pattern);
    return 0;
}
