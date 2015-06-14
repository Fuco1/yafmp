#include "linebuffer.h"

LineBuffer* makeLineBuffer(int size) {
    char* buffer = malloc(sizeof(char)*size);
    LineBuffer* lb = malloc(sizeof(LineBuffer));
    lb->size = size;
    lb->current = 0;
    lb->numberOfLines = 0;
    lb->buffer = buffer;
    return lb;
}

void destroyLineBuffer(LineBuffer** lb) {
    if ((*lb)->buffer) free((*lb)->buffer);
    if (*lb) free(*lb);
    *lb = NULL;
}

void appendLine(LineBuffer* lb, const char* line, int length) {
    int needRealloc = 0;
    while (lb->current + length + 1 >= lb->size) {
        lb->size *= 2;
        needRealloc = 1;
    }
    if (needRealloc) {
        lb->buffer = realloc(lb->buffer, sizeof(char) * lb->size);
    }
    strcpy(&lb->buffer[lb->current], line);
    lb->current += length + 1;
    lb->numberOfLines++;
}

void withLineBuffer(LineBuffer* lb, void (*cb)(const char*, int, void*), void* userData) {
    char* start = lb->buffer;
    for (int i = 0; i < lb->numberOfLines; i++) {
        int offset = strlen(start);
        cb(start, offset, userData);
        start += offset + 1;
    }
}

void printLine(const char* line, int _len, void* _userData) {
    printf("%s\n", line);
}

void printLineBuffer(LineBuffer* lb) {
    withLineBuffer(lb, printLine, NULL);
}
