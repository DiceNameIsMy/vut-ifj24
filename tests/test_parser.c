//
// Created by malbert on 10/5/24.
//
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "logging.h"

#include "test_utils.h"
#include "bvs.h"
#include "symtable.h"

#define CHUNK_SIZE 8000


TokenArray tokenArray;
ASTNode* astNode;
SymTable symTable;

int read_source_code(const char *filename, char **source_code) {
    if (source_code == NULL) {
        return -1;
    }

    FILE *source_code_stream = fopen(filename, "r");
    if (source_code_stream == NULL) {
        FAIL("Failed to open the source code file");
        return -1;
    }
    int r = streamToString(source_code_stream, source_code);
    fclose(source_code_stream);

    if (r != 0) {
        FAIL("Failed to read the source code from the file");
        return -1;
    }
    return 0;
}

TEST(parse_basic_program)
    char *source_code;
    if (read_source_code("tests/input/syntax/program.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    SymTable_Init(&symTable);
    astNode = parseInit(&tokenArray, &symTable);
    clearAstNode(astNode);
    free(source_code);
ENDTEST


TEST(parse_fac_iter_funcs_program)
    char *source_code;
    if (read_source_code("tests/input/syntax/factorial_iter.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    SymTable_Init(&symTable);
    astNode = parseInit(&tokenArray, &symTable);
    clearAstNode(astNode);
    free(source_code);
ENDTEST

TEST(parse_fac_rec_funcs_program)
    char *source_code;
    if (read_source_code("tests/input/syntax/factorial_rec.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    SymTable_Init(&symTable);
    astNode = parseInit(&tokenArray, &symTable);
    clearAstNode(astNode);
    free(source_code);
ENDTEST


TEST(parse_stdlib_funcs_program)
    char *source_code;
    if (read_source_code("tests/input/syntax/stdlib_funcs.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    runLexer(source_code, &tokenArray);
    SymTable_Init(&symTable);
    astNode = parseInit(&tokenArray, &symTable);
    clearAstNode(astNode);
    free(source_code);
ENDTEST







int main(int count, char **argv){


    RUN_TESTS();

    SUMMARIZE()
}
