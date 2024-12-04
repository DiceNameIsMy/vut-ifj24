//
// Created by malbert on 10/23/24.
//
#include "types.h"

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
    AddOperation,
    SubOperation,
    MulOperation,
    DivOperation,
    LessEqOperation,
    LessOperation,
    EqualOperation,
    NotEqualOperation,
    GreaterEqOperation,
    GreaterOperation,
    FuncCall,
    Identifier,
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    NullLiteral,
    VarDeclaration,
    NullBinding,
    ConditionalStatement,
    IfCondition,
    WhileCondition,
    ReturnStatement,
    Assignment,
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
typedef union ASTValue{
    char *string;
    int integer;
    double real;
} ASTValue;

typedef struct ASTNode {
    NodeType nodeType; // Type of the node (e.g., "Variable", "FunctionCall", etc.)
    type_t valType;
    ASTValue* value;    // Literal value or identifier
    char *name;        //TODO: free afterwards
    struct ASTNode* left;      // Left child node
    struct ASTNode* right;     // Right child node
    struct ASTNode* next;      // Pointer to the next statement (for sequences of statements)
    struct ASTNode* binding;   // Nullable binding, specific to 'if' statements
} ASTNode;

ASTValue *valCpy(ASTValue *value);
ASTNode* createASTNode(NodeType nodeType, char* value);
ASTNode* createASTNodeInteger(NodeType nodeType, int value);
ASTNode* createASTNodeReal(NodeType nodeType, double value);
ASTNode* createBinaryASTNode(NodeType operator, ASTNode* left, ASTNode* right);
void clearAstNode(ASTNode *node);

const char *nodeTypeToString(NodeType nodeType);
void inspectAstNode(ASTNode *node);

#endif // AST_H
