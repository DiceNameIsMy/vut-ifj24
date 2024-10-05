//
// Created by malbert on 10/5/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <parser/parser.h>

int main(int count, char **argv){
    printf("Hello, World!\n");

    lexer_t lexer = init_lexer(stdin);
    parser(&lexer);
    return 0;
}