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
        fprintf(stderr, "Types are equal\n");
        PerformArithm(left, right, higher_order);
        return higher_order->valType;
      }

      if((typeL == I32_LITERAL || typeL == I32) && (typeR == I32_LITERAL || typeR == I32)) {
        higher_order->valType = I32;
        fprintf(stderr, "Types are I32-like\n");
        PerformArithm(left, right, higher_order);
        if(typeL == I32_LITERAL && typeR == I32_LITERAL) {
          higher_order->valType = I32_LITERAL;
        }
        return higher_order->valType;
      }

      if((typeL == F64_LITERAL || typeL == F64) && (typeR == F64_LITERAL || typeR == F64)) {
        fprintf(stderr, "Types are F64-like\n");
        higher_order->valType = F64;
        PerformArithm(left, right, higher_order);
        if(typeL == F64_LITERAL && typeR == F64_LITERAL) {
          higher_order->valType = F64_LITERAL;
        }
        return higher_order->valType;
      }

      if(typeL == I32_LITERAL || typeR == I32_LITERAL) {
        ASTNode *IntLit = (typeL == I32_LITERAL) ? left : right;
        ASTNode *other  = (left == IntLit) ? right : left;
        if(other->valType == F64 || other->valType == F64_LITERAL) {
          higher_order->valType = other->valType;
          IntLit->valType = F64_LITERAL;
          IntLit->value->real = (double)(IntLit->value->integer);
          PerformArithm(left, right, higher_order);
          return higher_order->valType;
        }
      }
      
      if (typeL == I32 || typeR == I32) {
        ASTNode *IntVal = (typeL == I32) ? left : right;
        ASTNode *other = (IntVal == left) ? right : left;
        if(other->valType == F64_LITERAL && isRound(other->value->real)) {
          higher_order->valType = I32;
          other->valType = I32_LITERAL;
          other->value->integer = (int)(other->value->real);
          PerformArithm(left, right, higher_order);
          return I32;
        }
      }      
      return NONE;
    case EqualOperation:
    case NotEqualOperation:
      if(typeL == typeR && (typeL == I32 || typeL == I32_NULLABLE || typeL == F64 || typeL == F64_NULLABLE || typeL == I32_LITERAL || typeL == F64_LITERAL || typeL == NULL_LITERAL)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeL == NULL_LITERAL && (typeR == I32_NULLABLE || typeR == F64_NULLABLE || typeR == U8_ARRAY_NULLABLE)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeR == NULL_LITERAL && (typeL == I32_NULLABLE || typeL == F64_NULLABLE || typeR == U8_ARRAY_NULLABLE)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeR == I32_LITERAL && (typeL == F64 || typeL == F64_NULLABLE || typeL == F64_LITERAL)) {
        right->value->real = (double)(right->value->integer);
        right->valType = F64_LITERAL;
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeL == I32_LITERAL && (typeR == F64 || typeR == F64_NULLABLE || typeR == F64_LITERAL)) {
        left->value->real = (double)(left->value->integer);
        left->valType = F64_LITERAL;
        higher_order->valType = BOOL;
        return BOOL;
      }
      if((typeR == F64 || typeR == F64_NULLABLE || typeR == F64_LITERAL) && (typeL == F64 || typeL == F64_NULLABLE || typeL == F64_LITERAL)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeR == I32 && typeL == F64_LITERAL && isRound(left->value->real)) {
        left->value->integer = (int)(left->value->real);
        left->valType = I32_LITERAL;
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeL == I32 && typeR == F64_LITERAL && isRound(right->value->real)) {
        right->value->integer = (int)(right->value->real);
        right->valType = I32_LITERAL;
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeR == I32 && typeL == I32_LITERAL) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if(typeL == I32 && typeR == I32_LITERAL) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      return NONE;
    case LessOperation:
    case LessEqOperation:
    case GreaterOperation:
    case GreaterEqOperation:
      //fprintf(stderr, "Left is %d right is %d\n", (int)typeL, (int)typeR);
      if((typeL == typeR) && (typeL == I32 || typeL == F64 || typeL == I32_LITERAL || typeL == F64_LITERAL)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if((typeL == I32 || typeL == I32_LITERAL) && (typeR == I32 || typeR == I32_LITERAL)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if((typeL == F64 || typeL == F64_LITERAL) && (typeR == F64 || typeR == F64_LITERAL)) {
        higher_order->valType = BOOL;
        return BOOL;
      }
      if((typeL == F64 && typeR == I32_LITERAL) || (typeL == I32_LITERAL && typeR == F64)) {
        ASTNode *IntLit = (typeR == I32_LITERAL) ? right : left;
        IntLit->value->real = (double)(IntLit->value->integer);
        IntLit->valType = F64_LITERAL;
        higher_order->valType = BOOL;
        return BOOL;
      }
      if((typeL == F64_LITERAL && typeR == I32_LITERAL) || (typeR == F64_LITERAL && typeL == I32_LITERAL)) {
        ASTNode *IntLit = (typeR == I32_LITERAL) ? right : left;
        IntLit->value->real = (double)(IntLit->value->integer);
        IntLit->valType = F64_LITERAL;
        higher_order->valType = BOOL;
        return BOOL;
      }
      return NONE;
    default:
      return NONE;
  }
  return NONE;
}

type_t Sem_AssignConv(ASTNode *left, ASTNode *right, ASTNode *higher_order) {

  NodeType oper = higher_order->nodeType;
  type_t typeL = (oper == Assignment) ? higher_order->valType : ((left == NULL) ? UNDEFINED : left->valType); //in case var has no typenode 
  type_t typeR = right->valType; //to prevent segfaults
  
  if(typeR == NONE) {
    higher_order->valType = NONE;
    return NONE;
  }
  switch(typeL) {
    case I32_LITERAL:
    case I32:
      if(typeR == NULL_LITERAL || typeR == I32_NULLABLE) {
        higher_order->valType = NONE;
        return NONE;
      }
    case I32_NULLABLE:
      if(typeR == I32 || typeR == I32_NULLABLE || typeR == NULL_LITERAL) {
        higher_order->valType = typeL; //both for I32 and I32_NULLABLE
        higher_order->value = valCpy(right->value);
        return typeL;
      }
      if(typeR == I32_LITERAL) {
        higher_order->valType = typeL;
        higher_order->value = valCpy(right->value);
        return typeL;
      }
      if(typeR == F64_LITERAL && isRound(right->value->real)) {
        right->valType = I32_LITERAL;
        right->value->integer = (int)(right->value->real);
        higher_order->valType = typeL;
        higher_order->value = valCpy(right->value);
        return typeL;
      }
      return NONE;
    case F64_LITERAL:
    case F64:
      if(typeR == NULL_LITERAL || typeR == F64_NULLABLE) {
        higher_order->valType = NONE;
        return NONE;
      }
    case F64_NULLABLE:
      if(typeR == F64 || typeR == F64_NULLABLE || typeR == NULL_LITERAL) {
        higher_order->valType = typeL;
        higher_order->value = valCpy(right->value);
        return typeL;
      }
      if(typeR == F64_LITERAL) {
        higher_order->valType = typeL;
        higher_order->value = valCpy(right->value);
        return typeL;
      }
      if(typeR == I32_LITERAL) {
        right->valType = F64_LITERAL;
        right->value->real = (double)(right->value->integer);
        higher_order->valType = typeL;
        higher_order->value = valCpy(right->value);
        return typeL;
      }
      return NONE;
    case U8_ARRAY:
      if(typeR == NULL_LITERAL || typeR == U8_ARRAY_NULLABLE) {
        higher_order->valType = NONE;
        return NONE;
      }
    case U8_ARRAY_NULLABLE:
      if(typeR != U8_ARRAY_NULLABLE && typeR != U8_ARRAY && typeR != NULL_LITERAL) {
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
      higher_order->value = valCpy(right->value);
      return typeR;
    default:
      fprintf(stderr, "London bridge is falling down with nodetype %d\n", (int)higher_order->nodeType);
      exit(99);
    } 
}

bool isRound(double literal) {
  if((int)literal == literal)
    return true;
  return false;
}

void PerformArithm(ASTNode *left, ASTNode *right, ASTNode *higher_order) {
  if(right->value == NULL || left->value == NULL) {
    fprintf(stderr, "NULL...");
    return;
  }
  higher_order->value = (ASTValue *)malloc(sizeof(ASTValue));
  if((left->valType == I32 || left->valType == I32_LITERAL) && (right->valType == I32 || right->valType == I32_LITERAL)) {
    int A = left->value->integer;
    int B = right->value->integer;
    switch(higher_order->nodeType) {
      case AddOperation:
        fprintf(stderr, "%d + %d = %d\n", A, B, A + B);
        higher_order->value->integer = A + B;
        return;
      case SubOperation:
        fprintf(stderr, "%d - %d = %d\n", A, B, A - B);
        higher_order->value->integer = A - B;
        return;
      case MulOperation:
        fprintf(stderr, "%d * %d = %d\n", A, B, A * B);
        higher_order->value->integer = A * B;
        return;
      case DivOperation:
        fprintf(stderr, "%d / %d = %d\n", A, B, A / B);
        if(B == 0) {
          exit(10);
        }
        higher_order->value->integer = A / B;
        return;
      default:
        exit(99);
    }
  }

  double A = (left->valType == I32_LITERAL) ? (double)(left->value->integer) : left->value->real;
  double B = (right->valType == I32_LITERAL) ? (double)(right->value->integer) : right->value->real;
  switch (higher_order->nodeType) {
    case AddOperation:
      fprintf(stderr, "%lf + %lf = %lf\n", A, B, A + B);
      higher_order->value->real = A + B;
      return;
    case SubOperation:
      fprintf(stderr, "%lf - %lf = %lf\n", A, B, A - B);
      higher_order->value->real = A - B;
      return;
    case MulOperation:
      fprintf(stderr, "%lf * %lf = %lf\n", A, B, A * B);
      higher_order->value->real = A * B;
      return;
    case DivOperation:
      fprintf(stderr, "%lf / %lf = %lf\n", A, B, A / B);
      if (B == 0.0) {
        exit(10);
      }
      higher_order->value->real = A / B;
      return;
    default:
      exit(99);
  }
}


type_t Sem_ParamConv(type_t ParamType, type_t ArgType) {
  switch(ParamType) {
    case STR_LITERAL:
      if(ArgType == STR_LITERAL || ArgType == U8_ARRAY) {
        return STR_LITERAL;
      }
      return NONE;
    case I32:
      if(ArgType == I32_NULLABLE) {
        return NONE;
      }
    case I32_NULLABLE:
      if(ArgType == I32 || ArgType == I32_LITERAL || ArgType == I32_NULLABLE || ArgType == NULL_LITERAL) {
        return ParamType;
      }
      return NONE;
    case F64:
      if(ArgType == F64_NULLABLE) {
        return NONE;
      }
    case F64_NULLABLE:
      if(ArgType == F64 || ArgType == F64_LITERAL || ArgType == F64_NULLABLE || ArgType ==I32_LITERAL || ArgType == NULL_LITERAL) {
        return ParamType;
      }
      return NONE;
    case U8_ARRAY:
      if(ArgType == U8_ARRAY_NULLABLE) {
        return NONE;
      }
    case U8_ARRAY_NULLABLE:
      if(ArgType == U8_ARRAY || U8_ARRAY_NULLABLE || ArgType == NULL_LITERAL) {
        return ParamType;
      }
      return NONE;
    case UNDEFINED:
      return ParamType;
    default:
      exit(99);
  }    
}
