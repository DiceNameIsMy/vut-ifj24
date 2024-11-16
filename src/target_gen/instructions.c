#include <stdlib.h>
#include <inttypes.h>

#include "target_gen/instructions.h"

/**********************************************************/
/* Function Declarations */
/**********************************************************/

const char *getInstructionKeyword(Instruction *inst);
void printInstructionKeyword(Instruction *inst);

bool hasNoOperands(Instruction *inst);
bool hasOneOperand(Instruction *inst);
bool hasTwoOperands(Instruction *inst);
bool hasThreeOperands(Instruction *inst);

bool isSymbolOperand(Operand *op);

void printVar(Operand *op);
void printConst(Operand *op);
void printLabel(Operand *op);
void printType(Operand *op);
void printOperand(Operand *op);

void printFirstOperand(Instruction *inst);
void printSecondOperand(Instruction *inst);
void printThirdOperand(Instruction *inst);

/**********************************************************/
/* Function Definitions */
/**********************************************************/

// TODO: Maybe add a static stream pointer for testing

const char *getInstructionKeyword(Instruction *inst)
{
    switch (inst->type)
    {
    case INST_MOVE:
        return "MOVE";
    case INST_CREATEFRAME:
        return "CREATEFRAME";
    case INST_PUSHFRAME:
        return "PUSHFRAME";
    case INST_POPFRAME:
        return "POPFRAME";
    case INST_DEFVAR:
        return "DEFVAR";
    case INST_CALL:
        return "CALL";
    case INST_RETURN:
        return "RETURN";

    case INST_PUSHS:
        return "PUSHS";
    case INST_POPS:
        return "POPS";
    case INST_CLEARS:
        return "CLEARS";

    case INST_ADD:
        return "ADD";
    case INST_SUB:
        return "SUB";
    case INST_MUL:
        return "MUL";
    case INST_DIV:
        return "DIV";
    case INST_IDIV:
        return "IDIV";
    case INST_ADDS:
        return "ADDS";
    case INST_SUBS:
        return "SUBS";
    case INST_MULS:
        return "MULS";
    case INST_DIVS:
        return "DIVS";
    case INST_IDIVS:
        return "IDIVS";

    case INST_LT:
        return "LT";
    case INST_GT:
        return "GT";
    case INST_EQ:
        return "EQ";
    case INST_LTS:
        return "LTS";
    case INST_GTS:
        return "GTS";
    case INST_EQS:
        return "EQS";

    case INST_AND:
        return "AND";
    case INST_OR:
        return "OR";
    case INST_NOT:
        return "NOT";
    case INST_ANDS:
        return "ANDS";
    case INST_ORS:
        return "ORS";
    case INST_NOTS:
        return "NOTS";

    case INST_INT2FLOAT:
        return "INT2FLOAT";
    case INST_FLOAT2INT:
        return "FLOAT2INT";
    case INST_INT2CHAR:
        return "INT2CHAR";
    case INST_STRI2INT:
        return "STRI2INT";
    case INST_INT2FLOATS:
        return "INT2FLOATS";
    case INST_FLOAT2INTS:
        return "FLOAT2INTS";
    case INST_INT2CHARS:
        return "INT2CHARS";
    case INST_STRI2INTS:
        return "STRI2INTS";

    case INST_READ:
        return "READ";
    case INST_WRITE:
        return "WRITE";

    case INST_CONCAT:
        return "CONCAT";
    case INST_STRLEN:
        return "STRLEN";
    case INST_GETCHAR:
        return "GETCHAR";
    case INST_SETCHAR:
        return "SETCHAR";

    case INST_TYPE:
        return "TYPE";

    case INST_LABEL:
        return "LABEL";
    case INST_JUMP:
        return "JUMP";
    case INST_JUMPIFEQ:
        return "JUMPIFEQ";
    case INST_JUMPIFNEQ:
        return "JUMPIFNEQ";
    case INST_JUMPIFEQS:
        return "JUMPIFEQS";
    case INST_JUMPIFNEQS:
        return "JUMPIFNEQS";
    case INST_EXIT:
        return "EXIT";

    case INST_BREAK:
        return "BREAK";
    case INST_DPRINT:
        return "DPRINT";
    default:
        return "# Unknown instruction. Operands:";
    }
}

void printInstructionKeyword(Instruction *inst)
{
    printf("%s", getInstructionKeyword(inst));
}

bool hasNoOperands(Instruction *inst)
{
    switch (inst->type)
    {
    case INST_CREATEFRAME:
    case INST_PUSHFRAME:
    case INST_POPFRAME:
    case INST_CLEARS:
    case INST_RETURN:
    case INST_BREAK:
        return true;
    default:
        return false;
    }
}

bool hasOneOperand(Instruction *inst)
{
    switch (inst->type)
    {
    case INST_DEFVAR:
    case INST_CALL:
    case INST_PUSHS:
    case INST_POPS:
    case INST_WRITE:
    case INST_LABEL:
    case INST_JUMP:
    case INST_JUMPIFEQS:
    case INST_JUMPIFNEQS:
    case INST_EXIT:
    case INST_DPRINT:
        return true;
    default:
        return false;
    }
}

bool hasTwoOperands(Instruction *inst)
{
    switch (inst->type)
    {
    case INST_MOVE:
    case INST_INT2FLOAT:
    case INST_FLOAT2INT:
    case INST_INT2CHAR:
    case INST_INT2FLOATS:
    case INST_FLOAT2INTS:
    case INST_INT2CHARS:
    case INST_READ:
    case INST_STRLEN:
    case INST_TYPE:
        return true;
    default:
        return false;
    }
}

bool hasThreeOperands(Instruction *inst)
{
    switch (inst->type)
    {
    case INST_ADD:
    case INST_SUB:
    case INST_MUL:
    case INST_DIV:
    case INST_IDIV:
    case INST_ADDS:
    case INST_SUBS:
    case INST_MULS:
    case INST_DIVS:
    case INST_IDIVS:
    case INST_LT:
    case INST_GT:
    case INST_EQ:
    case INST_LTS:
    case INST_GTS:
    case INST_EQS:
    case INST_AND:
    case INST_OR:
    case INST_NOT:
    case INST_ANDS:
    case INST_ORS:
    case INST_NOTS:
    case INST_STRI2INT:
    case INST_STRI2INTS:
    case INST_CONCAT:
    case INST_GETCHAR:
    case INST_SETCHAR:
    case INST_JUMPIFEQ:
    case INST_JUMPIFNEQ:
        return true;
    default:
        return false;
    }
}

bool isSymbolOperand(Operand *op)
{
    return op->type == OP_VAR || op->type == OP_CONST_BOOL || op->type == OP_CONST_INT64 || op->type == OP_CONST_FLOAT64 || op->type == OP_CONST_STRING || op->type == OP_CONST_NIL;
}

void printVar(Operand *op)
{
    char *frame = NULL;
    if (op->attr.var.frame == GF)
        frame = "GF";
    else if (op->attr.var.frame == LF)
        frame = "LF";
    else if (op->attr.var.frame == TF)
        frame = "TF";
    else
        loginfo("Invalid frame type %d\n", op->attr.var.frame);

    printf("%s@%s", frame, op->attr.var.name);
}

void printConst(Operand *op)
{
    switch (op->type)
    {
    case OP_CONST_BOOL:
        printf("bool@%s", op->attr.boolean ? "true" : "false");
        break;
    case OP_CONST_INT64:
        // PRId64 is used to ensure that it would be correctly printed
        printf("int@%" PRId64, op->attr.i64);
        break;
    case OP_CONST_FLOAT64:
        printf("float@%.14gp+0", op->attr.f64);
        break;
    case OP_CONST_STRING:
        printf("string@%s", op->attr.string);
        break;
    case OP_CONST_NIL:
        printf("nil@nil");
        break;
    default:
        loginfo("Invalid constant type %d\n", op->type);
    }
}

void printLabel(Operand *op)
{
    printf("label@%s", op->attr.string);
}

void printType(Operand *op)
{
    printf("type@%s", op->attr.string);
}

void printOperand(Operand *op)
{
    switch (op->type)
    {
    case OP_VAR:
        printVar(op);
        break;
    case OP_CONST_BOOL:
    case OP_CONST_INT64:
    case OP_CONST_FLOAT64:
    case OP_CONST_STRING:
    case OP_CONST_NIL:
        printConst(op);
        break;
    case OP_LABEL:
        printLabel(op);
        break;
    case OP_TYPE:
        printType(op);
        break;
    default:
        loginfo("Invalid operand type %d\n", op->type);
    }
}

void printFirstOperand(Instruction *inst)
{
    Operand op = inst->opFirst;
    switch (inst->type)
    {
    case INST_PUSHS: // Only allows symbol
    case INST_WRITE:
    case INST_EXIT:
        if (isSymbolOperand(&op))
            printOperand(&op);
        else
            loginfo("Invalid instruction %s. Expected symbol as 1st operand, got %d\n", getInstructionKeyword(inst), op.type);
        break;

    case INST_CALL: // Only allows label
    case INST_LABEL:
    case INST_JUMP:
    case INST_JUMPIFEQS:
    case INST_JUMPIFNEQS:
        if (op.type == OP_LABEL)
            printOperand(&op);
        else
            loginfo("Invalid instruction %s. Expected label as 1st operand, got %d\n", getInstructionKeyword(inst), op.type);
        break;

    default: // Every other instruction only allows var
        if (op.type == OP_VAR)
            printOperand(&op);
        else
            loginfo("Invalid instruction %s. Expected var as 1st operand, got %d\n", getInstructionKeyword(inst), op.type);
        break;
    }
}

void printSecondOperand(Instruction *inst)
{
    Operand op = inst->opSecond;
    switch (inst->type)
    {
    case INST_TYPE: // Only allows type
        if (inst->opSecond.type == OP_TYPE)
            printOperand(&op);
        else
            loginfo("Invalid instruction %s. Expected type as 2nd operand, got %d\n", getInstructionKeyword(inst), inst->opSecond.type);
        break;

    default: // Every other instruction allows only symbol
        if (isSymbolOperand(&inst->opSecond))
            printOperand(&op);
        else
            loginfo("Invalid instruction %s. Expected symbol as 2nd operand, got %d\n", getInstructionKeyword(inst), inst->opSecond.type);
        break;
    }
}

void printThirdOperand(Instruction *inst)
{
    Operand op = inst->opThird;

    // Only symbol is allowed
    if (isSymbolOperand(&inst->opThird))
        printOperand(&op);
    else
        loginfo("Invalid instruction %s. Expected symbol as 3rd operand, got %d\n", getInstructionKeyword(inst), inst->opThird.type);
}

void printInstruction(Instruction *inst)
{
    if (inst == NULL)
    {
        printf("# printInstruction: Received a NULL pointer instead of an instruction\n");
        return;
    }

    printInstructionKeyword(inst);

    if (hasNoOperands(inst))
    {
    }
    else if (hasOneOperand(inst))
    {
        printFirstOperand(inst);
    }
    else if (hasTwoOperands(inst))
    {
        printFirstOperand(inst);
        printSecondOperand(inst);
    }
    else if (hasThreeOperands(inst))
    {
        printFirstOperand(inst);
        printSecondOperand(inst);
        printThirdOperand(inst);
    }
    printf("\n");
}