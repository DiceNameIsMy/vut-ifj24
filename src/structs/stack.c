//
// Created by malbert on 10/5/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs/stack.h"

void initializeStack(Stack *s, int capacity) {
    s->top = -1;
    s->capacity = capacity;
    s->items = (void **)malloc(capacity * sizeof(void *)); // Allocate memory for pointers
    if (s->items == NULL) {
        printf("Memory allocation failed!\n");
        exit(1); // Exit if memory allocation fails
    }
}

// Check if the stack is full
bool isStackIsFull(Stack *s) {
    return s->top == s->capacity - 1;
}

// Check if the stack is empty
bool isStackEmpty(Stack *s) {
    return s->top == -1;
}

// Resize the stack by doubling its capacity
void resizeStack(Stack *s) {
    printf("Resizing stack from %d to %d capacity...\n", s->capacity, s->capacity * 2);
    s->capacity += 10;

    // Use a temporary pointer to avoid losing reference to original memory
    void **temp = (void **)realloc(s->items, s->capacity * sizeof(void *));
    if (temp == NULL) {
        printf("Memory reallocation failed!\n");
        // Handle failure here, e.g., clean up resources or exit
        exit(1);
    }
    s->items = temp;
}

// Push an element of any type onto the stack
void StackPush(Stack *s, void *data, size_t dataSize) {
    if (isStackIsFull(s)) {
        resizeStack(s);  // Resize the stack if it's full
    }
    // Allocate memory for the new data and copy it to the stack
    void *item = malloc(dataSize);
    if (item == NULL) {
        printf("Memory allocation failed for item.\n");
        exit(1);
    }
    memcpy(item, data, dataSize);  // Copy the data to the newly allocated memory
    s->items[++(s->top)] = item;   // Store the pointer in the stack
    //printf("Item pushed to stack\n");
}

// Pop an element from the stack and return a pointer to it
void *popStack(Stack *s) {
    if (isStackEmpty(s)) {
        printf("Stack is empty! Cannot popStack.\n");
        return NULL;
    } else {
        void *item = s->items[s->top--];  // Retrieve the top item and decrement the stack
        return item;                      // Return the pointer to the data
    }
}

// Peek at the top element of the stack without removing it
void *peekStack(Stack *s) {
    if (isStackEmpty(s)) {
        printf("Stack is empty!\n");
        return NULL;
    } else {
        return s->items[s->top];  // Return a pointer to the top element
    }
}

// Free all the dynamically allocated memory for the stack
void destroyStack(Stack *s) {
    while (!isStackEmpty(s)) {
        free(s->items[s->top--]);  // Free each element
    }
    free(s->items);  // Free the array of pointers
    s->items = NULL;
}
