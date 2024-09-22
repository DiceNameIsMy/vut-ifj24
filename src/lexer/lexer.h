//
// Created by nur on 22.9.24.
//

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

enum TokenType {
    Invalid,
    ConstString,
    ConstInt32,
    ConstFloat64,
    VarDeclaration,
    VarAssignment,
    EndStatement,
    EndProgram,
};

typedef struct {
} InvalidToken;

typedef struct {
    char *value;
} ConstStringToken;

typedef struct {
    int32_t value;
} ConstInt32Token;

typedef struct {
    int64_t value;
} ConstFloat64Token;

typedef struct {
} VarDeclarationToken;

typedef struct {
    char *value;
} VarNameToken;

typedef struct {
} VarAssignmentToken;

// ";" symbol
typedef struct {
} EndStatementToken;

// ";" symbol
typedef struct {
} EndProgramToken;

typedef struct {
    enum TokenType type;
    union {
        InvalidToken invalid;
        ConstStringToken const_string;
        ConstInt32Token const_i32;
        ConstFloat64Token const_f64;
        VarDeclarationToken var_declaration;
        VarAssignmentToken var_assignment;
        EndStatementToken end_statement;
        EndProgramToken end_program;
    } data;
} Token;

// A function that on each consecutive call outputs the next token
typedef Token (*token_generator)();

typedef struct {
    FILE* source;
} lexer_t;

lexer_t init_lexer(FILE *stream);

Token get_next_token(const lexer_t *lexer);

#endif //LEXER_H
