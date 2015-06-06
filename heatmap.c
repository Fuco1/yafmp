#include "heatmap.h"

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

void incHeatmap(int* heatmap, int from, int to, int delta) {
    for (int i = from; i < to; i++) {
        heatmap[i] += delta;
    }
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
    incHeatmap(heatmap, 0, len, -GROUP_NUMBER_PENALTY_MULTIPLIER * numberOfGroups);

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
