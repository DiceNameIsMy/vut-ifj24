// Created by Savin Ivan
// 10.10.2025
// Rebuilded on 25.11.2024

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "structs/dynBuffer.h"
#include "structs/bvs.h"
#include "structs/symtable.h"
#include "logging.h"
char* sourceCode;
TokenArray *tokenArr;
DynBuffer buff;
int current_char_index = 0;

// Array of keywords
const char *keywords[] = {
    "const", "var", "if", "else", "while", "fn", "pub",
    "null", "return", "void", "@import",
    "i32", "?i32", "f64", "?f64", "u8", "[]u8", "?[]u8"
};

// Error handler
__attribute__((weak)) void endWithCode(int code) {
    ;
    // TODO: uncomment when time comes
    // exit(code);
}

int streamToString(FILE *stream, char **str) {
    if (str == NULL) {
        return -1;
    }

    DynBuffer buff;
    initDynBuffer(&buff, -1);

    char buffer[512];
    // TODO: Is fgets of 512 is correct when buffer size is 512?
    while (fgets(buffer, 512, stream)) {
        if (appendStringDynBuffer(&buff, buffer) == -1) {
            freeDynBuffer(&buff);
            return -1;
        }
    }

    if (copyFromDynBuffer(&buff, str) == -1) {
        freeDynBuffer(&buff);
        return -1;
    }

    freeDynBuffer(&buff);
    return 0;
}

// TODO: CHECK
bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
// TODO: CHECK
bool isSpecialSymbol(char c) {
    // TODO: do we need . / &
    const bool specialSymbol = strchr("+-*=()[]{}|,;:><!", c) != NULL;
    return specialSymbol; // return true if c is in the list of special symbols
}

bool inIdTemplate(char c) {
    return isalnum(c) || c == '_'; // TODO: can it use _ at the beginning?
}

void parse_SPECIAL_SYMBOL(){
    const char c = sourceCode[current_char_index];
    current_char_index++;
    Token specialSymbolToken;
    specialSymbolToken.attribute.str = NULL;
    specialSymbolToken.type = NULL; // for the beginning making it a flag

    if (c == '=') {
        if (sourceCode[current_char_index] == '=') {
            specialSymbolToken.type = TOKEN_EQUAL_TO;
            // 2 letters token, so increase index
            current_char_index++;
        } 
        else {
            specialSymbolToken.type = TOKEN_ASSIGNMENT;
        }
    }
    if (c == '<') {
        if (sourceCode[current_char_index] == '=') {
            specialSymbolToken.type = TOKEN_LESS_THAN_OR_EQUAL_TO;
            // 2 letters token, so increase index
            current_char_index++;
        } 
        else {
            specialSymbolToken.type = TOKEN_LESS_THAN;
        }
    }
    if (c == '>') {
        if (sourceCode[current_char_index] == '=') {
            specialSymbolToken.type = TOKEN_GREATER_THAN_OR_EQUAL_TO;
            // 2 letters token, so increase index
            current_char_index++;
        } 
        else {
            specialSymbolToken.type = TOKEN_GREATER_THAN;
        }
    }
    if (c == '!') {
        if (sourceCode[current_char_index] == '=') {
            specialSymbolToken.type = TOKEN_NOT_EQUAL_TO;
            // 2 letters token, so increase index
            current_char_index++;
        } 
        else {
            // ! can be used only as a part of !=
            specialSymbolToken.type = TOKEN_ERROR;
            initStringAttribute(&specialSymbolToken.attribute, "Expected = after !");
            addToken(tokenArr, specialSymbolToken);
            emptyDynBuffer(&buff);
            endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
        }
    }
    switch (c){
    // Operations
    case '+':
        specialSymbolToken.type = TOKEN_ADDITION;
        break;
    case '-':
        specialSymbolToken.type = TOKEN_SUBTRACTION;
        break;
    case '*':
        specialSymbolToken.type = TOKEN_MULTIPLICATION;
        break;
    case '/':
        // TODO: check solo symbol, all '/' cases and division as a token
        specialSymbolToken.type = TOKEN_DIVISION;
        break;
    // Brackets
    case '(':
        specialSymbolToken.type = TOKEN_LEFT_ROUND_BRACKET;
        break;
    case ')':
        specialSymbolToken.type = TOKEN_RIGHT_ROUND_BRACKET;
        break;
    case '[':
        specialSymbolToken.type = TOKEN_LEFT_SQUARE_BRACKET;
        break;
    case ']':
        specialSymbolToken.type = TOKEN_RIGHT_SQUARE_BRACKET;
        break;
    case '{':
        specialSymbolToken.type = TOKEN_LEFT_CURLY_BRACKET;
        break; 
    case '}':
        specialSymbolToken.type = TOKEN_RIGHT_CURLY_BRACKET;
        break;
    case '|':
        specialSymbolToken.type = TOKEN_VERTICAL_BAR;
        break;
    // Special symbols
    case ',': // TODO: I suppose in functions it works fine
        specialSymbolToken.type = TOKEN_COMMA;
        break;
    case '.': // TODO: Can dot be a solo symboL?
        specialSymbolToken.type = TOKEN_DOT;
        break;
    case ':':
        specialSymbolToken.type = TOKEN_COLON;
        break;
    case ';':
        specialSymbolToken.type = TOKEN_SEMICOLON;
        break;
    }
    
    // if after that special symbol token is still NULL, it means that it is an error
    if (specialSymbolToken.type == NULL){
        specialSymbolToken.type = TOKEN_ERROR;
        initStringAttribute(&specialSymbolToken.attribute, "Unrecognized special symbol");
        addToken(tokenArr, specialSymbolToken);
        emptyDynBuffer(&buff);
        endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
    }
    addToken(tokenArr, specialSymbolToken);
    emptyDynBuffer(&buff);
    // return to the common state
    return;
}

void parse_COMMENT() {
    while (sourceCode[current_char_index] != '\0') {
        const char c = sourceCode[current_char_index];
        current_char_index++;
        if (c == '\n'){
            return;
        }
    }
    // Even if comment state and end of file is reached, it is not an error
    // because it is a comment and it is not necessary to end it
    return;
}

void parse_MULTILINE_STRING_SKIP_WHITESPACE(){
    while (sourceCode[current_char_index] != '\0') {
        const char c = sourceCode[current_char_index];
        current_char_index++;
        const bool isNothing = c == ' ' || c == '\t';
        if (isNothing) {
            // if whitespace continue searching for next character
            continue;
        }else if (c == '\\') {
            // if next simbol is \ then it is a multiline string
            if (sourceCode[current_char_index] == '\\') {
                current_char_index++;
                // Multiline string continues -> a newline for that must be added
                appendDynBuffer(&buff, '\n');
                // continue with parsing multiline string
                parse_MULTILINE_STRIMG();
                // return to the common state
                return;
            } else {
                // got some other character after \ so it is an error 
                // (because nothing can be done with something that starts with \ in common state)
                Token errorToken = {.type = TOKEN_ERROR};
                initStringAttribute(&errorToken.attribute, "After multiline strange character was found");
                addToken(tokenArr, errorToken);
                endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
            }
        } else {
            // next line is not a part of multiline and must be processed as a common line
            Token stringToken = {.type = TOKEN_STRING_LITERAL};
            initStringAttribute(&stringToken.attribute, buff.data);
            addToken(tokenArr, stringToken);
            emptyDynBuffer(&buff);

            current_char_index--; // return to the last character so it can be processed in the common state
            // continue with parsing common line
            return;
        }
    }
    // New line is the end of the file
    // so it is not an error, string must be ended
    Token stringToken = {.type = TOKEN_STRING_LITERAL};
    initStringAttribute(&stringToken.attribute, buff.data);
    addToken(tokenArr, stringToken);
    emptyDynBuffer(&buff);
    return;
}

void parse_MULTILINE_STRING() {
    while (sourceCode[current_char_index] != '\0') {
        const char c = sourceCode[current_char_index];
        current_char_index++;
        if (c == '\n'){
            // Start waiting if next line as also a part of multiline string
            // if string continues - SKIP_WHITESPACE will continue in another parse_MULTILINE_STRING()
            // otherwise it will return to the common state
            parse_MULTILINE_STRING_SKIP_WHITESPACE();
            return;
        } else if (c == '\\') {
            // TODO: can replace nextChar with c 
            char nextChar = sourceCode[current_char_index];
            current_char_index++;
            switch (nextChar){
            case 'n':
                appendDynBuffer(&buff, '\n');
                break;
            case 'r':
                appendDynBuffer(&buff, '\r');
                break;
            case 't':  
                appendDynBuffer(&buff, '\t');
                break;
            case '\\':
                appendDynBuffer(&buff, '\\');
                break;
            case '"':
                appendDynBuffer(&buff, '"');
                break;
            case 'x':{
                // process hex number
                char firstHex = sourceCode[current_char_index++];
                if (firstHex == '\0')
                    endWithCode(1); // ERROR - didn't find the second hex number
                char secondHex = sourceCode[current_char_index++];
                if (!isxdigit(firstHex) || !isxdigit(secondHex)){
                    Token errorToken = {.type = TOKEN_ERROR};
                    initStringAttribute(&errorToken.attribute, "Got invalid hex number while parsing a multiline string");
                    addToken(tokenArr, errorToken);
                    emptyDynBuffer(&buff);
                    endWithCode(1); // ERROR - unrecognized literal TODO: clean up used memory
                } else {
                    // Convert hex to char
                    char hex[3] = {firstHex, secondHex, '\0'};
                    appendDynBuffer(&buff, (char)strtol(hex, NULL, 16));
                }
                break;
            }
            default:
                // Error if not a valid escape sequence
                Token errorToken = {.type = TOKEN_ERROR};
                initStringAttribute(&errorToken.attribute, "Got invalid escape sequence while parsing a multiline string");
                addToken(tokenArr, errorToken);
                emptyDynBuffer(&buff);
                endWithCode(1); // ERROR - unrecognized literal TODO: clean up used memory
            }
        } else if (c >= 32 && c <= 126){ 
            // printable characters
            appendDynBuffer(&buff, c);
        } else {
            // Put an error since double quote strings can't contain non-printable characters
            Token errorToken = {.type = TOKEN_ERROR};
            initStringAttribute(&errorToken.attribute, "Got non-printable character while parsing a multiline string");
            addToken(tokenArr, errorToken);
            emptyDynBuffer(&buff);
            endWithCode(1); // ERROR - non-printable character found TODO: clean up used memory
        }
    }
    // Error - multiline string was not ended properly
    // TODO: What if it is the last line?
    // TODO: perhaps, it's better to parse as a string and call no error
    endWithCode(1);
}

void parse_STRING() {
    while (sourceCode[current_char_index] != '\0') {
        const char c = sourceCode[current_char_index];
        current_char_index++;
        if (c == '"'){
            // end of the string
            // loginfo("String: \"%s\"\n", buff->data);
            Token stringToken = {.type = TOKEN_STRING_LITERAL};
            initStringAttribute(&stringToken.attribute, buff.data);
            addToken(tokenArr, stringToken);
            emptyDynBuffer(&buff);
            return;
        } else if (c == '\\') {
            char nextChar = sourceCode[current_char_index];
            switch (nextChar){
                case 'n':
                    appendDynBuffer(&buff, '\n');
                    break;
                case 'r':
                    appendDynBuffer(&buff, '\r');
                    break;
                case 't':  
                    appendDynBuffer(&buff, '\t');
                    break;
                case '\\':
                    appendDynBuffer(&buff, '\\');
                    break;
                case '"':
                    appendDynBuffer(&buff, '"');
                    break;
                case 'x':{
                    // process hex number
                    char firstHex = sourceCode[current_char_index++];
                    if (firstHex == '\0')
                        endWithCode(1); // ERROR - didn't find the second hex number
                    char secondHex = sourceCode[current_char_index++];
                    if (!isxdigit(firstHex) || !isxdigit(secondHex)){
                        Token errorToken = {.type = TOKEN_ERROR};
                        initStringAttribute(&errorToken.attribute, "Got invalid hex number while parsing a multiline string");
                        addToken(tokenArr, errorToken);
                        emptyDynBuffer(&buff);
                        endWithCode(1); // ERROR - unrecognized literal TODO: clean up used memory
                    } else {
                        // Convert hex to char
                        char hex[3] = {firstHex, secondHex, '\0'};
                        appendDynBuffer(&buff, (char)strtol(hex, NULL, 16));
                    }
                    break;
                }
                default:
                    // Error if not a valid escape sequence
                    Token errorToken = {.type = TOKEN_ERROR};
                    initStringAttribute(&errorToken.attribute, "Got invalid escape sequence while parsing a multiline string");
                    addToken(tokenArr, errorToken);
                    emptyDynBuffer(&buff);
                    endWithCode(1); // ERROR - unrecognized literal TODO: clean up used memory
            }
        } else if (c >= 32 && c <= 126){ 
            // printable characters
            appendDynBuffer(&buff, c);
        } else {
            // Put an error since double quote strings can't contain non-printable characters
            Token errorToken = {.type = TOKEN_ERROR};
            initStringAttribute(&errorToken.attribute, "Got non-printable character while parsing a double quote string");
            addToken(tokenArr, errorToken);
            emptyDynBuffer(&buff);
            endWithCode(1); // ERROR - non-printable character TODO: clean up used memory
        }
    }
    // Error - string was not ended properly
    Token errorToken = {.type = TOKEN_ERROR};
    initStringAttribute(&errorToken.attribute, "End of file was reached while parsing a string");
    addToken(tokenArr, errorToken);
    emptyDynBuffer(&buff);
    endWithCode(1);
}

// The main function of the lexer ##
void runLexer(const char *source_code, TokenArray *tokenArray) {

    // Initialize the buffer with default size (-1 triggers default size)
    initDynBuffer(&buff, -1);

    // make local global variable
    sourceCode = source_code;

    while (sourceCode[current_char_index] != '\0'){
        const char c = sourceCode[current_char_index];
        current_char_index++;   // increase index for next character reading
        if (isWhitespace(c)) {
            continue; // Wait for the next character
        } else if (c == '\\') {
            if (sourceCode[current_char_index] == '\\'){ 
                current_char_index++;
                parse_MULTILINE_STRING();
            }
            else {
                // there is no token for this case
                endWithCode(1);
            }
        } else if (c == '/') {
            if (sourceCode[current_char_index] == '/') {
                current_char_index++;
                parse_COMMENT();
            }
            else {
                // TODO: division token?
                endWithCode(1); 
            }
        } else if (inIdTemplate(c)){
            appendDynBuffer(&buff, c);
            parse_ID_TEMPLATE();
        } else if (c == '"') { 
            parse_STRING();
        } else if (c == '['){ // []u8
            appendDynBuffer(&buff, c);
            parse_ARRAY();
        } else if (c == '?'){ // ?u8 / ?[]u8
            appendDynBuffer(&buff, c);
            parse_NULLABLE();
        }else if (c >= '0' && c <= '9'){ // TODO: check conditions for numbers
            appendDynBuffer(&buff, c);
            parse_NUMBER();
        } else if (isSpecialSymbol(c)){
            // No need to memorize letters (type tells content) 
            // appendDynBuffer(&buff, c);
            parse_SPECIAL_SYMBOL();
        }  else { // TODO: Do we need check for other conditions?
            //there is no token for this character
            endWithCode(1);
        }
    }

    // Free the buffer
    freeDynBuffer(&buff);
    
    // Adding EOF token to the end of the token array
    addToken(tokenArray, createToken(TOKEN_EOF, (TokenAttribute) {}));
}
