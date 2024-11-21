//
// Created by malbert on 10/23/24.
//
#ifndef AST_H
#define AST_H


typedef enum {
    Program,
    Prolog,
    FunctionDef,
    Parameter,
    ReturnType,
    DataType,
    BlockStatement,
    ConstDeclaration,
    BinaryOperation,
    FuncCall,
    Identifier,
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    NullLiteral,
    VarDeclaration,
    NullBinding,
    IfStatement,
    WhileStatement,
    ReturnStatement,
    Assignment,
    BuiltInFunctionCall,
} NodeType;

//const char* getQuestionTypeName(NodeType type) {
//    switch (type) {
//        case Test: return "Test";
//        case Text: return "Text";
//        case Numbers: return "Numbers";
//        case YesNo: return "YesNo";
//        case MultipleChoice: return "MultipleChoice";
//        default: return "Unknown";
//    }
//}

typedef struct ASTNode {
    NodeType nodeType; // Type of the node (e.g., "Variable", "FunctionCall", etc.)
    char* value;    // Literal value or identifier
    struct ASTNode* left;      // Left child node
    struct ASTNode* right;     // Right child node
    struct ASTNode* next;      // Pointer to the next statement (for sequences of statements)
    struct ASTNode* binding;   // Nullable binding, specific to 'if' statements
} ASTNode;

ASTNode* createASTNode(NodeType nodeType, char* value);
ASTNode* createBinaryASTNode(char* operator, ASTNode* left, ASTNode* right);
void clearAstNode(ASTNode *node);

#endif // AST_H
