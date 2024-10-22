//
// Created by nur on 19.10.24.
//

#ifndef DYNBUFFER_H
#define DYNBUFFER_H

#include <stdbool.h>

// Dynamic buffer. Used to store strings of any length.
typedef struct {
    int capacity;
    int nextIdx;
    char *data;
} DynBuffer;

// Set size to less than 1 to use default size
int initDynBuffer(DynBuffer *buffer, int size);
void freeDynBuffer(DynBuffer *buffer);

bool isDynBufferEmpty(DynBuffer *buffer);
int appendDynBuffer(DynBuffer *buffer, char c);
int appendStringDynBuffer(DynBuffer *buffer, char *str);
int copyFromDynBuffer(DynBuffer *buffer, char **dest);
int emptyDynBuffer(DynBuffer *buffer);

#endif //DYNBUFFER_H
