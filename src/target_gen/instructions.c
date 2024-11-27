#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "logging.h"
#include "target_gen/instructions.h"

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

int copyOperand(Operand *dest, Operand *src);

int initInstruction(Instruction *inst, InstType type,
                    Operand opFirst, Operand opSecond, Operand opThird);

const char *getInstructionKeyword(InstType type);
const char *getOperandTypeName(OperandType type);

void printInstructionKeyword(Instruction *inst, FILE *stream);

bool hasNoOperands(InstType type);
bool hasOneOperand(InstType type);
bool hasTwoOperands(InstType type);
bool hasThreeOperands(InstType type);

bool isFirstOperandValid(InstType type, Operand op);
bool isSecondOperandValid(InstType type, Operand op);
bool isThirdOperandValid(InstType type, Operand op);

bool isSymbolOperand(Operand *op);

void printVar(Operand *op, FILE *stream);
void printConst(Operand *op, FILE *stream);
void printOperand(Operand *op, FILE *stream);


/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/

OperandAttribute initOperandVarAttr(VarFrameType frame, char *name)
{
    if (name == NULL)
    {
        exit(99);
    }

    OperandAttribute attr;
    attr.var.frame = frame;
    attr.var.name = strdup(name);
    if (attr.var.name == NULL)
    {
        loginfo("Failed to allocate memory for variable name");
        exit(99);
    }

    return attr;
}


OperandAttribute initOperandStringAttr(char *string)
{
    if (string == NULL)
    {
        exit(99);
    }

    OperandAttribute attr;
    attr.string = strdup(string);
    if (attr.string == NULL)
    {
        loginfo("Failed to allocate memory for string");
        exit(99);
    }

    return attr;
}

Operand initOperand(OperandType type, OperandAttribute attr)
{
    Operand op;
    op.type = type;
    op.attr = attr;

    return op;
}

Operand initVarOperand(OperandType type, VarFrameType frame, char *name)
{
    return initOperand(type, initOperandVarAttr(frame, name));
}
Operand initStringOperand(OperandType type, char *string)
{
    return initOperand(type, initOperandStringAttr(string));
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

Instruction initInstr0(InstType type)
{
    if (!hasNoOperands(type))
    {
        loginfo("Instruction %s does not support 0 operands", getInstructionKeyword(type));
        exit(99);
    }
    Instruction inst;
    if (initInstruction(&inst, type, initEmptyOperand(), initEmptyOperand(), initEmptyOperand()) == -1)
    {
        exit(99);
    }
    return inst;
}

Instruction initInstr1(InstType type, Operand opFirst)
{
    if (!hasOneOperand(type))
    {
        loginfo("Instruction %s does not support 1 operand", getInstructionKeyword(type));
        exit(99);
    }
    if (!isFirstOperandValid(type, opFirst))
    {
        loginfo("Invalid 1st operand for instruction %s of type %s", getInstructionKeyword(type), getOperandTypeName(opFirst.type));
        exit(99);
    }
    Instruction inst;
    if (initInstruction(&inst, type, opFirst, initEmptyOperand(), initEmptyOperand()) == -1)
    {
        exit(99);
    }
    return inst;
}

Instruction initInstr2(InstType type, Operand opFirst, Operand opSecond)
{
    if (!hasTwoOperands(type))
    {
        loginfo("Instruction %s does not support 2 operands", getInstructionKeyword(type));
        exit(99);
    }
    if (!isFirstOperandValid(type, opFirst))
    {
        loginfo("Invalid 1st operand for instruction %s", getInstructionKeyword(type));
        exit(99);
    }
    if (!isSecondOperandValid(type, opSecond))
    {
        loginfo("Invalid 2nd operand for instruction %s", getInstructionKeyword(type));
        exit(99);
    }
    Instruction inst;
    if (initInstruction(&inst, type, opFirst, opSecond, initEmptyOperand()) == -1)
    {
        exit(99);
    }
    return inst;
}

Instruction initInstr3(InstType type, Operand opFirst, Operand opSecond, Operand opThird)
{
    if (!hasThreeOperands(type))
    {
        loginfo("Instruction %s does not support 3 operands", getInstructionKeyword(type));
        exit(99);
    }
    if (!isFirstOperandValid(type, opFirst))
    {
        loginfo("Invalid 1st operand for instruction %s", getInstructionKeyword(type));
        exit(99);
    }
    if (!isSecondOperandValid(type, opSecond))
    {
        loginfo("Invalid 2nd operand for instruction %s", getInstructionKeyword(type));
        exit(99);
    }
    if (!isThirdOperandValid(type, opThird))
    {
        loginfo("Invalid 3rd operand for instruction %s", getInstructionKeyword(type));
        exit(99);
    }
    Instruction inst;
    if (initInstruction(&inst, type, opFirst, opSecond, opThird) == -1)
    {
        exit(99);
    }
    return inst;
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

    printInstructionKeyword(inst, stream);
    printOperand(&inst->opFirst, stream);
    printOperand(&inst->opSecond, stream);
    printOperand(&inst->opThird, stream);
    fprintf(stream, "\n");
}

/**********************************************************/
/* Function Definitions */
/**********************************************************/

int copyOperand(Operand *dest, Operand *src)
{
    if (dest == NULL || src == NULL)
    {
        loginfo("Operand pointers are NULL");
        return -1;
    }

    dest->type = src->type;
    if (src->type == OP_VAR)
    {
        dest->attr.var.frame = src->attr.var.frame;
        dest->attr.var.name = strdup(src->attr.var.name);
        if (dest->attr.var.name == NULL)
        {
            loginfo("Failed to allocate memory for variable name");
            return -1;
        }
    }
    else if (src->type == OP_CONST_STRING || src->type == OP_LABEL || src->type == OP_TYPE)
    {
        dest->attr.string = strdup(src->attr.string);
        if (dest->attr.string == NULL)
        {
            loginfo("Failed to allocate memory for string");
            return -1;
        }
    } else {
        dest->attr = src->attr;
    }

    return 0;
}

int initInstruction(Instruction *inst, InstType type,
                    Operand opFirst, Operand opSecond, Operand opThird)
{
    if (inst == NULL)
    {
        loginfo("Instruction pointer is NULL");
        return -1;
    }

    inst->type = type;
    copyOperand(&inst->opFirst, &opFirst);
    copyOperand(&inst->opSecond, &opSecond);
    copyOperand(&inst->opThird, &opThird);

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

const char *getOperandTypeName(OperandType type)
{
    switch (type)
    {
    case OP_VAR:
        return "Variable";
    case OP_CONST_BOOL:
        return "Boolean";
    case OP_CONST_INT64:
        return "Integer";
    case OP_CONST_FLOAT64:
        return "Float";
    case OP_CONST_STRING:
        return "String";
    case OP_CONST_NIL:
        return "Nil";
    case OP_LABEL:
        return "Label";
    case OP_TYPE:
        return "Type";
    case OP_NONE:
        return "None";
    default:
        return "Unknown";
    }
}

void printInstructionKeyword(Instruction *inst, FILE *stream)
{
    fprintf(stream, "%s", getInstructionKeyword(inst->type));
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
    case INST_NOT:
    case INST_NOTS:
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
    case INST_ANDS:
    case INST_ORS:
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

bool isFirstOperandValid(InstType type, Operand op)
{
    switch (type)
    {
    case INST_PUSHS: // Only allows symbol
    case INST_WRITE:
    case INST_EXIT:
        if (isSymbolOperand(&op))
            return true;
        loginfo("Invalid instruction %s. Expected symbol as 1st operand, got %d", getInstructionKeyword(type), op.type);
        break;

    case INST_CALL: // Only allows label
    case INST_LABEL:
    case INST_JUMP:
    case INST_JUMPIFEQ:
    case INST_JUMPIFNEQ:
    case INST_JUMPIFEQS:
    case INST_JUMPIFNEQS:
        if (op.type == OP_LABEL)
            return true;
        loginfo("Invalid instruction %s. Expected label as 1st operand, got %d", getInstructionKeyword(type), op.type);
        break;

    default: // Every other instruction only allows var
        if (op.type == OP_VAR)
            return true;
        loginfo("Invalid instruction %s. Expected var as 1st operand, got %d", getInstructionKeyword(type), op.type);
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
            loginfo("Invalid instruction %s. Expected type as 2nd operand, got %d", getInstructionKeyword(type), op.type);
        break;

    default: // Every other instruction allows only symbol
        if (isSymbolOperand(&op))
            return true;
        else
            loginfo("Invalid instruction %s. Expected symbol as 2nd operand, got %d", getInstructionKeyword(type), op.type);
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
        loginfo("Invalid instruction %s. Expected symbol as 3rd operand, got %d", getInstructionKeyword(type), op.type);

    return false;
}

bool isSymbolOperand(Operand *op)
{
    return op->type == OP_VAR || op->type == OP_CONST_BOOL || op->type == OP_CONST_INT64 || op->type == OP_CONST_FLOAT64 || op->type == OP_CONST_STRING || op->type == OP_CONST_NIL;
}

void printOperand(Operand *op, FILE *stream)
{
    switch (op->type)
    {
    case OP_VAR:
        printVar(op, stream);
        break;
    case OP_CONST_BOOL:
    case OP_CONST_INT64:
    case OP_CONST_FLOAT64:
    case OP_CONST_STRING:
    case OP_CONST_NIL:
        printConst(op, stream);
        break;
    case OP_LABEL:
        fprintf(stream, " %s", op->attr.string);
        break;
    case OP_TYPE:
        fprintf(stream, " type@%s", op->attr.string);
        break;
    case OP_NONE:
        break;
    default:
        loginfo("Invalid operand type %d", op->type);
    }
}

void printVar(Operand *op, FILE *stream)
{
    char *frame = NULL;
    if (op->attr.var.frame == FRAME_GF)
        frame = "GF";
    else if (op->attr.var.frame == FRAME_LF)
        frame = "LF";
    else if (op->attr.var.frame == FRAME_TF)
        frame = "TF";
    else
        loginfo("Invalid frame type %d", op->attr.var.frame);

    fprintf(stream, " %s@%s", frame, op->attr.var.name);
}

void printConst(Operand *op, FILE *stream)
{
    switch (op->type)
    {
    case OP_CONST_BOOL:
        fprintf(stream, " bool@%s", op->attr.boolean ? "true" : "false");
        break;
    case OP_CONST_INT64:
        // PRId64 is used to ensure that it would be correctly printed
        fprintf(stream, " int@%" PRId64, op->attr.i64);
        break;
    case OP_CONST_FLOAT64:
        fprintf(stream, " float@0x%.14gp+0", op->attr.f64);
        break;
    case OP_CONST_STRING:
        fprintf(stream, " string@%s", op->attr.string);
        break;
    case OP_CONST_NIL:
        fprintf(stream, " nil@nil");
        break;
    default:
        fprintf(stream, " [ERROR: Invalid constant type %d]", op->type);
        loginfo("Invalid constant type %d", op->type);
    }
}
