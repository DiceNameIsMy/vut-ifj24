//
// Created by nur on 16.11.24.
//

#include <string.h>
#include <assert.h>

#include "logging.h"
#include "structs/ast.h"

#include "target_gen/instructions.h"
#include "target_gen/target_gen.h"

FILE *target_outputStream;
SymTable *symbolTable;

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

void generateFunctions(ASTNode *node);
void generateStatements(ASTNode *node);
void generateAssignment(ASTNode *node);

/// @brief Expression is converted to a sequence of
///        instructions that lead to outVar value to be computed.
void generateExpression(ASTNode *node, Operand *outVar);
void generateBinaryExpression(ASTNode *node, Operand *outVar);

void generateConditionalBlock(ASTNode *node);
void generateBuiltInFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCall(ASTNode *node);
void generateFunctionCallParameters(ASTNode *node);

InstType binaryNodeToInstructionType(ASTNode *node);
bool negateBinaryInstruction(ASTNode *node);
Operand initConstantOperand(ASTNode *node);
bool isVarOrConstant(ASTNode *node);

/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/

// TODO: Proper error handling

// TODO: For each symbol (function id, variable), in a symtable assign an ID to it.
//       Use this ID when generating code.

// TODO: When temporary variables are needed, generate a unique name for them in a format
//       "tmp_<scope_id>_<id>", where id is a unique number for each temporary variable.

// TODO: For ALL variables in a function, define them at the beginning of the function

// TODO: When generating code, we might want to insert instructions between other instructions.
//   if that will be the case, instead of just outputting on the fly, we might need to store the
//   instructions in a list first, and output at the end.

/// @brief
/// @param root It's assumed that AST has a correct structure.
///             If an unexpected AST is passed, the function might exit the program.
/// @return -1 if parameters are invalid, 0 otherwise.
///         exit() function is called if target generation fails for other reasons.
int generateTargetCode(ASTNode *root, SymTable *symTable, FILE *output)
{
  if (output == NULL)
  {
    return -1;
  }
  if (symTable == NULL)
  {
    return -1;
  }
  target_outputStream = output;
  symbolTable = symTable;

  // Every .ifjcode program should start with this
  fprintf(target_outputStream, ".IFJcode24\n");

  // Call main function
  Operand mainFunc = initStringOperand(OP_LABEL, "TODO:main");
  Instruction callMainInst = initInstr1(INST_CALL, mainFunc);
  printInstruction(&callMainInst, target_outputStream);

  // Jump to the end of the generated file
  Operand endProgramLabel = initStringOperand(OP_LABEL, "TODO:EndProgramLabel");
  Instruction jumpToEndInst = initInstr1(INST_JUMP, endProgramLabel);
  printInstruction(&jumpToEndInst, target_outputStream);

  // Generate functions
  generateFunctions(root->right);

  // Add label to the end of the program. After main function is done,
  // the program will jump to this label to end the program.
  Operand var = initStringOperand(OP_CONST_STRING, "TODO:EndProgramLabel");
  Instruction inst = initInstr1(INST_LABEL, var);
  printInstruction(&inst, target_outputStream);

  return 0;
}

/**********************************************************/
/* Private Functions Definitions */
/**********************************************************/

void generateFunctions(ASTNode *node)
{
  // Add label for function name
  Operand var = initStringOperand(OP_LABEL, node->value.string);
  Instruction inst = initInstr1(INST_LABEL, var);
  printInstruction(&inst, target_outputStream);

  // Define all local variables
  // TODO:
  // - Enter a scope related to this block
  // - Get all variables in this scope using a symtable

  // Add function body
  generateStatements(node->right);

  // Generate every other function
  if (node->next != NULL)
  {
    return generateFunctions(node->next);
  }
}

void generateStatements(ASTNode *node)
{
  while (node != NULL)
  {
    switch (node->nodeType)
    {
    case VarDeclaration:
    case ConstDeclaration:
      // Skip, as every declaration in this scope has already
      // been made at the beginning of the function
      break;
    case Assignment:
      generateAssignment(node);
      break;
    case BlockStatement:
      // TODO: Generate DEFVAR for variables in this block
      generateStatements(node->left);
      break;
    case IfStatement:
      loginfo("If statement not implemented yet");
      exit(99);
    case WhileStatement:
      loginfo("While statement not implemented yet");
      exit(99);
    case ReturnStatement:
      loginfo("Return statement not implemented yet");
      exit(99);
    default:
      loginfo("Unexpected statement type: %s", nodeTypeToString(node->nodeType));
      exit(99);
    }
  }

  // Generate every consecutive statement
  if (node->next != NULL)
  {
    generateStatements(node->next);
  }
}

void generateAssignment(ASTNode *node)
{
  assert(node->nodeType == Assignment);
  assert(node->left->nodeType == Identifier);

  Operand dest;
  if (strcmp(node->left->value.string, "_") == 0)
  {
    // TODO: assign to a really temporary varaible
    dest = initVarOperand(OP_VAR, FRAME_LF, "TODO:TemporaryVariable");
  }
  else
  {
    dest = initVarOperand(OP_VAR, FRAME_LF, node->left->value.string);
  }

  // Generate assigment evaluation
  generateExpression(node->right, &dest);
}

void generateExpression(ASTNode *node, Operand *outVar)
{
  Instruction inst;

  switch (node->nodeType)
  {
  case AddOperation:
  case SubOperation:
  case MulOperation:
  case DivOperation:
  case LessEqOperation:
  case LessOperation:
  case EqualOperation:
  case NotEqualOperation:
  case GreaterEqOperation:
  case GreaterOperation:
    generateBinaryExpression(node, outVar);
    break;

  case FuncCall:
    generateFunctionCall(node);
    assert(node->right->nodeType == ReturnType);

    bool returnsVoid = strcmp(node->right->value.string, "void") == 0;
    if (!returnsVoid)
    {
      inst = initInstr1(INST_POPS, *outVar);
      printInstruction(&inst, target_outputStream);
    }
    break;

  case BuiltInFunctionCall:
    generateBuiltInFunctionCall(node, outVar);
    break;

  case Identifier:
    inst = initInstr2(
        INST_MOVE,
        *outVar,
        initVarOperand(OP_VAR, FRAME_LF, node->value.string));
    printInstruction(&inst, target_outputStream);
    break;

  case IntLiteral:
    inst = initInstr2(
        INST_MOVE,
        *outVar,
        initOperand(OP_CONST_INT64, (OperandAttribute){.i64 = node->value.integer}));
    printInstruction(&inst, target_outputStream);
    break;

  case FloatLiteral:
    inst = initInstr2(
        INST_MOVE,
        *outVar,
        initOperand(OP_CONST_FLOAT64, (OperandAttribute){.f64 = node->value.real}));
    printInstruction(&inst, target_outputStream);
    break;

  case StringLiteral:
    inst = initInstr2(
        INST_MOVE,
        *outVar,
        initStringOperand(OP_CONST_STRING, node->value.string));
    printInstruction(&inst, target_outputStream);
    break;

  case NullLiteral:
    inst = initInstr2(
        INST_MOVE,
        *outVar,
        initOperand(OP_CONST_NIL, (OperandAttribute){}));
    printInstruction(&inst, target_outputStream);
    break;

  default:
    loginfo("Unexpected factor type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }
}

void generateBinaryExpression(ASTNode *node, Operand *outVar)
{
  InstType instType = binaryNodeToInstructionType(node);
  bool negate = negateBinaryInstruction(node);

  bool inlineLeft = isVarOrConstant(node->left);
  bool inlineRight = isVarOrConstant(node->right);

  Instruction inst;
  if (inlineLeft && inlineRight)
  {
    // outVar = leftInline <op> rightInline
    inst = initInstr3(
        instType,
        *outVar,
        initVarOperand(OP_VAR, FRAME_LF, node->left->value.string),
        initVarOperand(OP_VAR, FRAME_LF, node->right->value.string));
  }
  else if (inlineLeft && !inlineRight)
  {
    // outVar = evaluate(right)
    generateExpression(node->right, outVar);

    // outVar = leftInline <op> outVar
    Operand inlineLeftOperand = initConstantOperand(node->left);
    inst = initInstr3(
        instType,
        *outVar,
        inlineLeftOperand,
        *outVar);
  }
  else if (!inlineLeft && inlineRight)
  {
    // outVar = evaluate(left)
    generateExpression(node->left, outVar);

    // outVar = outVar <op> rightInline
    Operand inlineRightOperand = initConstantOperand(node->right);
    inst = initInstr3(
        instType,
        *outVar,
        *outVar,
        inlineRightOperand);
  }
  else
  {
    // outVar = evaluate(left)
    generateExpression(node->left, outVar);

    // TODO: Add this variable to the DEFVAR list
    Operand defVar = initVarOperand(OP_VAR, FRAME_LF, "TODO:ExtraVariable");

    // defVar = evaluate(right)
    generateExpression(node->right, &defVar);

    // outVar = outVar <op> defVar
    inst = initInstr3(
        instType,
        *outVar,
        *outVar,
        defVar);
  }

  printInstruction(&inst, target_outputStream);
}

InstType binaryNodeToInstructionType(ASTNode *node)
{
  switch (node->nodeType)
  {
  case AddOperation:
    return INST_ADD;
  case SubOperation:
    return INST_SUB;
  case MulOperation:
    return INST_MUL;
  case DivOperation:
    if (node->valType == I32 || node->valType == I32_NULLABLE)
    {
      return INST_IDIV;
    }
    else if (node->valType == F64 || node->valType == F64_NULLABLE)
    {
      return INST_DIV;
    }
    loginfo("Unexpected division of type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  case LessOperation:
    return INST_LT;
  case EqualOperation:
    return INST_EQ;
  case GreaterOperation:
    return INST_GT;
  case NotEqualOperation:
    // For NotEqualOperation operation there is no 1to1 mapping to target.
    // There is though 1to2:
    //  - EQ t <a> <b>
    //  - NOT t t
    return INST_EQ;
  case LessEqOperation:
    return INST_GT;
  case GreaterEqOperation:
    return INST_LT;
  default:
    loginfo("Unexpected binary operation type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }
}

bool negateBinaryInstruction(ASTNode *node)
{
  switch (node->nodeType)
  {
  case AddOperation:
  case SubOperation:
  case MulOperation:
  case DivOperation:
  case LessOperation:
  case EqualOperation:
    return false;
  case NotEqualOperation:
  case GreaterOperation:
  case LessEqOperation:
  case GreaterEqOperation:
    return true;
  default:
    loginfo("Unexpected binary instruction type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }
}

Operand initConstantOperand(ASTNode *node)
{
  switch (node->nodeType)
  {
  case IntLiteral:
    return initOperand(OP_CONST_INT64, (OperandAttribute){.i64 = node->value.integer});
  case FloatLiteral:
    return initOperand(OP_CONST_FLOAT64, (OperandAttribute){.f64 = node->value.real});
  case StringLiteral:
    return initStringOperand(OP_CONST_STRING, node->value.string);
  case NullLiteral:
    return initOperand(OP_CONST_NIL, (OperandAttribute){});
  default:
    loginfo("Unexpected constant type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }
}

bool isVarOrConstant(ASTNode *node)
{
  return node->nodeType == Identifier || node->nodeType == IntLiteral || node->nodeType == FloatLiteral || node->nodeType == StringLiteral || node->nodeType == NullLiteral;
}

void generateConditionalBlock(ASTNode *node)
{
  // express condition & put it into a local variable
  // jump to label on false

  // Define all local variables
  // TODO:
  // - Enter a scope related to this block
  // - Get all variables in this scope using a symtable

  // run generateStatements

  // label
}

void generateBuiltInFunctionCall(ASTNode *node, Operand *outVar)
{
  assert(node->nodeType == BuiltInFunctionCall);

  if (strcmp(node->value.string, "ifj.readstr") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "string"));
    printInstruction(&readInst, target_outputStream);
  }
  else if (strcmp(node->value.string, "ifj.readi32") == 0)
  {
    // TODO: Implement readi32
  }
  else if (strcmp(node->value.string, "ifj.readf64") == 0)
  {
    // TODO: Implement readf64
  }
  else if (strcmp(node->value.string, "ifj.i2f") == 0)
  {
    // TODO: Implement i2f
  }
  else if (strcmp(node->value.string, "ifj.f2i") == 0)
  {
    // TODO: Implement f2i
  }
  else if (strcmp(node->value.string, "ifj.string") == 0)
  {
    // TODO: Implement string
  }
  else if (strcmp(node->value.string, "ifj.length") == 0)
  {
    // TODO: Implement length
  }
  else if (strcmp(node->value.string, "ifj.concat") == 0)
  {
    // TODO: Implement concat
  }
  else if (strcmp(node->value.string, "ifj.substring") == 0)
  {
    // TODO: Implement substring
  }
  else
  {
    loginfo("Unexpected builtin function call: %s", node->value.string);
    exit(99);
  }
}

void generateFunctionCall(ASTNode *node)
{
  assert(node->nodeType == FuncCall);

  // Create TF for parameters
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  printInstruction(&createFrameInst, target_outputStream);

  // Add parameters
  generateFunctionCallParameters(node->left);

  // Push frame
  Instruction pushFrameInst = initInstr0(INST_PUSHFRAME);
  printInstruction(&pushFrameInst, target_outputStream);

  // Call function
  Instruction callInst = initInstr1(INST_CALL, initStringOperand(OP_CONST_STRING, "TODO:FunctionName"));
  printInstruction(&callInst, target_outputStream);

  // Pop frame
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  printInstruction(&popFrameInst, target_outputStream);
}

void generateFunctionCallParameters(ASTNode *node)
{
  // TODO: Some loop to go through all parameters, define and assign them

  Operand param = initVarOperand(OP_VAR, FRAME_TF, "called_func_param_name");

  Operand var;
  bool param_is_variable = true;
  if (param_is_variable)
  {
    var = initVarOperand(OP_VAR, FRAME_LF, "called_func_param_name");
  }
  else
  {
    // TODO: Get a value depending on the type of the parameter
    var = initOperand(OP_CONST_NIL, (OperandAttribute){});
  }

  Instruction instr = initInstr2(INST_MOVE, param, var);
  printInstruction(&instr, target_outputStream);
}
