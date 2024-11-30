// Created by Savin Ivan
// 10.10.2025
// Rebuilded on 25.11.2024

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "structs/dynBuffer.h"
#include "structs/bvs.h"
#include "structs/symtable.h"
#include "logging.h"

void parse_MULTILINE_STRING();

const char* sourceCode;
TokenArray *lexer_tokenArr;
DynBuffer buff;
int current_char_index = 0;

// Array of keywords
// No need for ?i32, ?f64, ?[]u8 as they are nullable types
const char *keywords[] = {
    "const", "var", "if", "else", "while", "fn", "pub",
    "null", "return", "void", "@import", "i32", "f64", "u8"
};

bool tryGetKeyword(const char *str, TokenType *keywordType) {
    if (strcmp(str, "const") == 0) {
        *keywordType = TOKEN_KEYWORD_CONST;
    } else if (strcmp(str, "var") == 0) {
        *keywordType = TOKEN_KEYWORD_VAR;
    } else if (strcmp(str, "if") == 0) {
        *keywordType = TOKEN_KEYWORD_IF;
    } else if (strcmp(str, "else") == 0) {
        *keywordType = TOKEN_KEYWORD_ELSE;
    } else if (strcmp(str, "while") == 0) {
        *keywordType = TOKEN_KEYWORD_WHILE;
    } else if (strcmp(str, "fn") == 0) {
        *keywordType = TOKEN_KEYWORD_FN;
    } else if (strcmp(str, "pub") == 0) {
        *keywordType = TOKEN_KEYWORD_PUB;
    } else if (strcmp(str, "null") == 0) {
        *keywordType = TOKEN_KEYWORD_NULL;
    } else if (strcmp(str, "return") == 0) {
        *keywordType = TOKEN_KEYWORD_RETURN;
    } else if (strcmp(str, "void") == 0) {
        *keywordType = TOKEN_KEYWORD_VOID;
    } else if (strcmp(str, "@import") == 0) {
        *keywordType = TOKEN_KEYWORD_IMPORT;
    } else if (strcmp(str, "i32") == 0) {
        *keywordType = TOKEN_KEYWORD_I32;
    } else if (strcmp(str, "f64") == 0) {
        *keywordType = TOKEN_KEYWORD_F64;
    } else if (strcmp(str, "u8") == 0) {
        *keywordType = TOKEN_KEYWORD_U8;
    } else {
        return false;
    }
    return true;
}

// Error handler
__attribute__((weak)) void endWithCode(int code) {
    ;
    // TODO: uncomment when time comes
    freeDynBuffer(&buff); // free the buffer (it was allocated during initialization) and was not freed yet
    exit(code);
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

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isSpecialSymbol(char c) {
    const bool specialSymbol = strchr("+-*=()[]{}|,;:><!", c) != NULL;
    return specialSymbol; // return true if c is in the list of special symbols
}

bool inIdTemplate(char c) {
    return '_' == c || isalnum(c); // numbers, letters and _ are allowed
}

void parse_ID_TEMPLATE(){
    while (sourceCode[current_char_index] != '\0') {
        const char c = sourceCode[current_char_index];
        current_char_index++;
        if (inIdTemplate(c)) {
            // continue reading the same ID
            appendDynBuffer(&buff, c);
        } else if (strcmp(buff.data, "ifj") == 0) { // ID ended and it is "ifj"
            // ifj state is a special case
            
            current_char_index--; // start searching dot right after "ifj"

            // Skip spaces until the first dot
            while (isspace(sourceCode[current_char_index])) {
                current_char_index++;
            }

            // Expecting a dot next
            if (sourceCode[current_char_index] == '.') {
                appendDynBuffer(&buff, sourceCode[current_char_index]);
                current_char_index++;
            } else {
                // ifj word and no dot after it
                // simple ID
                Token idToken = {.type = TOKEN_ID};
                initStringAttribute(&idToken.attribute, buff.data);
                addToken(lexer_tokenArr, idToken);
                emptyDynBuffer(&buff);
                current_char_index--; // return to the last character so it can be processed in the common state
                return;
            }

            // Skip spaces until the next word
            while (isspace(sourceCode[current_char_index])) {
                current_char_index++;
            }

            int check_i = current_char_index;
            // Read the next word into the buffer
            // if ifj . 12func is possible error
            if (isdigit(sourceCode[current_char_index])) {
                // if the next character is a digit, handle as an error or unexpected token
                Token errorToken = {.type = TOKEN_ERROR};
                initStringAttribute(&errorToken.attribute, "Error: Expected [function] after 'ifj.'");
                addToken(lexer_tokenArr, errorToken);
                emptyDynBuffer(&buff);
                endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
                break;
            }
            while (inIdTemplate(sourceCode[current_char_index])) {
                appendDynBuffer(&buff, sourceCode[current_char_index]);
                current_char_index++;
            }

            if (check_i == current_char_index) {
                // If the buffer is empty, handle as an error or unexpected token
                Token errorToken = {.type = TOKEN_ERROR};
                initStringAttribute(&errorToken.attribute, "Error: Expected [function] after 'ifj.'");
                endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
                break;
            }

            // Process the word as a token
            TokenAttribute attribute;
            // printf("IFJ: %s\n", buff.data);
            initStringAttribute(&attribute, buff.data);
            const Token token = createToken(TOKEN_ID, attribute);
            addToken(lexer_tokenArr, token);
            emptyDynBuffer(&buff);
            // added [ifj.'functionName'] as a token_id
            // return to the common state
            return;
        } else {
            // end of the identifier
            TokenType tokenType;

            // check if it is a keyword
            if (tryGetKeyword(buff.data, &tokenType)) {
                // if the identifier is a keyword
                // add it as a keyword token
                Token keywordToken = {.type = tokenType};
                initStringAttribute(&keywordToken.attribute, buff.data);
                addToken(lexer_tokenArr, keywordToken);
            } else {
                // if the identifier is not a keyword
                // add it as an identifier token
                Token idToken = {.type = TOKEN_ID};
                initStringAttribute(&idToken.attribute, buff.data);
                addToken(lexer_tokenArr, idToken);
            }
            emptyDynBuffer(&buff);
            current_char_index--; // return to the last character so it can be processed in the common state
            return;
        }
    }
    // end of the identifier and the source code
    TokenType tokenType;
    // check if it is a keyword
    if (tryGetKeyword(buff.data, &tokenType)) {
        // if the identifier is a keyword
        // add it as a keyword token
        Token keywordToken = {.type = tokenType};
        initStringAttribute(&keywordToken.attribute, buff.data);
        
        addToken(lexer_tokenArr, keywordToken);
    } else {
        // if the identifier is not a keyword
        // add it as an identifier token
        Token idToken = {.type = TOKEN_ID};
        initStringAttribute(&idToken.attribute, buff.data);
        addToken(lexer_tokenArr, idToken);
    }
    emptyDynBuffer(&buff);
    return;
}

void parse_ARRAY(){
    // []u8 case
    Token arrayToken;
    arrayToken.attribute.str = NULL;
    arrayToken.type = TOKEN_ERROR; // for the beginning making it a flag
    if (strlen(sourceCode) < current_char_index + 4) {
        ;
    } else if (strncmp(&sourceCode[current_char_index-1], "[]u8", 4) == 0) {
        arrayToken.type = TOKEN_KEYWORD_U8_ARRAY;
        current_char_index += 3; // Move the current index
    }

    // If the token was not set, generate an error
    if (arrayToken.type == TOKEN_ERROR) {
        arrayToken.type = TOKEN_ERROR;
        initStringAttribute(&arrayToken.attribute, "Expected array type");
        addToken(lexer_tokenArr, arrayToken);
        emptyDynBuffer(&buff);
        endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
    }

    addToken(lexer_tokenArr, arrayToken);
    emptyDynBuffer(&buff);
    // Return to the common state
    return;
}

void parse_NULLABLE() {
        Token nullableToken;
        nullableToken.attribute.str = NULL;
        nullableToken.type = TOKEN_ERROR; // Изначально делаем флагом
        
        // Возможные значения и их типы
        const char *nullableKeywords[] = {
            "?i32", "?f64", "?[]u8"
        };
        const int nullableTypes[] = {
            TOKEN_KEYWORD_I32_NULLABLE, TOKEN_KEYWORD_F64_NULLABLE, 
            TOKEN_KEYWORD_U8_ARRAY_NULLABLE
        };
        const int keywordsCount = sizeof(nullableKeywords) / sizeof(nullableKeywords[0]);

    // Проверяем каждый ключевой токен
    for (int i = 0; i < keywordsCount; i++) {
        size_t keywordLength = strlen(nullableKeywords[i]);
        if (strlen(sourceCode) < current_char_index + keywordLength) {
            continue; // Пропускаем, если не хватает символов
        }
        if (strncmp(&sourceCode[current_char_index - 1], nullableKeywords[i], keywordLength) == 0) {
            nullableToken.type = nullableTypes[i];
            current_char_index += keywordLength-1; // Смещаем текущий индекс
            break;
        }
    }

    // Если токен не был установлен, генерируем ошибку
    if (nullableToken.type == TOKEN_ERROR) {
        initStringAttribute(&nullableToken.attribute, "Expected nullable type");
        addToken(lexer_tokenArr, nullableToken);
        emptyDynBuffer(&buff);
        endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
    }

    addToken(lexer_tokenArr, nullableToken);
    emptyDynBuffer(&buff);
    // Возвращаемся к общему состоянию
    return;
}

void parse_SPECIAL_SYMBOL(){
    const char c = sourceCode[current_char_index];
    
    current_char_index++;
    Token specialSymbolToken;
    specialSymbolToken.attribute.str = NULL;
    specialSymbolToken.type = TOKEN_ERROR; // for the beginning making it a flag

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
            addToken(lexer_tokenArr, specialSymbolToken);
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
    /*
    // Dot can be parsed from common only as a part of float number
    case '.':
        specialSymbolToken.type = TOKEN_DOT;
        break;
    */
    case ':':
        specialSymbolToken.type = TOKEN_COLON;
        break;
    case ';':
        specialSymbolToken.type = TOKEN_SEMICOLON;
        break;
    }
    
    // if after that special symbol token is still NULL, it means that it is an error
    if (specialSymbolToken.type == TOKEN_ERROR){
        initStringAttribute(&specialSymbolToken.attribute, "Unrecognized special symbol");
        addToken(lexer_tokenArr, specialSymbolToken);
        emptyDynBuffer(&buff);
        endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
    }
    addToken(lexer_tokenArr, specialSymbolToken);
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

void parse_NUMBER() {
    Token numberToken;
    numberToken.attribute.str = NULL;

    // Check if the number starts with 0
    // If it does, it can't be followed by a digit
    if (buff.data[0] == '0' && sourceCode[current_char_index] != '.' && 
        sourceCode[current_char_index] != 'e' && sourceCode[current_char_index] != 'E' &&
        isdigit(sourceCode[current_char_index])) {
        Token errorToken = {.type = TOKEN_ERROR};
        initStringAttribute(&errorToken.attribute, "Number can't start with 0");
        addToken(lexer_tokenArr, errorToken);
        emptyDynBuffer(&buff);
        endWithCode(1); // ERROR - invalid number
    }

    int isFloat = 0; // flag for float number
    int hasExponent = 0; // flag for exponent

    while (sourceCode[current_char_index] != '\0') {
        const char c = sourceCode[current_char_index];

        if (c >= '0' && c <= '9') {
            // digit to the number
            appendDynBuffer(&buff, c);
        } else if (c == '.' && !isFloat) {
            // if there is a dot, it is a float number
            isFloat = 1;
            appendDynBuffer(&buff, c);
        } else if ((c == 'e' || c == 'E') && !hasExponent) {
            hasExponent = 1;
            appendDynBuffer(&buff, c);

            // next character must be a digit or + or -
            current_char_index++;
            char next = sourceCode[current_char_index];
            if (next == '+' || next == '-') {
                appendDynBuffer(&buff, next);
                next = sourceCode[++current_char_index];
            }
            // check for 0.2e (no digit after e)
            if (!isdigit(next)) {
                // a digit expected
                Token errorToken = {.type = TOKEN_ERROR};
                initStringAttribute(&errorToken.attribute, "Invalid exponent format");
                addToken(lexer_tokenArr, errorToken);
                emptyDynBuffer(&buff);
                endWithCode(1); // ERROR - invalid exponent
            } else{
                // digit after e/E
                appendDynBuffer(&buff, next);
            }
        } else {
            // End of the number
            break;
        }
        current_char_index++;
    }

    // Check if the number ends with a dot
    if (isFloat && buff.data[buff.nextIdx - 1] == '.') {
        Token errorToken = {.type = TOKEN_ERROR};
        initStringAttribute(&errorToken.attribute, "Invalid number format");
        addToken(lexer_tokenArr, errorToken);
        emptyDynBuffer(&buff);
        endWithCode(1); // ERROR - invalid number format
    }

    // Determine the type of the number
    if (isFloat || hasExponent) {
        double fValue = strtod(buff.data, NULL);
        numberToken.attribute.real = fValue;
        numberToken.type = TOKEN_F64_LITERAL;
    } else {
        int iValue = strtol(buff.data, NULL, 10);
        numberToken.attribute.integer = iValue;
        numberToken.type = TOKEN_I32_LITERAL;
    }
    

    addToken(lexer_tokenArr, numberToken);
    emptyDynBuffer(&buff);
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
                parse_MULTILINE_STRING();
                // return to the common state
                return;
            } else {
                // got some other character after \ so it is an error 
                // (because nothing can be done with something that starts with \ in common state)
                Token errorToken = {.type = TOKEN_ERROR};
                initStringAttribute(&errorToken.attribute, "After multiline strange character was found");
                addToken(lexer_tokenArr, errorToken);
                endWithCode(1); // ERROR - unrecognized token found TODO: clean up used memory
            }
        } else {
            // next line is not a part of multiline and must be processed as a common line
            Token stringToken = {.type = TOKEN_STRING_LITERAL};
            initStringAttribute(&stringToken.attribute, buff.data);
            addToken(lexer_tokenArr, stringToken);
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
    addToken(lexer_tokenArr, stringToken);
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
                    addToken(lexer_tokenArr, errorToken);
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
                addToken(lexer_tokenArr, errorToken);
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
            addToken(lexer_tokenArr, errorToken);
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
            Token stringToken = {.type = TOKEN_STRING_LITERAL};
            initStringAttribute(&stringToken.attribute, buff.data);
            addToken(lexer_tokenArr, stringToken);
            emptyDynBuffer(&buff);
            return;
        } else if (c == '\\') {
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
                        addToken(lexer_tokenArr, errorToken);
                        emptyDynBuffer(&buff);
                        endWithCode(1); // ERROR - unrecognized literal TODO: clean up used memory
                    } else {
                        // TODO: could it be \x00 and can it cause an error?
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
                    addToken(lexer_tokenArr, errorToken);
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
            addToken(lexer_tokenArr, errorToken);
            emptyDynBuffer(&buff);
            endWithCode(1); // ERROR - non-printable character TODO: clean up used memory
        }
    }
    // Error - string was not ended properly
    Token errorToken = {.type = TOKEN_ERROR};
    initStringAttribute(&errorToken.attribute, "End of file was reached while parsing a string");
    addToken(lexer_tokenArr, errorToken);
    emptyDynBuffer(&buff);
    endWithCode(1);
}

// The main function of the lexer ##
void runLexer(const char *source_code, TokenArray *tokenArray) {

    // Initialize the buffer with default size (-1 triggers default size)
    initDynBuffer(&buff, -1);
    // Initialize the token array
    lexer_tokenArr = tokenArray;

    // make local global variable
    sourceCode = source_code;
    current_char_index = 0;

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
                // parse '/' as a division
                current_char_index--;
                parse_SPECIAL_SYMBOL();
            }
        } else if (isalpha(c) || c == '_' || c == '@') {
            // Exception for @import, because it starts as a special symbol
            // but in fact it is a keyword
            appendDynBuffer(&buff, c);
            parse_ID_TEMPLATE();
        } else if (c == '"') { 
            parse_STRING();
        } else if (c == '['){ // []u8
            // appendDynBuffer(&buff, c);
            parse_ARRAY();
        } else if (c == '?'){ // ?u8 / ?[]u8
            // appendDynBuffer(&buff, c);
            parse_NULLABLE();
        } else if (c >= '0' && c <= '9'){ // TODO: check conditions for numbers
            appendDynBuffer(&buff, c);
            parse_NUMBER();
        } else if (c == '.'){ // In case of dot, it can be a part of a number
            appendDynBuffer(&buff, '0');
            current_char_index--;
            parse_NUMBER();
        } else if (isSpecialSymbol(c)){
            // No need to memorize letters (type tells content) 
            // appendDynBuffer(&buff, c);
            current_char_index--;
            parse_SPECIAL_SYMBOL();
        }  else {
            //there is no token for this character
            endWithCode(1);
        }
    }

    // Adding EOF token to the end of the token array
    addToken(tokenArray, createToken(TOKEN_EOF, (TokenAttribute) {}));
    // Free the buffer
    freeDynBuffer(&buff);
}
