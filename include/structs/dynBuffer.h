//
// Created by nur on 19.10.24.
//

#ifndef DYNBUFFER_H
#define DYNBUFFER_H

// Dynamic buffer. Used to store strings of any length.
typedef struct {
    int capacity;
    int nextIdx;
    char *data;
} DynBuffer;

// Set size to less than 1 to use default size
int initDynBuffer(DynBuffer *buffer, int size);
void freeDynBuffer(DynBuffer *buffer);

int appendDynBuffer(DynBuffer *buffer, char c);
int copyFromDynBuffer(DynBuffer *buffer, char **dest);
int emptyDynBuffer(DynBuffer *buffer);

#endif //DYNBUFFER_H
