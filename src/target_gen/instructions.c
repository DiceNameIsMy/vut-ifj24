#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "logging.h"
#include "target_gen/instructions.h"

FILE *outputStream;

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

int initInstruction(Instruction *inst, InstType type,
                    Operand opFirst, Operand opSecond, Operand opThird);

const char *getInstructionKeyword(InstType type);

void printInstructionKeyword(Instruction *inst);

bool hasNoOperands(InstType type);
bool hasOneOperand(InstType type);
bool hasTwoOperands(InstType type);
bool hasThreeOperands(InstType type);

bool isFirstOperandValid(InstType type, Operand op);
bool isSecondOperandValid(InstType type, Operand op);
bool isThirdOperandValid(InstType type, Operand op);

bool isSymbolOperand(Operand *op);

void printVar(Operand *op);
void printConst(Operand *op);
void printOperand(Operand *op);

/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/

int initVarAttribute(OperandAttribute *attr, VarFrameType frame, char *name)
{
    attr->var.frame = frame;
    attr->var.name = malloc(strlen(name) + 1);
    if (attr->var.name == NULL)
    {
        loginfo("Failed to allocate memory for variable name");
        return -1;
    }

    strcpy(attr->var.name, name);

    return 0;
}

int initStringAttribute(OperandAttribute *attr, char *string)
{
    attr->string = malloc(strlen(string) + 1);
    if (attr->string == NULL)
    {
        return -1;
    }
    strcpy(attr->string, string);

    return 0;
}

void destroyAttribute(OperandAttribute *attr)
{
    if (attr == NULL)
        return;

    if (attr->string != NULL)
    {
        free(attr->string);
        attr->string = NULL;
    }
    else if (attr->var.name != NULL)
    {
        free(attr->var.name);
        attr->var.name = NULL;
    }
}

Operand initOperand(OperandType type, OperandAttribute attr)
{
    Operand op;
    op.type = type;
    op.attr = attr;

    return op;
}

Operand initEmptyOperand()
{
    Operand op;
    op.type = OP_NONE;
    op.attr.boolean = false;

    return op;
}

void destroyOperand(Operand *op)
{
    if (op->type == OP_CONST_STRING || op->type == OP_LABEL || op->type == OP_TYPE)
    {
        free(op->attr.string);
        op->attr.string = NULL;
    }
    else if (op->type == OP_VAR)
    {
        free(op->attr.var.name);
        op->attr.var.name = NULL;
    }
}

int initInstr0(Instruction *inst, InstType type)
{
    if (!hasNoOperands(type))
    {
        loginfo("Instruction %s does not support 0 operands", getInstructionKeyword(type), type);
        return -1;
    }
    return initInstruction(inst, type, initEmptyOperand(), initEmptyOperand(), initEmptyOperand());
}

int initInstr1(Instruction *inst, InstType type, Operand opFirst)
{
    if (!hasOneOperand(type))
    {
        loginfo("Instruction %s does not support 1 operand", getInstructionKeyword(type), type);
        return -1;
    }
    if (!isFirstOperandValid(type, opFirst))
    {
        loginfo("Invalid 1st operand for instruction %s", getInstructionKeyword(type));
        return -1;
    }
    return initInstruction(inst, type, opFirst, initEmptyOperand(), initEmptyOperand());
}

int initInstr2(Instruction *inst, InstType type, Operand opFirst, Operand opSecond)
{
    if (!hasTwoOperands(type))
    {
        loginfo("Instruction %s does not support 2 operands", getInstructionKeyword(type), type);
        return -1;
    }
    if (!isFirstOperandValid(type, opFirst))
    {
        loginfo("Invalid 1st operand for instruction %s", getInstructionKeyword(type));
        return -1;
    }
    if (!isSecondOperandValid(type, opSecond))
    {
        loginfo("Invalid 2nd operand for instruction %s", getInstructionKeyword(type));
        return -1;
    }
    return initInstruction(inst, type, opFirst, opSecond, initEmptyOperand());
}

int initInstr3(Instruction *inst, InstType type, Operand opFirst, Operand opSecond, Operand opThird)
{
    if (!hasThreeOperands(type))
    {
        loginfo("Instruction %s does not support 3 operands", getInstructionKeyword(type), type);
        return -1;
    }
    if (!isFirstOperandValid(type, opFirst))
    {
        loginfo("Invalid 1st operand for instruction %s", getInstructionKeyword(type));
        return -1;
    }
    if (!isSecondOperandValid(type, opSecond))
    {
        loginfo("Invalid 2nd operand for instruction %s", getInstructionKeyword(type));
        return -1;
    }
    if (!isThirdOperandValid(type, opThird))
    {
        loginfo("Invalid 3rd operand for instruction %s", getInstructionKeyword(type));
        return -1;
    }
    return initInstruction(inst, type, opFirst, opSecond, opThird);
}

void destroyInstruction(Instruction *inst)
{
    if (inst == NULL)
        return;

    destroyOperand(&inst->opFirst);
    destroyOperand(&inst->opSecond);
    destroyOperand(&inst->opThird);
}

void printInstruction(Instruction *inst, FILE *stream)
{
    if (inst == NULL)
    {
        return;
    }
    outputStream = stream == NULL ? stdout : stream;

    printInstructionKeyword(inst);

    if (hasNoOperands(inst->type))
    {
    }
    else if (hasOneOperand(inst->type))
    {
        printOperand(&inst->opFirst);
    }
    else if (hasTwoOperands(inst->type))
    {
        printOperand(&inst->opFirst);
        printOperand(&inst->opSecond);
    }
    else if (hasThreeOperands(inst->type))
    {
        printOperand(&inst->opFirst);
        printOperand(&inst->opSecond);
        printOperand(&inst->opThird);
    }
    fprintf(outputStream, "\n");
}

/**********************************************************/
/* Function Definitions */
/**********************************************************/

int initInstruction(Instruction *inst, InstType type,
                    Operand opFirst, Operand opSecond, Operand opThird)
{
    if (inst == NULL)
        return -1;

    inst->type = type;
    inst->opFirst = opFirst;
    inst->opSecond = opSecond;
    inst->opThird = opThird;

    return 0;
}

const char *getInstructionKeyword(InstType type)
{
    switch (type)
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
        return "# [Unknown instruction] #";
    }
}

void printInstructionKeyword(Instruction *inst)
{
    fprintf(outputStream, "%s", getInstructionKeyword(inst->type));
}

bool hasNoOperands(InstType type)
{
    switch (type)
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

bool hasOneOperand(InstType type)
{
    switch (type)
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

bool hasTwoOperands(InstType type)
{
    switch (type)
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

bool hasThreeOperands(InstType type)
{
    switch (type)
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

// TODO Require operations to be on the same type

bool isFirstOperandValid(InstType type, Operand op)
{
    switch (type)
    {
    case INST_PUSHS: // Only allows symbol
    case INST_WRITE:
    case INST_EXIT:
        if (isSymbolOperand(&op))
            return true;
        else
            loginfo("Invalid instruction %s. Expected symbol as 1st operand, got %d\n", getInstructionKeyword(type), op.type);
        break;

    case INST_CALL: // Only allows label
    case INST_LABEL:
    case INST_JUMP:
    case INST_JUMPIFEQS:
    case INST_JUMPIFNEQS:
        if (op.type == OP_LABEL)
            return true;
        else
            loginfo("Invalid instruction %s. Expected label as 1st operand, got %d\n", getInstructionKeyword(type), op.type);
        break;

    default: // Every other instruction only allows var
        if (op.type == OP_VAR)
            return true;
        else
            loginfo("Invalid instruction %s. Expected var as 1st operand, got %d\n", getInstructionKeyword(type), op.type);
        break;
    }
    return false;
}

bool isSecondOperandValid(InstType type, Operand op)
{
    switch (type)
    {
    case INST_TYPE: // Only allows type
        if (op.type == OP_TYPE)
            return true;
        else
            loginfo("Invalid instruction %s. Expected type as 2nd operand, got %d\n", getInstructionKeyword(type), op.type);
        break;

    default: // Every other instruction allows only symbol
        if (isSymbolOperand(&op))
            return true;
        else
            loginfo("Invalid instruction %s. Expected symbol as 2nd operand, got %d\n", getInstructionKeyword(type), op.type);
        break;
    }
    return false;
}

bool isThirdOperandValid(InstType type, Operand op)
{
    // Only symbol is allowed
    if (isSymbolOperand(&op))
        return true;
    else
        loginfo("Invalid instruction %s. Expected symbol as 3rd operand, got %d\n", getInstructionKeyword(type), op.type);

    return false;
}

bool isSymbolOperand(Operand *op)
{
    return op->type == OP_VAR || op->type == OP_CONST_BOOL || op->type == OP_CONST_INT64 || op->type == OP_CONST_FLOAT64 || op->type == OP_CONST_STRING || op->type == OP_CONST_NIL;
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
        fprintf(outputStream, " label@%s", op->attr.string);
        break;
    case OP_TYPE:
        fprintf(outputStream, " type@%s", op->attr.string);
        break;
    case OP_NONE:
        break;
    default:
        loginfo("Invalid operand type %d", op->type);
    }
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

    fprintf(outputStream, " %s@%s", frame, op->attr.var.name);
}

void printConst(Operand *op)
{
    switch (op->type)
    {
    case OP_CONST_BOOL:
        fprintf(outputStream, " bool@%s", op->attr.boolean ? "true" : "false");
        break;
    case OP_CONST_INT64:
        // PRId64 is used to ensure that it would be correctly printed
        fprintf(outputStream, " int@%" PRId64, op->attr.i64);
        break;
    case OP_CONST_FLOAT64:
        fprintf(outputStream, " float@%.14gp+0", op->attr.f64);
        break;
    case OP_CONST_STRING:
        fprintf(outputStream, " string@%s", op->attr.string);
        break;
    case OP_CONST_NIL:
        fprintf(outputStream, " nil@nil");
        break;
    default:
        loginfo(" [Invalid constant type %d]", op->type);
    }
}
