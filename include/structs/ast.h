//
// Created by malbert on 10/23/24.
//


typedef struct ASTNode {
    char* nodeType; // Type of the node (e.g., "Variable", "FunctionCall", etc.)
    char* value;    // Literal value or identifier
    struct ASTNode* left;      // Left child node
    struct ASTNode* right;     // Right child node
    struct ASTNode* next;      // Pointer to the next statement (for sequences of statements)
    struct ASTNode* binding;   // Nullable binding, specific to 'if' statements
} ASTNode;

ASTNode* createASTNode(char* nodeType, char* value);
ASTNode* createBinaryASTNode(char* nodeType, ASTNode* left, ASTNode* right);