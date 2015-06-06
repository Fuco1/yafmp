#ifndef LINEBUFFER_H
#define LINEBUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int size;
    int current;
    int numberOfLines;
    char* buffer;
} LineBuffer;

LineBuffer* makeLineBuffer(int size);

void destroyLineBuffer(LineBuffer* lb);

void appendLine(LineBuffer* lb, char* text, int length);

void printLineBuffer(LineBuffer* lb);

// line, length, user data
void withLineBuffer(LineBuffer* lb, void (*cb)(const char*, int, void*), void* userData);

#endif /* LINEBUFFER_H */
