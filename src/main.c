#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "structs/ast.h"
#include "structs/symtable.h"
#include "logging.h"

#define CHUNK_SIZE 1024

TokenArray tokenArray;
ASTNode** astNode;
SymTable *symTable;

char* readStdinAsString() {
    char* buffer = NULL;
    size_t bufferSize = 0;
    size_t totalSize = 0;

    char temp[CHUNK_SIZE];  // Temporary buffer to read in chunks
    while (fgets(temp, CHUNK_SIZE, stdin) != NULL) {
        size_t tempLen = strlen(temp);

        // Allocate (or reallocate) memory for the main buffer
        char* newBuffer = realloc(buffer, totalSize + tempLen + 1);
        if (newBuffer == NULL) {
            perror("Failed to allocate memory");
            free(buffer);
            return NULL;
        }

        buffer = newBuffer;

        // Copy the temporary buffer content into the main buffer
        strcpy(buffer + totalSize, temp);
        totalSize += tempLen;
    }

    return buffer;
}

int main(void) {
    char* source_code;
    streamToString(stdin, &source_code);
    
    SymTable_Init(symTable);
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    free(source_code); // Free the source code buffer

    printf("Hello, World!\n");
    parseInit(&tokenArray, symTable);
}


