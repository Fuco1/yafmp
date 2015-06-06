#ifndef GROUP_H
#define GROUP_H

#include <stdio.h>
#include <stdlib.h>

#include "iarray.h"

/**
 * Structure holding information about a word group.
 */
typedef struct Group {
    /** Start positions of words of this group, in ascending order. */
    IntArray* words;
    /** Pointer to previous group. */
    struct Group* prev;
} Group;

/**
 * Create new empty group.
 *
 * @return New group.
 */
Group* makeGroup();

/**
 * Destroy group, releasing all used memory.
 *
 * @param Group to destroy.
 */
void destroyGroup(Group* array);

/**
 * Return number of words in this group.
 *
 * @param Group.
 * @return Number of words in this group.
 */
int numberOfWords(Group* group);

/**
 * Return number of effective words.
 *
 * An effective word is a word of non-zero length (inbetween two
 * delimiters).
 * @param Group.
 * @return Number of effective words.
 */
int numberOfEffectiveWords(Group* group);

/**
 * Return the initial position of the first word in this group.
 *
 * @param Group.
 * @return Position of the initial word.
 */
int groupStart(Group* group);

/**
 * Add new word.
 *
 * @param Group.
 * @param Word-starting position.
 */
void addWord(Group* group, int pos);

/**
 * Get word at position.
 *
 * @param Group.
 * @param Position of word in group.
 */
int getWord(Group* group, int index);

void printGroup(Group* group);

#endif /* GROUP_H */
