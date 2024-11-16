//
// Created by malbert on 10/5/24.
//

#ifndef VUT_IFJ_STACK_H
#define VUT_IFJ_STACK_H

// stack.h

#include <stdbool.h>  // For boolean type
#include <stddef.h>

// Stack structure definition for generic data
typedef struct Stack {
    void **items;   // Array of void pointers to store any type of data
    int top;        // Index of the top element
    int capacity;   // Maximum number of elements the stack can hold
} Stack;

// Function declarations
void initializeStack(Stack *s, int capacity);
bool isStackIsFull(Stack *s);
bool isStackEmpty(Stack *s);
void StackPush(Stack *s, void *data, size_t dataSize);  // Push with data size
void resizeStack(Stack *s);
void *popStack(Stack *s);
void *peekStack(Stack *s);
void destroyStack(Stack *s);  // Function to free allocated memory



#endif //VUT_IFJ_STACK_H

