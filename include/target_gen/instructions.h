//
// Created by nur on 16.11.24.
//

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Instruction types
typedef enum
{
    // Frame and function call instructions
    INST_MOVE,
    INST_CREATEFRAME,
    INST_PUSHFRAME,
    INST_POPFRAME,
    INST_DEFVAR,
    INST_CALL,
    INST_RETURN,

    // Stack instructions
    INST_PUSHS,
    INST_POPS,
    INST_CLEARS,

    // Arithmetic instructions
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_IDIV,
    INST_ADDS,
    INST_SUBS,
    INST_MULS,
    INST_DIVS,
    INST_IDIVS,

    // Relational instructions
    INST_LT,
    INST_GT,
    INST_EQ,
    INST_LTS,
    INST_GTS,
    INST_EQS,

    // Boolean instructions
    INST_AND,
    INST_OR,
    INST_NOT,
    INST_ANDS,
    INST_ORS,
    INST_NOTS,

    // Conversion instructions
    INST_INT2FLOAT,
    INST_FLOAT2INT,
    INST_INT2CHAR,
    INST_STRI2INT,
    INST_INT2FLOATS,
    INST_FLOAT2INTS,
    INST_INT2CHARS,
    INST_STRI2INTS,

    // Input-output instructions
    INST_READ,
    INST_WRITE,

    // String instructions
    INST_CONCAT,
    INST_STRLEN,
    INST_GETCHAR,
    INST_SETCHAR,

    // Type instructions
    INST_TYPE,

    // Program flow instructions
    INST_LABEL,
    INST_JUMP,
    INST_JUMPIFEQ,
    INST_JUMPIFNEQ,
    INST_JUMPIFEQS,
    INST_JUMPIFNEQS,
    INST_EXIT,

    // Debugging instructions
    INST_BREAK,
    INST_DPRINT
} InstType;

typedef enum
{
    // None is set for operands of instructions that need less that 2 operands.
    //
    // For example, DEVFAR <var>, where .opSecond's and .opThird's 
    // operands type would be set to OP_NONE.
    OP_NONE,

    OP_VAR,

    OP_CONST_BOOL,
    OP_CONST_INT64,
    OP_CONST_FLOAT64,
    OP_CONST_STRING, // String literals
    OP_CONST_NIL,

    OP_LABEL,

    OP_TYPE, // Only used for TYPE instruction
} OperandType;

typedef enum {
    FRAME_GF,
    FRAME_LF,
    FRAME_TF,
} VarFrameType;

typedef struct
{
    VarFrameType frame;
    char *name;
} Variable;

typedef union
{
    bool boolean;
    int64_t i64;
    double f64;
    char *string;
    Variable var;
} OperandAttribute;

typedef struct
{
    OperandType type;
    OperandAttribute attr;
} Operand;

typedef struct
{
    InstType type;
    Operand opFirst;
    Operand opSecond;
    Operand opThird;
} Instruction;

OperandAttribute initVarAttr(VarFrameType frame, char *name);
OperandAttribute initStringAttr(char *string);

Operand initOperand(OperandType type, OperandAttribute attr);
Operand initVarOperand(OperandType type, VarFrameType frame, char *name);
Operand initStringOperand(OperandType type, char *string);
Operand initEmptyOperand();

void destroyOperand(Operand *op);

Instruction initInstr0(InstType type);
Instruction initInstr1(InstType type, Operand opFirst);
Instruction initInstr2(InstType type, Operand opFirst, Operand opSecond);
Instruction initInstr3(InstType type, Operand opFirst, Operand opSecond, Operand opThird);

void destroyInstruction(Instruction *inst);

void printInstruction(Instruction *inst, FILE* stream);

#endif // INSTRUCTIONS_H
