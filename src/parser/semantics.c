#include "semantics.h"
#include <stdio.h>

type_t Sem_MathConv(ASTNode *left, ASTNode *right, ASTNode *higher_order) {
  NodeType oper = higher_order->nodeType;
  type_t typeL = left->valType;
  type_t typeR = right->valType;
  switch(oper) {
    case AddOperation:
    case SubOperation:
    case MulOperation:
    case DivOperation:
      
      if((typeL == typeR) && (typeL == I32 || typeL == F64 || typeL == I32_LITERAL || typeL == F64_LITERAL)) { //equal numeric types 
        higher_order->valType = typeL;
        if(typeL == I32_LITERAL || typeL == F64_LITERAL) {
          PerformArithm(left, right, higher_order);
        }
        return higher_order->valType;
      }
      
      if((typeL == I32_LITERAL || typeL == F64_LITERAL || typeL == F64) && (typeR == I32_LITERAL || typeR == F64_LITERAL || typeR == F64)) {
        higher_order->valType = F64;
        if(typeL == I32_LITERAL) {
          ASTNode *convNode = createASTNode(FuncCall, "ifj.i2f"); //convert impicitly
          convNode->valType = F64_LITERAL;
          convNode->value.real = (double)(left->value.integer);
          convNode->left = left;
          higher_order->left = convNode;
        }
        if(typeR == I32_LITERAL) {
          ASTNode *convNode = createASTNode(FuncCall, "ifj.i2f"); //convert implicitly
          convNode->valType = F64_LITERAL;
          convNode->value.real = (double)(right->value.integer);
          convNode->left = right;
          higher_order->right = convNode;
        }
        if(typeR != F64 && typeL != F64) {
          higher_order->valType = F64_LITERAL;
          PerformArithm(higher_order->left, right, higher_order);
        }
        return higher_order->valType; //I32Lit X I32Lit was already handled
      }
      
      if (typeL == I32) {
        if(typeR == I32_LITERAL) {
          higher_order->valType = I32;
          return I32;
        }
        if(typeR == F64_LITERAL && isRound(right->value.real)) {
          higher_order->valType = I32;
          ASTNode *convNode = createASTNode(FuncCall, "ifj.f2i"); //convert implicitly
          convNode->valType = I32_LITERAL;
          convNode->value.integer = (int)(right->value.real);
          convNode->left = right;
          higher_order->right = convNode;
          return I32;
        }
      }
      
      if(typeR == I32) {
        if(typeL == I32_LITERAL) {
          higher_order->valType = I32;
          return I32;
        }
        if(typeL == F64_LITERAL && isRound(left->value.real)) {
          higher_order->valType = I32;
          ASTNode *convNode = createASTNode(FuncCall, "ifj.f2i"); //convert implicitly
          convNode->valType = I32_LITERAL;
          convNode->value.integer = (int)(left->value.real);
          convNode->left = left;
          higher_order->left = convNode;
          return I32;
        }
      }
      return NONE;
    case EqualOperation:
    case NotEqualOperation:
      return NONE;
    case LessOperation:
    case LessEqOperation:
    case GreaterOperation:
    case GreaterEqOperation:
      return NONE;
    default:
      return NONE;
  }
  return NONE;
}

type_t Sem_AssignConv(ASTNode *left, ASTNode *right, ASTNode *higher_order) {
  /*switch(type1) {
    case I32:
      return (type2 == I32 || type2 == I32_LITERAL) ? I32 : NONE;
    case F64: 
      return (type2 == F64 || type2 == F64_LITERAL) ? F64 : NONE;
    case U8_ARRAY:
      return (type2 == U8_ARRAY) ? U8_ARRAY : NONE;
    case I32_NULLABLE:
      return (type2 == I32_NULLABLE || type2 == I32 || type2 == I32_LITERAL || type2 == NULL_LITERAL) ? I32_NULLABLE : NONE;
    case F64_NULLABLE:
      return (type2 == F64_NULLABLE || type2 == F64 || type2 == F64_LITERAL || type2 == NULL_LITERAL) ? F64_NULLABLE : NONE;
    case U8_ARRAY_NULLABLE:
      return (type2 == U8_ARRAY_NULLABLE || type2 == U8_ARRAY || type2 == NULL_LITERAL) ? U8_ARRAY_NULLABLE : NONE;
    case UNDEFINED:
      return type2;
    default: 
      return NONE;
  }*/

  type_t typeL = (left == NULL) ? UNDEFINED : left->valType;
  type_t typeR = (right == NULL) ? NONE : right->valType;
  NodeType oper = higher_order->nodeType;
  switch(oper) {
    case ConstDeclaration:
    case VarDeclaration:
      if(typeR == NONE) {
        higher_order->valType = NONE;
        return NONE;
      }
      switch(typeL) {
        case I32:
          if(typeR == NULL_LITERAL || typeR == I32_NULLABLE) {
            higher_order->valType = NONE;
            return NONE;
          }
        case I32_NULLABLE:
          if(typeR == I32 || typeR == I32_NULLABLE || typeR == NULL_LITERAL) {
            higher_order->valType = typeL; //both for I32 and I32_NULLABLE
            return typeL;
          }
          if(typeR == I32_LITERAL) {
            higher_order->valType = I32_LITERAL;
            higher_order->value.integer = right->value.integer;            
            return I32_LITERAL;
          }
          if(typeR == F64_LITERAL && isRound(right->value.real)) {
            higher_order->valType = I32_LITERAL;
            //TODO: f2i conversion!
            higher_order->value.integer = (int)(right->value.real);
            return I32_LITERAL;
          }
          return NONE;
        case F64:
          if(typeR == NULL_LITERAL || typeR == F64_NULLABLE) {
            higher_order->valType = NONE;
            return NONE;
          }
        case F64_NULLABLE:
          if(typeR == F64 || typeR == F64_NULLABLE || typeR == NULL_LITERAL) {
            higher_order->valType = typeL;
            return typeL;
          }
          if(typeR == F64_LITERAL) {
            higher_order->valType = F64_LITERAL;
            higher_order->value.real = right->value.real;
            return F64_LITERAL;
          }
          if(typeR == I32_LITERAL) {
            higher_order->valType = F64_LITERAL;
            //TODO: i2f conversion!
            higher_order->value.real = (double)(right->value.integer);
            return F64_LITERAL;
          }
          return NONE;
        case U8_ARRAY:
          if(typeR == NULL_LITERAL || typeR == U8_ARRAY_NULLABLE) {
            higher_order->valType = NONE;
            return NONE;
          }
        case U8_ARRAY_NULLABLE:
          if(typeR != U8_ARRAY_NULLABLE || typeR != U8_ARRAY) {
            higher_order->valType = NONE;
            return NONE;
          }
          higher_order->valType = typeL;
          return typeL;
        case UNDEFINED:
          if(typeR == STR_LITERAL || typeR == FUNCTION){
            higher_order->valType = NONE;
            return NONE;
          }
          higher_order->valType = typeR;
          return typeR;
        default:
          fprintf(stderr, "London bridge is falling down\n");
          exit(99);
      }
    case Assignment:
      higher_order->valType = typeL;
      return typeL;
    default:
      fprintf(stderr, "Cuz I've blown it up\n");
      exit(99);
  }
  
}

bool isRound(double literal) {
  if((int)literal == literal)
    return true;
  return false;
}

void PerformArithm(ASTNode *left, ASTNode *right, ASTNode *higher_order) {
  if(left->valType == I32_LITERAL && right->valType == I32_LITERAL) {
    int A = left->value.integer;
    int B = right->value.integer;
    switch(higher_order->nodeType) {
      case AddOperation:
        higher_order->value.integer = A + B;
        return;
      case SubOperation:
        higher_order->value.integer = A - B;
        return;
      case MulOperation:
        higher_order->value.integer = A * B;
        return;
      case DivOperation:
        if(B == 0) {
          exit(10);
        }
        higher_order->value.integer = A / B;
        return;
      default:
        exit(99);
    }
  }

  double A = (left->valType == I32_LITERAL) ? (double)(left->value.integer) : left->value.real;
  double B = (right->valType == I32_LITERAL) ? (double)(right->value.integer) : right->value.real;
  switch (higher_order->nodeType) {
    case AddOperation:
      higher_order->value.real = A + B;
      return;
    case SubOperation:
      higher_order->value.real = A - B;
      return;
    case MulOperation:
      higher_order->value.real = A * B;
      return;
    case DivOperation:
      if (B == 0.0) {
        exit(10);
      }
      higher_order->value.real = A / B;
      return;
    default:
      exit(99);
  }
}
