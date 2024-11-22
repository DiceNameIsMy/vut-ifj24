//
// Created by nur on 16.11.24.
//
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "../../include/structs/ast.h" // Assuming the AST structure is defined here

ASTNode *createASTNode(NodeType nodeType, char *value)
{
    // Allocate memory for the ASTNode
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed for ASTNode\n");
        exit(99);
    }

    // Allocate and copy the nodeType
    newNode->nodeType = nodeType;

    // Allocate and copy the value (if provided)
    newNode->value.string = value ? strdup(value) : NULL;
    if (newNode->value.string == NULL && value) {
        fprintf(stderr, "Memory allocation failed for nodeType\n");
        free(newNode);
        exit(99);
    }
    // Initialize child pointers to NULL
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->next = NULL;
    newNode->binding = NULL;

    return newNode;
}

ASTNode* createASTNodeInteger(NodeType nodeType, int value) {
    // Allocate memory for the ASTNode
    ASTNode* newNode = (ASTNode*)malloc(sizeof(ASTNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for ASTNode\n");
        exit(99);
    }
    // Allocate and copy the nodeType
    newNode->nodeType = nodeType;
    // Allocate and copy the value (if provided)
    newNode->value.integer = value;
    // Initialize child pointers to NULL
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->next = NULL;
    newNode->binding = NULL;

    return newNode;
}


ASTNode* createASTNodeReal(NodeType nodeType, double value) {
    // Allocate memory for the ASTNode
    ASTNode* newNode = (ASTNode*)malloc(sizeof(ASTNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for ASTNode\n");
        exit(99);
    }

    // Allocate and copy the nodeType
    newNode->nodeType = nodeType;


    // Allocate and copy the value (if provided)
    newNode->value.real = value;
    // Initialize child pointers to NULL
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->next = NULL;
    newNode->binding = NULL;

    return newNode;
}

ASTNode* createBinaryASTNode(NodeType operator, ASTNode* left, ASTNode* right) {
    // Allocate memory for the ASTNode
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed for ASTNode\n");
        exit(99);
    }

    // Allocate and copy the operator as the node type
    newNode->nodeType = operator;

    // Allocate and copy the operator as the value

    // Attach the left and right child nodes
    newNode->left = left;
    newNode->right = right;

    // Initialize the next pointer to NULL (binary nodes usually don't use it)
    newNode->next = NULL;
    newNode->binding = NULL;

    return newNode;
}

void clearAstNode(ASTNode *node)
{
    if (node == NULL)
    {
        return;
    }
    clearAstNode(node->left);
    clearAstNode(node->right);
    clearAstNode(node->next);
    clearAstNode(node->binding);
    free(node);
}

const char *nodeTypeToString(NodeType nodeType)
{
    switch (nodeType)
    {
    case Program:
        return "Program";
    case Prolog:
        return "Prolog";
    case FunctionDef:
        return "FunctionDef";
    case Parameter:
        return "Parameter";
    case ReturnType:
        return "ReturnType";
    case DataType:
        return "DataType";
    case BlockStatement:
        return "BlockStatement";
    case ConstDeclaration:
        return "ConstDeclaration";
    case FuncCall:
        return "FuncCall";
    case IntLiteral:
        return "IntLiteral";
    case FloatLiteral:
        return "FloatLiteral";
    case StringLiteral:
        return "StringLiteral";
    case NullLiteral:
        return "NullLiteral";
    case VarDeclaration:
        return "VarDeclaration";
    case NullBinding:
        return "NullBinding";
    case IfStatement:
        return "IfStatement";
    case WhileStatement:
        return "WhileStatement";
    case ReturnStatement:
        return "ReturnStatement";
    case BuiltInFunctionCall:
        return "BuiltInFunctionCall";
    }
}
