//
// Created by malbert on 10/5/24.
//
#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "structs/ast.h"
#include "../src/structs/bvs.c"
#include "logging.h"

#include "test_utils.h"
#include "structs/bvs.h"

#define CHUNK_SIZE 8000


TokenArray tokenArray;
ASTNode* astNode;
BVS *symTable;

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
int main(int count, char **argv){
    char* source_code = readStdinAsString();
    printf("Hello, World! %s\n", source_code);
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    astNode = parseInit(&tokenArray);

    free(source_code);
//    RUN_TESTS();
//
//    SUMMARIZE()
    return 0;
}