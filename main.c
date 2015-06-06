#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "iarray.h"
#include "group.h"

#define INITIAL_PENALTY (-35)

#define BASEGROUP_BONUS 34
#define WORDSTART_BONUS 85

#define GROUP_NUMBER_PENALTY_MULTIPLIER 2
#define GROUP_POSITION_PENALTY 2
#define WORD_IN_GROUP_POSITION_PENALTY_MULTIPLIER 3

const char wordDelimiters[] = " -_:./\\";

Group* getGroups(const char* input, const char* groupDelimiters) {
    Group* g = makeGroup();
    addWord(g, 0);
    int i = 1;
    // 'c' lags one character behind where i points.  We need to check
    // *previous* character to decide if new word starts
    for (char c = input[0]; c != '\0'; c = input[i], i++) {
        // start of new group
        if (NULL != strchr(groupDelimiters, c)) {
            Group* next = makeGroup();
            next->prev = g;
            g = next;
            addWord(g, i);
            continue;
        }
        // start of new word
        if ((NULL != strchr(wordDelimiters, c)) || isupper(input[i])) {
            addWord(g, i);
        }
    }
    return g;
}

void incHeatmap(int* heatmap, int delta, int from, int to) {
    for (int i = from; i < to; i++) {
        heatmap[i] += delta;
    }
}

int max(int a, int b) {
    return a > b ? a : b;
}

int* makeHeatmap(const char* input, const char* groupDelimiters) {
    int len = strlen(input);
    int* heatmap = malloc(sizeof(int) * len);
    for (int i = 0; i < len; i++) {
        heatmap[i] = INITIAL_PENALTY;
    }

    Group* g = getGroups(input, groupDelimiters);

    int numberOfGroups = 0;
    Group* cur = g;
    while (cur != NULL) {
        numberOfGroups++;
        cur = cur->prev;
    }

    // Apply the "group count" penalty.  The more groups we have, the
    // less preferred the match should be.
    incHeatmap(heatmap, -GROUP_NUMBER_PENALTY_MULTIPLIER * numberOfGroups, 0, len);

    // Apply word penalties for words of each group
    int groupIndex = numberOfGroups;
    int groupEnd = len;
    cur = g;
    while (cur != NULL) {
        // Penalty for group position. Later groups are slightly preferred over later groups.
        int groupPositionPenalty = - GROUP_POSITION_PENALTY + groupIndex;
        // Apply the "basegroup" bonus.  Basegroup is the last group.  For
        // example, in file paths /bar/baz.txt, we want to give higher weight
        // to 'baz.txt' portion.
        if (groupIndex == numberOfGroups) {
            groupPositionPenalty = -(BASEGROUP_BONUS - numberOfEffectiveWords(cur) + numberOfGroups);
        }
        int wordStart;
        int wordEnd = groupEnd;
        int effectiveWordIndex = numberOfWords(cur) - 1;
        for (int i = effectiveWordIndex; i >= 0; i--) {
            wordStart = getWord(cur, i);
            int wordGroupPenalty = effectiveWordIndex * WORD_IN_GROUP_POSITION_PENALTY_MULTIPLIER;
            // Wordstart bonus.  For non-final groups we only consider effective words.
            if (groupIndex == numberOfGroups) {
                effectiveWordIndex--;
                heatmap[wordStart] += WORDSTART_BONUS;
            } else if (wordStart + 1 != wordEnd) {
                effectiveWordIndex--;
                heatmap[wordStart] += WORDSTART_BONUS;
            }
            for (int j = wordStart; j < wordEnd; j++) {
                heatmap[j] -=
                    groupPositionPenalty   // group index penalty
                    + wordGroupPenalty     // word index penalty
                    + (j + 1 - wordStart); // char index penalty
            }
            wordEnd = wordStart;
        }
        groupEnd = wordStart;
        cur = cur->prev;
        groupIndex--;
    }

    // free the resources
    while (g != NULL) {
        Group* d = g;
        g = g->prev;
        destroyGroup(d);
    }

    return heatmap;
}

void printArray(int* array, int len) {
    for (int i = 0; i < len; i++) {
        printf("%5d", array[i]);
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

void printTable(int rc,  int cc, int (*t)[cc]) {
    for (int r = 0; r < rc; r++) {
        for (int c = 0; c < cc; c++) {
            printf("%5d", t[r][c]);
        }
        printf("\n");
    }
    printf("\n");
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
    return 1;
}

int main() {
    /* DynamicArray* d = makeDynamicArray(100); */
    /* append(d, "foobar\n", 7); */
    /* print(d); */
    /* append(d, "bar\n", 4); */
    /* print(d); */
    // [43 -43 -44 -45 -46 40 -46 -47 -48 37 -49 -50 -51 79 -7 -8 -9 76 -10 -11 -11]
    // [43 -43 -44 -45 -46 40 -46 -47 -48 37 -49 -50 -51 79 76 73 -13 -14 -15 70 -16 -17 -17]
    char input[] = "AbcBbcCccabcAbcabcabcAbcBc";//"foo--bar-baz/qux-flux";
    int len = strlen(input);
    Group* g = getGroups(input, "/");
    //printGroup(g);
    //printGroup(g->prev);
    printf("\n");
    int* heatmap = makeHeatmap(input, "/");
    printArray(heatmap, len);
    printf("\n");
    score(input, "abcabc", heatmap);
    return 0;
}
