#include "group.h"

Group* makeGroup() {
    Group* group = malloc(sizeof(Group));
    group->words = makeIntArray(6);
    group->prev = NULL;
    return group;
}

int numberOfWords(Group* group) {
    return group->words->current;
}

int numberOfEffectiveWords(Group* group) {
    int n = 1;
    for (int i = 0; i < numberOfWords(group) - 1; i++) {
        if (getWord(group, i) + 1 != getWord(group, i + 1)) {
            n++;
        }
    }
    return n;
}

int groupStart(Group* group) {
    return group->words->data[0];
}

void addWord(Group* group, int pos) {
    pushInt(group->words, pos);
}

int getWord(Group* group, int index) {
    return getInt(group->words, index);
}

void printGroup(Group* group) {
    printIntArray(group->words);
}
