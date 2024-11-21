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
    newNode->value = value ? strdup(value) : NULL;
    if (newNode->value == NULL && value)
    {
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

ASTNode *createBinaryASTNode(char *operator, ASTNode * left, ASTNode *right)
{
    // Allocate memory for the ASTNode
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed for ASTNode\n");
        exit(99);
    }

    // Allocate and copy the operator as the node type
    newNode->nodeType = BinaryOperation;

    // Allocate and copy the operator as the value
    newNode->value = strdup(operator);
    if (newNode->value == NULL)
    {
        fprintf(stderr, "Memory allocation failed for operator\n");
        free(newNode);
        exit(99);
    }

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
    if (node->value != NULL)
    {
        free(node->value);
        node->value = NULL;
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
