//
// Created by malbert on 10/5/24.
//
#include "token.h"
#include "ast.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

Token token;
TokenArray *tokenArr;
SymTable *sym_Table;
unsigned int stat_index = 0;
bool falseStatement = false;
type_t type_to_return; //semantic return checks
type_t type_returned; //semantic return checks


type_t idType(Token token) {
        switch (token.type) {
            case TOKEN_KEYWORD_F64_NULLABLE:
                return F64_NULLABLE;
            case TOKEN_KEYWORD_I32_NULLABLE:
                return I32_NULLABLE;
            case TOKEN_KEYWORD_U8_ARRAY_NULLABLE:
                return U8_ARRAY_NULLABLE;
            case TOKEN_KEYWORD_F64:
                return F64;
            case TOKEN_KEYWORD_I32:
                return I32;
            case TOKEN_KEYWORD_U8_ARRAY:
                return U8_ARRAY;
            default:
                return NONE;
        }
}

void addFunctionsToSymTable(TokenArray *array, SymTable *table) {
    Symbol funWrite = {"ifj.write", FUNCTION, false, true, NONE, NULL};
    Symbol funReadi32 = {"ifj.readi32", FUNCTION, false, true, I32_NULLABLE, NULL};
    Symbol funReadf64 = {"ifj.readf64", FUNCTION, false, true, F64_NULLABLE, NULL};
    Symbol funReadstr = {"ifj.readstr", FUNCTION, false, true, U8_ARRAY_NULLABLE, NULL};
    Symbol funf2i = {"ifj.f2i", FUNCTION, false, true, I32, NULL};
    Symbol funi2f = {"ifj.i2f", FUNCTION, false, true, F64, NULL};
    Symbol funString = {"ifj.string", FUNCTION, false, true, U8_ARRAY, NULL};
    Symbol funLength = {"ifj.length", FUNCTION, false, true, I32, NULL};
    Symbol funConcat = {"ifj.concat", FUNCTION, false, true, U8_ARRAY, NULL};
    Symbol funSubstring = {"ifj.substring", FUNCTION, false, true, U8_ARRAY_NULLABLE, NULL};
    Symbol funStrcmp = {"ifj.strcmp", FUNCTION, false, true, I32, NULL};
    Symbol funOrd = {"ifj.ord", FUNCTION, false, true, I32, NULL};
    Symbol funChr = {"ifj.chr", FUNCTION, false, true, U8_ARRAY, NULL};
    Symbol varNull = {"_", UNDEFINED, true, true, NONE, NULL};
    SymTable_AddSymbol(table, &funWrite);
    SymTable_AddSymbol(table, &funReadi32);
    SymTable_AddSymbol(table, &funReadf64);
    SymTable_AddSymbol(table, &funReadstr);
    SymTable_AddSymbol(table, &funf2i);
    SymTable_AddSymbol(table, &funi2f);
    SymTable_AddSymbol(table, &funString);
    SymTable_AddSymbol(table, &funLength);
    SymTable_AddSymbol(table, &funConcat);
    SymTable_AddSymbol(table, &funSubstring);
    SymTable_AddSymbol(table, &funStrcmp);
    SymTable_AddSymbol(table, &funOrd);
    SymTable_AddSymbol(table, &funChr);
    SymTable_AddSymbol(table, &varNull);
    SymTable_PushFuncParam(table, "ifj.write", UNDEFINED, "format");
    SymTable_PushFuncParam(table, "ifj.f2i", F64, "float");
    SymTable_PushFuncParam(table, "ifj.i2f", I32, "int");
    SymTable_PushFuncParam(table, "ifj.string", STR_LITERAL, "literal");
    SymTable_PushFuncParam(table, "ifj.length", U8_ARRAY, "str");
    SymTable_PushFuncParam(table, "ifj.concat", U8_ARRAY, "str1");
    SymTable_PushFuncParam(table, "ifj.concat", U8_ARRAY, "str2");
    SymTable_PushFuncParam(table, "ifj.substring", U8_ARRAY, "str");
    SymTable_PushFuncParam(table, "ifj.substring", I32, "i");
    SymTable_PushFuncParam(table, "ifj.substring", I32, "j");
    SymTable_PushFuncParam(table, "ifj.strcmp", U8_ARRAY, "str1");
    SymTable_PushFuncParam(table, "ifj.strcmp", U8_ARRAY, "str2");
    SymTable_PushFuncParam(table, "ifj.ord", U8_ARRAY, "str");
    SymTable_PushFuncParam(table, "ifj.ord", I32, "index");
    SymTable_PushFuncParam(table, "ifj.chr", I32, "int");    


    
    int token_no = 0;
    for(token_no = 0; token_no < array->size; token_no++) {
        if(array->tokens[token_no].type == TOKEN_KEYWORD_FN && token_no != array->size - 1 && array->tokens[token_no+1].type == TOKEN_ID) {
            Symbol funName;
            funName.name = array->tokens[token_no + 1].attribute.str;
            funName.used = false;
            funName.mut = false;
            funName.type = FUNCTION;
            funName.paramList = NULL;
            if(SymTable_Search(table, funName.name) != NULL) {
                fprintf(stderr, "Error: redefinition of a function!\n");
                exit(5);
            }
            SymTable_AddSymbol(table, &funName);
            token_no+=2;
            if(token_no >= array->size || array->tokens[token_no].type != TOKEN_LEFT_ROUND_BRACKET) {
                exit(2);
            }
            token_no++;        
            while(array->tokens[token_no].type != TOKEN_RIGHT_ROUND_BRACKET) {
                if(token_no >= array->size || array->tokens[token_no].type != TOKEN_ID) {
                    exit(2);
                }
                char *paramName = array->tokens[token_no].attribute.str;

                token_no++;
                if(token_no >= array->size || array->tokens[token_no].type != TOKEN_COLON) {
                    exit(2);
                }
                token_no++;
                if(token_no >= array->size || idType(array->tokens[token_no]) == NONE) {
                    exit(2);
                }

                SymTable_PushFuncParam(table, funName.name, idType(array->tokens[token_no]), paramName);

                token_no++;
                if(token_no >= array->size || 
                   (array->tokens[token_no].type != TOKEN_COMMA && array->tokens[token_no].type != TOKEN_RIGHT_ROUND_BRACKET)) {
                    exit(2);
                }
                if(array->tokens[token_no].type == TOKEN_RIGHT_ROUND_BRACKET) {
                    break;
                }
                token_no++;
            }
            token_no++;
            if(token_no >= array->size) {
                exit(2);
            }
            
            SymTable_SetRetType(table, funName.name, idType(array->tokens[token_no]));
        }
    }
    return;
}

type_t typeConv(type_t type1, type_t type2) {
    if(type1 == type2) {
        return type1;
    }
    if(type1 == NONE || type2 == NONE) {
        return NONE;
    }
    if(type2 == NULL_LITERAL) {
        if(type1 == I32_NULLABLE || type1 == F64_NULLABLE || type1 == U8_ARRAY_NULLABLE || type1 == UNDEFINED) {
            return type1;
        }
        return NONE;
    }
    if(type1 == UNDEFINED) {
        return type2; //NULL_LITERAL OPTION INLUDED EARLIER
    }
    if(type1 == I32_NULLABLE && type2 == I32) {
        return I32_NULLABLE;
    }
    if(type1 == F64_NULLABLE && type2 == F64) {
        return F64_NULLABLE;
    }
    if(type1 == U8_ARRAY_NULLABLE && type2 == U8_ARRAY) {
        return U8_ARRAY_NULLABLE;
    }
    if(type1 == STR_LITERAL && type2 == U8_ARRAY) {
        return U8_ARRAY;
    }
    return NONE;
}

ASTNode* parseInit(TokenArray* array, SymTable *table) {
    stat_index = 0;
    tokenArr = array;
    sym_Table = table;
    SymTable_NewScope(table);
//    //Scroll over function names
    addFunctionsToSymTable(array, sym_Table);
    token = get_next_token();  // Initialize the first token
    return parseProgram();  // Parse the program and store the AST root
}

ASTNode* parseProgram() {
    // Create the root node for the program
    ASTNode* root = createASTNode(Program, NULL);
    // Parse the prolog first
    ASTNode* prologNode = parseProlog();
    // Parse the list of function definitions
    ASTNode* functionListNode = parseFunctionDefList();

    // Attach the prolog and function list as children of the program root
    root->left = prologNode;
    root->right = functionListNode;

    return root;  // Return the root of the AST
}

void match(TokenType expected) {
    if ((TokenType)token.type == expected) {
        token = get_next_token(); // Move to the next token
    } else {
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", expected, (TokenType)token.type);
        exit(2); // or handle error gracefully
    }
}

bool isMatch(TokenType expected) {
    if ((TokenType)token.type == expected) {
        return true;// Move to the next token
    } else {
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", expected, (TokenType)token.type);
        exit(2); // or handle error gracefully
    }
}

ASTNode* parseProlog() {
    ASTNode* prologNode = createASTNode(Prolog, NULL);
    match(TOKEN_KEYWORD_CONST);  // Match 'const'
    match(TOKEN_ID);             // Match 'ifj'
    match(TOKEN_ASSIGNMENT);     // Match '='
    match(TOKEN_KEYWORD_IMPORT);             // Match '@import'
    match(TOKEN_LEFT_ROUND_BRACKET);
    match(TOKEN_STRING_LITERAL);
    match(TOKEN_RIGHT_ROUND_BRACKET);
    match(TOKEN_SEMICOLON);
    return prologNode;  // Return the prolog node
}


ASTNode* parseFunctionDefList() {
    ASTNode* head = NULL;  // Head of the function definition list
    ASTNode* current = NULL;  // Current node in the list
    isMatch(TOKEN_KEYWORD_PUB);
    while (token.type == TOKEN_KEYWORD_PUB) {  // While there are functions to parse
        ASTNode* funcDefNode = parseFunctionDef();  // Parse each function definition

        if (head == NULL) {
            head = funcDefNode;  // First function becomes the head
        } else if (current != NULL){
            current->binding = funcDefNode;  // Link the new function to the previous one
        }

        current = funcDefNode;  // Move the current pointer to the newly added function
    }
    match(TOKEN_EOF);
    return head;  // Return the head of the function definition list
}


ASTNode* parseFunctionDef() {
    match(TOKEN_KEYWORD_PUB);      // Matches 'pub'
    match(TOKEN_KEYWORD_FN);       // Matches 'fn'

    SymTable_NewScope(sym_Table); //each function is a scope
    // Capture the function name
    char* functionName;
    if(isMatch(TOKEN_ID)) {
        functionName = strdup(token.attribute.str);
        if_malloc_error(functionName);
    } else{
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", TOKEN_ID, (TokenType)token.type);
        exit(2); // or handle error gracefully
    }
    match(TOKEN_ID);

    type_to_return = SymTable_GetRetType(sym_Table, functionName);
    //fprintf(stderr, "For function %s we need ret.type %d", functionName, (int)type_to_return);
    type_returned = UNDEFINED;

    // Create an AST node for the function definition
    ASTNode* funcNode = createASTNode(FunctionDef, functionName);

    // Parse parameters and attach to the function node
    match(TOKEN_LEFT_ROUND_BRACKET);
    funcNode->left = parseParamList();  // Attach parameter list as the left child
    match(TOKEN_RIGHT_ROUND_BRACKET);

    // Parse return type
    funcNode->right = parseReturnType();  // Attach return type as the right child

    // Parse function body and add it as the next node
    match(TOKEN_LEFT_CURLY_BRACKET);
    funcNode->next = parseStatementList();  // Attach the function body statements
    match(TOKEN_RIGHT_CURLY_BRACKET);
    if(type_to_return != NONE && type_returned == UNDEFINED) {
        fprintf(stderr, "Error: no return statement was provided in function %s\n", functionName);
        exit(6);
    }

    if(SymTable_UpperScope(sym_Table) != 0) { //quit the scope
        exit(9); //unused variables
    }

    return funcNode;  // Return the completed function definition node
}


ASTNode* parseParamList() {
    ASTNode* head = NULL;

    if (token.type == TOKEN_ID) {
        // Parse the first parameter
        char* paramName = strdup(token.attribute.str);
        if_malloc_error(paramName);
        Symbol symbol;
        
        symbol.name = token.attribute.str; //symbol info
        
        match(TOKEN_ID);
        match(TOKEN_COLON);
        
        symbol.type = idType(token); //more symbol info
        symbol.mut = true;
        symbol.used = false;
        symbol.retType = NONE;
        symbol.paramList = NULL;
        SymTable_AddSymbol(sym_Table, &symbol);
        
        ASTNode* paramType = parseType();

        head = createASTNode(Parameter, paramName);
        head->left = paramType;

        // Parse additional parameters
        head->next = parseParamListTail();
    }

    return head;
}

ASTNode* parseParamListTail() {
    if (token.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);

        // Parse the next parameter
        char* paramName;
        if(isMatch(TOKEN_ID)) {
            paramName = strdup(token.attribute.str);
            if_malloc_error(paramName);
        }
        else{
            // Handle syntax error
            fprintf(stderr ,"Syntax error: expected %d, but got %d\n", TOKEN_ID, (TokenType)token.type);
            exit(2); // or handle error gracefully
        }
        Symbol symbol;
        symbol.name = token.attribute.str;
        match(TOKEN_ID);
        match(TOKEN_COLON);
        symbol.type = idType(token);
        symbol.mut = true;
        symbol.used = false;
        symbol.retType = NONE;
        symbol.paramList = NULL;
        if(SymTable_Search(sym_Table, symbol.name) != NULL) {
            fprintf(stderr, "Error: resefinition of a variable!\n");
            exit(5);
        }
        SymTable_AddSymbol(sym_Table, &symbol);
        
        ASTNode* paramType = parseType();

        ASTNode* paramNode = createASTNode(Parameter, paramName);
        paramNode->left = paramType;

        // Continue parsing additional parameters
        paramNode->next = parseParamListTail();
        return paramNode;
    }

    return NULL;  // No more parameters
}


ASTNode* parseReturnType() {
    ASTNode* returnTypeNode = NULL;

    if (token.type == TOKEN_KEYWORD_VOID) {
        returnTypeNode = createASTNode(ReturnType, "void");
        match(TOKEN_KEYWORD_VOID);
    } else {
        returnTypeNode = parseType();  // Use parseType() for non-void types
    }

    return returnTypeNode;
}


ASTNode* parseType() {
    ASTNode* typeNode = NULL;

    if (token.type == TOKEN_KEYWORD_I32) {
        typeNode = createASTNode(DataType, "i32");
        typeNode->valType = idType(token);  //OPTIMIZE!!!!
        match(TOKEN_KEYWORD_I32);
    } else if (token.type == TOKEN_KEYWORD_F64) {
        typeNode = createASTNode(DataType, "f64");
        typeNode->valType = idType(token);
        match(TOKEN_KEYWORD_F64);
    } else if (token.type == TOKEN_KEYWORD_U8_ARRAY) {
        typeNode = createASTNode(DataType, "[]u8");
        typeNode->valType = idType(token);
        match(TOKEN_KEYWORD_U8_ARRAY);
    } else if (token.type == TOKEN_KEYWORD_I32_NULLABLE) {
        typeNode = createASTNode(DataType, "?i32");
        typeNode->valType = idType(token);
        match(TOKEN_KEYWORD_I32_NULLABLE);
    } else if (token.type == TOKEN_KEYWORD_F64_NULLABLE) {
        typeNode = createASTNode(DataType, "?f64");
        typeNode->valType = idType(token);
        match(TOKEN_KEYWORD_F64_NULLABLE);
    } else if (token.type == TOKEN_KEYWORD_U8_ARRAY_NULLABLE) {
        typeNode = createASTNode(DataType, "?[]u8");
        typeNode->valType = idType(token);
        match(TOKEN_KEYWORD_U8_ARRAY_NULLABLE);
    } else {
        typeNode = createASTNode(DataType, "Invalid");
        typeNode->valType = idType(token);
        match(TOKEN_ID);
    }

    return typeNode;  // Return the type node for the AST
}

ASTNode* parseStatementList() {
    ASTNode* head = NULL;      // Head of the statement list
    ASTNode* current = NULL;   // Current node in the list
    ASTNode* afterElse = NULL;
    while (token.type != TOKEN_RIGHT_CURLY_BRACKET) {  // Stop when '}' is encountered
        ASTNode* stmtNode = parseStatement();  // Parse each statement and get its AST node
        // if (stmtNode->next != NULL) {
        //     printf("Statement node has .next field not null. .next must not be used for parsing since it is used for linking statements in a sequence");
        //     exit(99);
        // }

        if (head == NULL) {
            head = stmtNode;  // The first statement becomes the head
            if (falseStatement){
                afterElse = stmtNode;
                while (afterElse->next != NULL){
                    afterElse = afterElse->next;
                }
            }
        }
        else if (falseStatement && afterElse){
            afterElse->next = stmtNode;
            falseStatement = false;
        }
        else if (current != NULL){
            current->next = stmtNode;  // Link the new statement to the previous one
            if (falseStatement){
                afterElse = stmtNode;
                while (afterElse->next != NULL){
                    afterElse = afterElse->next;
                }
            }

        }

        current = stmtNode;  // Move the current pointer to the newly added statement
    }

    return head;  // Return the head of the statement list
}



ASTNode* parseStatement() {
    ASTNode* stmtNode = NULL;

    switch (token.type) {
        case TOKEN_KEYWORD_CONST:
            stmtNode = parseConstDeclaration();  // Parse const declaration
            break;
        case TOKEN_KEYWORD_VAR:
            stmtNode = parseVarDeclaration();  // Parse variable declaration
            break;
        case TOKEN_ID:
            stmtNode = parseAssignmentOrFunctionCall();  // Parse assignment or function call
            break;
        case TOKEN_KEYWORD_IF:
            stmtNode = parseIfCondition();
            break;
        case TOKEN_KEYWORD_WHILE:
            stmtNode = parseWhileCondition();  // Parse while statement
            break;
        case TOKEN_KEYWORD_RETURN:
            stmtNode = parseReturnStatement();  // Parse return statement
            break;
        case TOKEN_LEFT_CURLY_BRACKET:  // New case for block statements
            stmtNode = parseBlockStatement();
            break;
        default:
            // Handle syntax error for unexpected token
            fprintf(stderr, "Syntax error: unexpected token in statement\n");
            exit(2);
    }

    return stmtNode;
}

ASTNode* parseBlockStatement() {
    match(TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    SymTable_NewScope(sym_Table); //dive

    // Parse the list of statements inside the block
    ASTNode* stmtListNode = parseStatementList();

    match(TOKEN_RIGHT_CURLY_BRACKET);  // Match '}'
    
    if(SymTable_UpperScope(sym_Table) != 0) {  //un-dive
        exit(9); //unused variables
    }

    // Create a BlockStatement node
    ASTNode* blockNode = createASTNode(BlockStatement, NULL);
    blockNode->left = stmtListNode;  // Attach the statement list as the left child

    return blockNode;
}


ASTNode* parseConstDeclaration() {
    match(TOKEN_KEYWORD_CONST);  // Match 'const' keyword
    Symbol symbol;
    
    char *constName;
    // Capture the constant name
    if (isMatch(TOKEN_ID)){
        constName = strdup(token.attribute.str);
        if_malloc_error(constName);
        symbol.name = token.attribute.str; //TODO: INVENT A WAY TO MARK AS A CONSTANT
        symbol.mut = false;
        symbol.used = false;
        symbol.type = UNDEFINED;
        symbol.retType = NONE;
        symbol.paramList = NULL;
    }
    else{
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", TOKEN_ID, (TokenType)token.type);
        exit(2); // or handle error gracefully
    }

    match(TOKEN_ID);  // Match the identifier (constant name)

    // Use parseVarType to handle optional type annotation
    ASTNode* typeNode = parseVarType();  // Returns the type node or NULL if no type
    if (typeNode != NULL) {
        symbol.type = typeNode->valType; //the type we expect from the exptession on the right
    }


    match(TOKEN_ASSIGNMENT);  // Match '='
    ASTNode* exprNode = parseExpression();  // Parse the constant's assigned value
    symbol.type = typeConv(symbol.type, exprNode->valType);
    if(symbol.type == NONE) {
        fprintf(stderr, "Error: Cannot assign to a variable of an uncompatible type : %s\n", symbol.name);
        exit(7);//I'll lookup the right code later or even write a special routine for this
    }
    if(symbol.type == UNDEFINED) {
        fprintf(stderr, "Error: Cannot determine the type of the declared variable :%s\n", symbol.name);
        exit(8);
    }
    if(SymTable_Search(sym_Table, symbol.name) != NULL) {
        fprintf(stderr, "Error: redefinition of a const\n");
        exit(5); //exit with error (redefinition!!!!)
    }
    SymTable_AddSymbol(sym_Table, &symbol);
    // Create the AST node for the const declaration
    ASTNode* constNode = createASTNode(ConstDeclaration, constName);
    constNode->left = typeNode;    // Attach type as left child (if available)
    constNode->right = exprNode;   // Attach the expression as right child
    //constNode->value = SymTable_Search(sym_Table, symbol.name); 
    match(TOKEN_SEMICOLON);  // Match ';'

    return constNode;
}



ASTNode* parseVarType() {
    ASTNode* typeNode = NULL;

    if (token.type == TOKEN_COLON) {  // Check if there's a type annotation
        match(TOKEN_COLON);
        typeNode = parseType();  // Parse and create the type node
    }

    return typeNode;  // Return the type node or NULL if no type specified
}


ASTNode* parseExpression() {
    ASTNode* left = parseSimpleExpression();  // Parse the initial simple expression

    // Use parseRelationalTail to handle additional relational operators
    ASTNode* result = parseRelationalTail(left);
    return result;  // Start the tail parsing with the initial left expression
}

ASTNode* parseRelationalTail(ASTNode* left) {
    // Check if the current token is a relational operator
    if (token.type == TOKEN_LESS_THAN || token.type == TOKEN_LESS_THAN_OR_EQUAL_TO ||
        token.type == TOKEN_GREATER_THAN || token.type == TOKEN_GREATER_THAN_OR_EQUAL_TO ||
        token.type == TOKEN_EQUAL_TO || token.type == TOKEN_NOT_EQUAL_TO) {
        NodeType operator;
        switch (token.type) {
            case TOKEN_LESS_THAN:
                operator = LessOperation;
                break;
            case TOKEN_LESS_THAN_OR_EQUAL_TO:
                operator = LessEqOperation;
                break;
            case TOKEN_GREATER_THAN:
                operator = GreaterOperation;
                break;
            case TOKEN_GREATER_THAN_OR_EQUAL_TO:
                operator = GreaterEqOperation;
                break;
            case TOKEN_EQUAL_TO:
                operator = EqualOperation;
                break;
            default:
                operator = NotEqualOperation;
                break;
        }
        
        // Capture the operator and move to the
        match(token.type);

        // Parse the right operand
        ASTNode* right = parseSimpleExpression();

        bool canCmp = (typeConv(left->valType, right->valType) != NONE ||
                      typeConv(right->valType, left->valType) != NONE ||
                        right->nodeType == IntLiteral && (left->valType == F64 || left->valType == F64_NULLABLE) ||
                        left->nodeType == IntLiteral && (right->valType == F64 || right->valType == F64_NULLABLE));
        if(!canCmp) { //TODO: cmp logic
            fprintf(stderr, "Error: Cannot compare uncompatible types\n");
            exit(7);
        }
        // Create an AST node for the relational operation
        ASTNode* opNode = createBinaryASTNode(operator, left, right);
        opNode->valType = BOOL;
        // Recursively call parseRelationalTail with the new opNode as the left operand
        return parseRelationalTail(opNode);
    }

    return left;  // Return the completed expression node
}


ASTNode* parseSimpleExpression() {
    ASTNode* left = parseTerm();  // Parse the first term (left operand)

    // Handle addition and subtraction operators
    while (token.type == TOKEN_ADDITION || token.type == TOKEN_SUBTRACTION) {
        NodeType operator = token.type == TOKEN_ADDITION ? AddOperation : SubOperation;// Capture the operator
        match(token.type);  // Consume the operator

        // Parse the next term (right operand)
        ASTNode* right = parseTerm();

        if((right->valType != I32 || left->valType != I32) && (right->valType != F64 || left->valType != F64)) {
            fprintf(stderr, "Error: cannot add/subtract uncompatible types\n");
            exit(7);
        }
        // Create a binary operation node
        left = createBinaryASTNode(operator, left, right);  // Update left with new binary node
        left->valType = typeConv(right->valType, left->left->valType);
    }

    return left;  // Return the completed simple expression node
}


ASTNode* parseTerm() {
    ASTNode* left = parseFactor();  // Parse the initial factor (left operand)

    // Handle multiplication and division
    while (token.type == TOKEN_MULTIPLICATION || token.type == TOKEN_DIVISION) {
        NodeType operator = token.type == TOKEN_MULTIPLICATION ? MulOperation : DivOperation;// Capture the operator
        match(token.type);  // Consume the operator

        // Parse the next factor (right operand)
        ASTNode* right = parseFactor();
        if((right->valType != I32 || left->valType != I32) && (left->valType != F64 || right->valType != F64)) {
            fprintf(stderr, "Error: cannot multiply/divide uncompatible types\n");
            exit(7);
        }
        // Create a binary operation node
        left = createBinaryASTNode(operator, left, right);  // Update left with new binary node
        left->valType = typeConv(right->valType, left->left->valType);
    }

    return left;  // Return the completed term node
}


ASTNode* parseFactor() {
    ASTNode* factorNode = NULL;

    if (token.type == TOKEN_LEFT_ROUND_BRACKET) {  // For expressions in parentheses
        match(TOKEN_LEFT_ROUND_BRACKET);
        factorNode = parseExpression();  // Parse the sub-expression inside parentheses
        match(TOKEN_RIGHT_ROUND_BRACKET);
    } else if (token.type == TOKEN_ID) {  // For identifiers (variables or function calls)
        char* identifier = strdup(token.attribute.str);
        if_malloc_error(identifier);
        match(TOKEN_ID);
        if (token.type == TOKEN_DOT) {
            // If the next token is a dot, parse it as a qualified function call
            match(TOKEN_DOT);

            char* functionName;
            if(isMatch(TOKEN_ID)) {
                functionName = (char *)calloc(100, sizeof(char)); //MAX_FUNCTIONNAME_LENGTH=100
                strcat(functionName, identifier);
                strcat(functionName, ".");
                strcat(functionName, token.attribute.str); //build a function name
                if_malloc_error(functionName);
            }
            else{
                // Handle syntax error
                fprintf(stderr ,"Syntax error: expected %d, but got %d\n", TOKEN_ID, (TokenType)token.type);
                exit(2); // or handle error gracefully
            }

            match(TOKEN_ID);


            if(SymTable_Search(sym_Table, functionName) == NULL || SymTable_GetType(sym_Table, functionName) != FUNCTION) {
                fprintf(stderr, "Error: Function %s not declared\n", functionName);
                exit(3); //NOT DECLARED OR NOT A FUNCTION
            }
            SymTable_SetUsed(sym_Table, functionName, true);
            
            // Parse function call parameters
            ASTNode* params = NULL;
            if (token.type == TOKEN_LEFT_ROUND_BRACKET) {
                params = parseFunctionCall(functionName);
            }

            // Create a node for the qualified function call
            ASTNode* funcCallNode = createASTNode(BuiltInFunctionCall, functionName);
            funcCallNode->valType = SymTable_GetRetType(sym_Table, functionName);
            funcCallNode->left = params;  // Attach parameters as left child
//            // Attach the main identifier (e.g., 'ifj') as an additional node
//            ASTNode* mainNode = createASTNode(Identifier, identifier);
//            mainNode->left = funcCallNode;

            return funcCallNode;
        }
        else if (token.type == TOKEN_LEFT_ROUND_BRACKET) {  // If it’s a function call
            
            if(SymTable_Search(sym_Table, identifier) == NULL || SymTable_GetType(sym_Table, identifier) != FUNCTION) {
                fprintf(stderr, "Error: Function %s not declared\n", identifier);
                exit(3); //NOT DECLARED OR NOT A FUNCTION 
            }
            SymTable_SetUsed(sym_Table, identifier, true);
            
            factorNode = createASTNode(FuncCall, identifier);
            factorNode->valType = SymTable_GetRetType(sym_Table, identifier);
            factorNode->left = parseFunctionCall(identifier);  // Attach arguments
        } else {
            
            if(SymTable_Search(sym_Table, identifier) == NULL) {
                fprintf(stderr, "Error: variable %s not declared\n", identifier);
                exit(3); //NOT DECLARED
            }
            SymTable_SetUsed(sym_Table, identifier, true);

            factorNode = createASTNode(Identifier, identifier);  // Variable reference
            factorNode->valType = SymTable_GetType(sym_Table, identifier);
        }
    } else if (token.type == TOKEN_I32_LITERAL || token.type == TOKEN_F64_LITERAL ||
               token.type == TOKEN_STRING_LITERAL || token.type == TOKEN_KEYWORD_NULL) {
        char* literalValue;
        switch (token.type) {
            case TOKEN_I32_LITERAL:
                factorNode = createASTNodeInteger(IntLiteral, token.attribute.integer);  // Literal node
                factorNode->valType = I32;
                break;
            case TOKEN_F64_LITERAL:
                factorNode = createASTNodeReal(FloatLiteral, token.attribute.real);  // Literal node
                factorNode->valType = F64;
                break;
            case TOKEN_STRING_LITERAL:
                factorNode = createASTNode(StringLiteral, token.attribute.str);  // Literal node
                factorNode->valType = STR_LITERAL;
                break;
            default:
                literalValue = strdup("NULL");
                factorNode = createASTNode(NullLiteral, literalValue);  // Literal node
                factorNode->valType = NULL_LITERAL;
                if_malloc_error(literalValue);
        }
        // For literals and `null`
        match(token.type);  // Consume the literal or `null`
    } else {
        // Handle syntax error if no valid factor is found
        fprintf(stderr, "Syntax error: unexpected token in factor\n");
        exit(2);
    }

    return factorNode;
}


ASTNode* parseFunctionCall(char *funcName) {
    match(TOKEN_LEFT_ROUND_BRACKET);  // Match '('

    // Create the root node for the function call arguments
    ASTNode* argsHead = NULL;
    ASTNode* currentArg = NULL;
    //CHECK IF A FUNCTION?
    Param *param = SymTable_GetParamList(sym_Table, funcName);

    // Parse arguments if any
    if (token.type != TOKEN_RIGHT_ROUND_BRACKET) {
        argsHead = parseExpression();  // Parse the first argument expression
        currentArg = argsHead;

        
        if(param == NULL || typeConv(param->paramType, currentArg->valType) == NONE) { //incorrect count or type
            fprintf(stderr, "Error: Invalid count/type of arguments in function call %s\n", funcName);
            exit(4);
        }

        // Parse additional arguments, if any, separated by commas
        while (token.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);  // Match ','

            // Parse the next argument expression
            ASTNode* nextArg = parseExpression();
            currentArg->next = nextArg;  // Link arguments
            currentArg = nextArg;

            param = param->next;
            if(param == NULL || typeConv(param->paramType, currentArg->valType) == NONE) {
                fprintf(stderr, "Error: Invalid count/type of arguments in function call %s\n", funcName);
                exit(4);
            }
        }
    }
    if(param != NULL) {
        param = param->next;
    }
    if(param != NULL) { //too few arguments
        fprintf(stderr, "Error: Too few arguments provided fpr function %s\n", funcName);
        exit(4); 
    }
    
    match(TOKEN_RIGHT_ROUND_BRACKET);  // Match ')'

    return argsHead;  // Return the head of the argument list
}


ASTNode* parseVarDeclaration() {
    match(TOKEN_KEYWORD_VAR);  // Match 'var' keyword
    char* varName;
    Symbol symbol;
    // Capture variable name
    if (isMatch(TOKEN_ID)){
        varName = strdup(token.attribute.str);
        if_malloc_error(varName);
        symbol.name = token.attribute.str;
        symbol.type = UNDEFINED;
        symbol.mut = true;
        symbol.used = false;
        symbol.retType = NONE;
        symbol.paramList = NULL;
    } else{
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", TOKEN_ID, (TokenType)token.type);
        exit(2); // or handle error gracefully
    }
    match(TOKEN_ID);  // Match the identifier (variable name)

    // Use parseVarType to handle optional type annotation
    ASTNode* typeNode = parseVarType();  // Returns the type node or NULL if no type
    if (typeNode != NULL) {
        symbol.type = typeNode->valType;
    }

    match(TOKEN_ASSIGNMENT);  // Match '='
    ASTNode* exprNode = parseExpression();  // Parse the assigned expression

    symbol.type = typeConv(symbol.type, exprNode->valType);
    if(symbol.type == NONE) {
        fprintf(stderr, "Error: Cannot assign a value to a variable of incompatible type : %s\n", symbol.name);
        exit(7);
    }
    if(symbol.type == UNDEFINED) {
        fprintf(stderr, "Error: Cannot determine the type of the declared variable :%s\n", symbol.name);
        exit(8);
    }
    if(SymTable_Search(sym_Table, symbol.name) != NULL) {
        fprintf(stderr, "Error: Redefinition of a variable: %s", symbol.name);
        exit(5); //REDEFINITION!
    }
    SymTable_AddSymbol(sym_Table, &symbol);
    // Create AST node for variable declaration
    ASTNode* varNode = createASTNode(VarDeclaration, varName);
    varNode->left = typeNode;   // Attach type as left child (if available)
    varNode->right = exprNode;  // Attach expression as right child

    match(TOKEN_SEMICOLON);  // Match ';'

    return varNode;
}

ASTNode* parseAssignmentOrFunctionCall() {
    // Capture the identifier (variable or function name)
    char* identifier;
    if(isMatch(TOKEN_ID)) {
        identifier = strdup(token.attribute.str);
        if_malloc_error(identifier);
    }
    match(TOKEN_ID);  // Match the identifier
    if (token.type == TOKEN_DOT) {
        // If the next token is a dot, parse it as a qualified function call
        match(TOKEN_DOT);

        // Parse the function name after the dot
        char *functionName;
        if(isMatch(TOKEN_ID)) {
            functionName = (char *)calloc(100, sizeof(char)); //MAX_FUNCTIONNAME_LENGTH=100
            strcat(functionName, identifier);
            strcat(functionName, ".");
            strcat(functionName, token.attribute.str); //build a function name
            if_malloc_error(functionName);
        } else {
            exit(2);
        }
        
        if(SymTable_Search(sym_Table, functionName) == NULL) {
            fprintf(stderr, "Error: Function %s not defined\n", functionName);
            exit(3); //DOESN'T EXIST
        }
        SymTable_SetUsed(sym_Table, functionName, true);
        
        match(TOKEN_ID);

        // Parse function call parameters
        ASTNode* params = NULL;
        if (token.type == TOKEN_LEFT_ROUND_BRACKET) {
            params = parseFunctionCall(functionName);
        } else {
            exit(2);
        }

        // Create a node for the qualified function call
        ASTNode* funcCallNode = createASTNode(BuiltInFunctionCall, functionName);
        funcCallNode->valType = SymTable_GetRetType(sym_Table, functionName);
        if(funcCallNode->valType != NONE) {
            fprintf(stderr, "Error: Function (%s) output is abandoned\n", functionName);
            exit(4);
        }
        funcCallNode->left = params;  // Attach parameters as left child
        // Attach the main identifier (e.g., 'ifj') as an additional node
        match(TOKEN_SEMICOLON);  // Match ';'
        return funcCallNode;
    }
    if (token.type == TOKEN_ASSIGNMENT) {
        // Handle assignment
        match(TOKEN_ASSIGNMENT);  // Match '='

        if(SymTable_Search(sym_Table, identifier) == NULL) { 
            fprintf(stderr, "Error: variable %s not defined\n", identifier);
            exit(3); //DOESNT EXIST OR A CONST ASSIGNMENT
        }
        
        if(!SymTable_GetMut(sym_Table, identifier)) {
            fprintf(stderr, "Error: invalid assignment to a constant: %s\n", identifier);
            exit(5);
        }
        SymTable_SetUsed(sym_Table, identifier, true);

        ASTNode* exprNode = parseExpression();  // Parse the expression to assign
        if(typeConv(SymTable_GetType(sym_Table, identifier), exprNode->valType) == NONE) { //typecheck
            fprintf(stderr, "Error: Cannot assign a value to a variable of uncompatible type: %s\n", identifier);
            exit(7);
        }
        ASTNode* assignNode = createASTNode(Assignment, identifier);  // Create an assignment node
        assignNode->left = exprNode;  // Attach the expression as the left child

        match(TOKEN_SEMICOLON);  // Match ';'
        return assignNode;
    } else if(token.type == TOKEN_LEFT_ROUND_BRACKET) {
        // Handle function call
        if(SymTable_Search(sym_Table, identifier) == NULL || SymTable_GetType(sym_Table, identifier) != FUNCTION) {
            fprintf(stderr, "Error: function %s not defined\n", identifier);
            exit(3); //DOESN'T EXIST OR NOT A FUNCTION
        }
        SymTable_SetUsed(sym_Table, identifier, true);
        ASTNode* funcCallNode = createASTNode(FuncCall, identifier);  // Create function call node
        if(SymTable_GetRetType(sym_Table, identifier) != NONE) {
            fprintf(stderr, "Error: Function (%s) output is abandoned\n", identifier);
            exit(4);
        }
        funcCallNode->left = parseFunctionCall(identifier);  // Attach the argument list as the left child

        match(TOKEN_SEMICOLON);  // Match ';'
        return funcCallNode;
    } else {
        // Handle syntax error for unexpected token after identifier
        fprintf(stderr, "Syntax error: unexpected token after identifier\n");
        exit(2);
    }
}


ASTNode* parseIfCondition() {
    match(TOKEN_KEYWORD_IF);           // Match 'if'
    match(TOKEN_LEFT_ROUND_BRACKET);   // Match '('

    // Parse the condition expression
    ASTNode* conditionNode = parseExpression();    
    match(TOKEN_RIGHT_ROUND_BRACKET);  // Match ')'

    // Handle nullable binding if present
    ASTNode* bindingNode = NULL;
    SymTable_NewScope(sym_Table);
    if (token.type == TOKEN_VERTICAL_BAR) {
        match(TOKEN_VERTICAL_BAR);      // Match '|'
        char* bindingVar = strdup(token.attribute.str);
        Symbol symbol;
        symbol.name = token.attribute.str;

        switch (conditionNode->valType) {
            case I32_NULLABLE:
                symbol.type = I32;
                break;
            case F64_NULLABLE:
                symbol.type = F64;
                break;
            case U8_ARRAY_NULLABLE:
                symbol.type = U8_ARRAY;
                break;
            default:
                fprintf(stderr, "Error: Nullable binding with non-nullable value.\n");
                exit(7);//INVALID CONDITION TYPE
        }
        
        symbol.mut = true;
        symbol.used = false;
        symbol.retType = NONE;
        symbol.paramList = NULL;
        SymTable_AddSymbol(sym_Table, &symbol);
        if_malloc_error(bindingVar);
        match(TOKEN_ID);                // Match identifier for nullable binding
        bindingNode = createASTNode(NullBinding, bindingVar);
        match(TOKEN_VERTICAL_BAR);      // Match closing '|'
    }
    
    if(bindingNode == NULL && conditionNode->valType != BOOL) { //PERHAPS
        fprintf(stderr, "Error: Cannot evaluate a condition\n");
        exit(7);
    }

    match(TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    ASTNode* trueBranch = parseStatementList();  // Parse statements in the true branch
    if(SymTable_UpperScope(sym_Table) != 0) {
        exit(9); //unused vars
    }
    match(TOKEN_RIGHT_CURLY_BRACKET); // Match '}'

    // Optional 'else' block
    ASTNode* falseBranch = NULL;
    if (token.type == TOKEN_KEYWORD_ELSE) {
        falseStatement = true;
        match(TOKEN_KEYWORD_ELSE);// Match 'else'
        if (token.type == TOKEN_KEYWORD_IF){
            falseBranch = parseIfCondition();
        } else{
            SymTable_NewScope(sym_Table);
            match(TOKEN_LEFT_CURLY_BRACKET); // Match '{'
            falseBranch = parseStatementList();  // Parse statements in the false branch
            if(SymTable_UpperScope(sym_Table) != 0) {
                exit(9); //unused vars
            }
            match(TOKEN_RIGHT_CURLY_BRACKET);    // Match '}'
        }
    }

    // Create the AST node for the if statement
    ASTNode* ifNode = createASTNode(IfCondition, NULL);
    ifNode->left = conditionNode;   // Attach condition as the left child
    ifNode->right = trueBranch;     // Attach true branch as the right child
    ifNode->next = falseBranch;     // Attach false branch as the next node
    ifNode->binding = bindingNode;    // Attach nullable binding as an extra node if present

    ASTNode *conditionStatementNode = createASTNode(ConditionalStatement, NULL);
    conditionStatementNode->left = ifNode;
    return conditionStatementNode;
}


ASTNode* parseWhileCondition() {
    match(TOKEN_KEYWORD_WHILE);         // Match 'while'
    match(TOKEN_LEFT_ROUND_BRACKET);    // Match '('

    // Parse the condition expression
    ASTNode* conditionNode = parseExpression();
    match(TOKEN_RIGHT_ROUND_BRACKET);   // Match ')'

    // Handle nullable binding if present
    ASTNode* bindingNode = NULL;
    SymTable_NewScope(sym_Table);
    if (token.type == TOKEN_VERTICAL_BAR) {
        match(TOKEN_VERTICAL_BAR);      // Match '|'
        char* bindingVar;
        if(isMatch(TOKEN_ID)) {
            bindingVar = strdup(token.attribute.str);
            if_malloc_error(bindingVar);
            Symbol symbol;
            symbol.name = token.attribute.str;
            switch (conditionNode->valType) {
                case I32_NULLABLE:
                    symbol.type = I32;
                    break;
                case F64_NULLABLE:
                    symbol.type = F64;
                    break;
                case U8_ARRAY_NULLABLE:
                    symbol.type = U8_ARRAY;
                    break;
                default:
                    fprintf(stderr, "Error: Nullable binding with non-nullable value.\n");
                    exit(7);//INVALID CONDITION TYPE
            }
            symbol.mut = true;
            symbol.used = false;
            symbol.retType = NONE;
            symbol.paramList = NULL;
            SymTable_AddSymbol(sym_Table, &symbol);
        }
        else{
            // Handle syntax error
            fprintf(stderr ,"Syntax error: expected %d, but got %d\n", TOKEN_ID, (TokenType)token.type);
            exit(2); // or handle error gracefully
        }
        match(TOKEN_ID);                // Match identifier for nullable binding
        bindingNode = createASTNode(NullBinding, bindingVar);
        match(TOKEN_VERTICAL_BAR);      // Match closing '|'
    }
    
    if(bindingNode == NULL && conditionNode->valType != BOOL) {
        fprintf(stderr, "Error: Cannot evaluate a condition\n");
        exit(7);
    }

    match(TOKEN_LEFT_CURLY_BRACKET);    // Match '{'
    ASTNode* bodyNode = parseStatementList();  // Parse the loop body
    if(SymTable_UpperScope(sym_Table) != 0) {
        exit(9); //unused vars
    }
    match(TOKEN_RIGHT_CURLY_BRACKET);   // Match '}'

    // Create the AST node for the while statement
    ASTNode* whileNode = createASTNode(WhileCondition, NULL);
    whileNode->left = conditionNode;    // Attach condition as the left child
    whileNode->right = bodyNode;        // Attach the body as the right child
    whileNode->binding = bindingNode;   // Attach the nullable binding, if any

    ASTNode *conditionStatementNode = createASTNode(ConditionalStatement, NULL);
    conditionStatementNode->left = whileNode;
    return conditionStatementNode;
}


ASTNode* parseReturnStatement() {
    match(TOKEN_KEYWORD_RETURN);  // Match 'return'

    // Check if there is an expression to return
    ASTNode* exprNode = NULL;
    if (token.type != TOKEN_SEMICOLON) {
        exprNode = parseExpression();  // Parse the expression
    }

    // Create the AST node for the return statement
    ASTNode* returnNode = createASTNode(ReturnStatement, NULL);
    returnNode->valType = (exprNode == NULL) ? NONE : exprNode->valType;
    type_returned = returnNode->valType;
    if(type_to_return != type_returned && typeConv(type_to_return, returnNode->valType) == NONE) {
        fprintf(stderr, "Error: tried to return a value of a wrong type in a function\n");
        exit(4); //WRONG RETURN TYPE
    }
    returnNode->left = exprNode;  // Attach the expression as the left child (if present)

    match(TOKEN_SEMICOLON);  // Match ';'
    return returnNode;
}


Token get_next_token(){
    if (stat_index <= tokenArr->size){
        return tokenArr->tokens[stat_index++];
    }
    exit(1);
}

void if_malloc_error(const char* string){
    if (string == NULL){
        fprintf(stderr, "Memory allocation failed for string\n");
        exit(99);
    }

}
