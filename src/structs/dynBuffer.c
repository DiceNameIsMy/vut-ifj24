//
// Created by nur on 19.10.24.
//

#include "structs/dynBuffer.h"
#include <stdlib.h>
#include <string.h>

const int INIT_DYN_BUFFER_SIZE = 512;

int initDynBuffer(DynBuffer *buffer, int size) {
	if (size <= 0)
		size = INIT_DYN_BUFFER_SIZE;

    buffer->data = calloc(size, sizeof(char));
    if (buffer->data == NULL)
        return -1;

    buffer->capacity = size;
    buffer->nextIdx = 0;
    return 0;
}

void freeDynBuffer(DynBuffer *buffer) {
	if (buffer == NULL)
    	return;
    if (buffer->data == NULL)
    	return;
	free(buffer->data);
	buffer->data = NULL;
	buffer->capacity = 0;
	buffer->nextIdx = 0;
}

int extendDynBuffer(DynBuffer *buffer) {
	const int newSize = buffer->capacity * 9 / 5 + 1; // increase capacity by 1.8 times
	char *newData = realloc(buffer->data, newSize);
	if (newData == NULL) {
		return -1;
	}
	memset(newData + buffer->nextIdx, 0, newSize - buffer->nextIdx);
	buffer->data = newData;
	buffer->capacity = newSize;
	return 0;
}

int appendDynBuffer(DynBuffer *buffer, const char c) {
	if (buffer->capacity == buffer->nextIdx + 1) { // +1 to keep \0 at the end, so that data can be used as string.
		if (extendDynBuffer(buffer) != 0)
			return -1;
	}

	buffer->data[buffer->nextIdx] = c;
	buffer->nextIdx++;
	return 0;
}

int copyFromDynBuffer(DynBuffer *buffer, char **dest) {
	char *cpy = strdup(buffer->data);
	*dest = cpy;
	return 0;
}

int emptyDynBuffer(DynBuffer *buffer) {
	memset(buffer->data, 0, buffer->capacity - buffer->nextIdx);
	buffer->nextIdx = 0;
	return 0;
}


