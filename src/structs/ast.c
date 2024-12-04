//
// Created by nur on 16.11.24.
//
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "stdio.h"
#include "ast.h"

ASTValue *valCpy(ASTValue *value) {
    ASTValue *newVal = NULL;
    if(value != NULL) {
        newVal = (ASTValue *)malloc(sizeof(ASTValue));
        (*newVal) = (*value);
    }
    return newVal;
}

ASTNode *createASTNode(NodeType nodeType, char *name)
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
    newNode->name = name ? strdup(name) : NULL;
    if (newNode->name == NULL && name)
    {
        fprintf(stderr, "Memory allocation failed for nodeType\n");
        free(newNode);
        exit(99);
    }
    // Initialize child pointers to NULL
    newNode->valType = NONE;
    newNode->value = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->next = NULL;
    newNode->binding = NULL;

    return newNode;
}

ASTNode *createASTNodeInteger(NodeType nodeType, int value)
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
    newNode->value = (ASTValue *) malloc(sizeof(ASTValue));
    if (newNode->value == NULL){
        fprintf(stderr, "Memory allocation failed for node value\n");
        free(newNode);
        exit(99);
    }
    newNode->value->integer = value;
    newNode->name = NULL;
    newNode->valType = I32;
    // Initialize child pointers to NULL
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->next = NULL;
    newNode->binding = NULL;

    return newNode;
}

ASTNode *createASTNodeReal(NodeType nodeType, double value)
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
    newNode->value = (ASTValue *) malloc(sizeof(ASTValue));
    if (newNode->value == NULL){
        fprintf(stderr, "Memory allocation failed for node value\n");
        free(newNode);
        exit(99);
    }
    // Allocate and copy the value (if provided)
    newNode->value->real = value;
    newNode->valType = F64;
    // Initialize child pointers to NULL
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->next = NULL;
    newNode->binding = NULL;
    newNode->name = NULL;

    return newNode;
}

ASTNode *createBinaryASTNode(NodeType operator, ASTNode * left, ASTNode *right)
{
    // Allocate memory for the ASTNode
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed for ASTNode\n");
        exit(99);
    }

    // Allocate and copy the operator as the node type
    newNode->nodeType = operator;
    newNode->valType = NONE;

    // Allocate and copy the operator as the value
    newNode->value = NULL;

    // Attach the left and right child nodes
    newNode->left = left;
    newNode->right = right;
    newNode->name = NULL;
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
    if (node->value != NULL) {
    if(node->value->string != NULL && node->nodeType == StringLiteral){
        free(node->value->string);
        node->value->string = NULL;
    }
        free(node->value);
        node->value = NULL;
    }
    if (node->left != NULL) {
        clearAstNode(node->left);
        node->left = NULL;
    }
    if (node->right != NULL) {
        clearAstNode(node->right);
        node->right = NULL;
    }
    if (node->next != NULL) {
        clearAstNode(node->next);
        node->next = NULL;
    }
    if (node->binding != NULL) {
        clearAstNode(node->binding);
        node->binding = NULL;
    }

    if(node->name != NULL) {
        free(node->name);
        node->name = NULL;
    }
    free(node);
    node = NULL;
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
    case AddOperation:
        return "AddOperation";
    case SubOperation:
        return "SubOperation";
    case MulOperation:
        return "MulOperation";
    case DivOperation:
        return "DivOperation";
    case LessEqOperation:
        return "LessEqOperation";
    case LessOperation:
        return "LessOperation";
    case EqualOperation:
        return "EqualOperation";
    case NotEqualOperation:
        return "NotEqualOperation";
    case GreaterEqOperation:
        return "GreaterEqOperation";
    case GreaterOperation:
        return "GreaterOperation";
    case FuncCall:
        return "FuncCall";
    case Identifier:
        return "Identifier";
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
    case ConditionalStatement:
        return "ConditionalStatement";
    case IfCondition:
        return "IfCondition";
    case WhileCondition:
        return "WhileCondition";
    case ReturnStatement:
        return "ReturnStatement";
    case Assignment:
        return "Assignment";
    default:
        return "Unknown";
    }
}

void inspectAstNode(ASTNode *node)
{
    if (node == NULL)
    {
        loginfo("Node is NULL");
        return;
    }
    loginfo("Node type: %s, with valType: %i", nodeTypeToString(node->nodeType), node->valType);
    if (node->name != NULL)
        loginfo("Node name: %s", node->name);
    else
        loginfo("Node name is NULL");
    if (node->left != NULL)
        loginfo("Left node type: %s", nodeTypeToString(node->left->nodeType));
    else
        loginfo("Left node is NULL");
    if (node->right != NULL)
        loginfo("Right node type: %s", nodeTypeToString(node->right->nodeType));
    else
        loginfo("Right node is NULL");
    if (node->next != NULL)
        loginfo("Next node type: %s", nodeTypeToString(node->next->nodeType));
    else
        loginfo("Next node is NULL");
    if (node->binding != NULL)
        loginfo("Binding node type: %s", nodeTypeToString(node->binding->nodeType));
    else
        loginfo("Binding node is NULL");
}
