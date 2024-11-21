#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "structs/ast.h"
#include "structs/symtable.h"
#include "logging.h"

#define CHUNK_SIZE 1024

TokenArray tokenArray;
SymTable symTable;
ASTNode* astNode;

int endWithCode(int code) {
    exit(code);
}

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
    // Read the source code from stdin
    char* source_code = readStdinAsString();
    if (source_code == NULL) {
        return 1;
    }
    SymTable_Init(&symTable);
    // Run the lexer
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    free(source_code); // Free the source code buffer

    astNode = parseInit(&tokenArray, &symTable); // Parse the source code

    freeTokenArray(&tokenArray); // Free the token array

    printf("Hello, World!\n");
    return 0;
}


