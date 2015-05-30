#include "iarray.h"

IntArray* makeIntArray(int size) {
    IntArray* array = malloc(sizeof(IntArray));
    array->size = size;
    array->current = 0;
    array->data = malloc(sizeof(int) * size);
    return array;
}

void pushInt(IntArray* array, int value) {
    if (array->size == array->current) {
        array->size *= 2;
        array->data = realloc(array->data, sizeof(int) * array->size);
    }
    array->data[array->current] = value;
    array->current++;
}

int getInt(IntArray* array, int index) {
    return array->data[index];
}

void printIntArray(IntArray* array) {
    printf("[");
    for (int i = 0; i < array->current - 1; i++) {
        printf("%d, ", array->data[i]);
    }
    printf("%d]", array->data[array->current - 1]);
}
