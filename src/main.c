#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "symtable.h"

#include "lexer.h"
#include "parser.h"
#include "target_gen.h"

#include "logging.h"
#include "target_gen.h"

#define CHUNK_SIZE 1024

TokenArray tokenArray;

SymTable Table;

ASTNode* astNode;

int main(void) {
    // Read the source code from stdin
    char* source_code;

    if (streamToString(stdin, &source_code) != 0) {
        return 99; // Exit with an allocation error
    }
    SymTable_Init(&Table);
    // Run the lexer
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    free(source_code); // Free the source code buffer

    astNode = parseInit(&tokenArray, &Table); // Parse the source code

    generateTargetCode(astNode, &Table, stdout); // Generate the target code

    freeTokenArray(&tokenArray); // Free the token array
    clearAstNode(astNode);
    SymTable_Dispose(&Table);

    return 0;
}


