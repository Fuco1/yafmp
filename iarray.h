#ifndef IARRAY_H
#define IARRAY_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Dynamic array holding integer values.
 */
typedef struct {
    /** Size of the available buffer. */
    int size;
    /** Current position.  The index where next item is placed. */
    int current;
    /** Data */
    int* data;
} IntArray;

/**
 * Create new integer array.
 *
 * @param Initial size of the buffer.
 * @return New dynamic integer array.
 */
IntArray* makeIntArray(int size);

/**
 * Destroy integer array, releasing all used memory.
 *
 * @param Array to destroy.
 */
void destroyIntArray(IntArray* array);

/**
 * Push new item into the array.
 *
 * @param Pointer to a dynamic array.
 * @param New value to insert at the end.
 */
void pushInt(IntArray* array, int value);

/**
 * Get value at index.
 *
 * @param Pointer to dynamic array.
 * @param Index.
 * @return Value at index.
 */
int getInt(IntArray* array, int index);

void printIntArray(IntArray* array);

#endif /* IARRAY_H */
