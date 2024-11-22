//
// Created by nur on 16.11.24.
//

#include <string.h>

#include "logging.h"
#include "structs/ast.h"

#include "target_gen/instructions.h"
#include "target_gen/target_gen.h"

FILE *target_outputStream;
SymTable *symbolTable;

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

int generateFunctions(ASTNode *node);
int generateStatements(ASTNode *node);
int generateAssignment(ASTNode *node);

/// @brief Expression is converted to a sequence of
/// instructions that lead to outVar value to be computed.
int generateExpression(ASTNode *node, Operand *outVar);

int generateConditionalBlock(ASTNode *node);
int generateBuiltInFunctionCall(ASTNode *node, Operand *outVar);
int generateFunctionCall(ASTNode *node);
int generateFunctionCallParameters(ASTNode *node);

/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/


// TODO: Proper error handling

// TODO: For each symbol (function id, variable), in a symtable assign an ID to it. 
//       Use this ID when generating code.

// TODO: When temporary variables are needed, generate a unique name for them in a format 
//       "tmp_<scope_id>_<id>", where id is a unique number for each temporary variable.

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
  Operand mainFunc = initStringOperand(OP_CONST_STRING, "TODO:main");
  Instruction callMainInst = initInstr1(INST_CALL, mainFunc);
  printInstruction(&callMainInst, target_outputStream);

  // Jump to the end of the generated file
  Operand endProgramLabel = initStringOperand(OP_CONST_STRING, "TODO:EndProgramLabel");
  Instruction jumpToEndInst = initInstr1(INST_JUMP, endProgramLabel);
  printInstruction(&jumpToEndInst, target_outputStream);

  // Generate functions
  int result = generateFunctions(root->right);
  if (result != 0)
  {
    return result;
  }

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

int generateFunctions(ASTNode *node)
{
  int result = 0;

  // Add label for function name
  Operand var = initStringOperand(OP_LABEL, node->value.string);
  Instruction inst = initInstr1(INST_LABEL, var);
  printInstruction(&inst, target_outputStream);
  
  // Define all local variables
  // TODO:
  // - Enter a scope related to this block
  // - Get all variables in this scope using a symtable

  // Add function body
  result = generateStatements(node->right);
  if (result != 0)
  {
    return result;
  }

  // Generate every other function
  if (node->next != NULL)
  {
    return generateFunctions(node->next);
  }

  // All functions were generated successfully
  return 0;
}

int generateStatements(ASTNode *node)
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
    return generateStatements(node->next);
  }
  return 0;
}

int generateAssignment(ASTNode *node)
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

  return 0;
}

int generateExpression(ASTNode *node, Operand *outVar)
{
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
    // TODO
    loginfo("Binary operations are not implemented yet");
    exit(99);
    break;

  case FuncCall:
    generateFunctionCall(node);
    assert(node->right->nodeType, ReturnType);

    bool returnsVoid = strcmp(node->right->value.string, "void") == 0;
    if (!returnsVoid)
    {
      Instruction inst = initInstr1(INST_POPS, *outVar);
      printInstruction(&inst, target_outputStream);
    }
    break;

  case BuiltInFunctionCall:
    generateBuiltInFunctionCall(node, outVar);
    break;

  case Identifier:
    Instruction inst = initInstr2(
        INST_MOVE,
        *outVar,
        initVarOperand(OP_VAR, FRAME_LF, node->value.string));
    printInstruction(&inst, target_outputStream);
    break;

  case IntLiteral:
    Instruction inst = initInstr2(
        INST_MOVE,
        *outVar,
        initOperand(OP_CONST_INT64, (OperandAttribute){.i64 = 0})); // TODO: Set real value
    printInstruction(&inst, target_outputStream);
    break;

  case FloatLiteral:
    Instruction inst = initInstr2(
        INST_MOVE,
        *outVar,
        initOperand(OP_CONST_FLOAT64, (OperandAttribute){.f64 = 0})); // TODO: Set real value
    printInstruction(&inst, target_outputStream);
    break;

  case StringLiteral:
    Instruction inst = initInstr2(
        INST_MOVE,
        *outVar,
        initStringOperand(OP_CONST_STRING, node->value.string));
    printInstruction(&inst, target_outputStream);
    break;

  case NullLiteral:
    Instruction inst = initInstr2(
        INST_MOVE,
        *outVar,
        initOperand(OP_CONST_NIL, (OperandAttribute){}));
    printInstruction(&inst, target_outputStream);
    break;

  default:
    loginfo("Unexpected factor type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }

  return 0;
}

int generateConditionalBlock(ASTNode *node)
{
  // express condition & put it into a local variable
  // jump to label on false

  // Define all local variables
  // TODO:
  // - Enter a scope related to this block
  // - Get all variables in this scope using a symtable

  // run generateStatements

  // label
  return 0;
}

int generateBuiltInFunctionCall(ASTNode *node, Operand *outVar)
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

  return 0;
}

int generateFunctionCall(ASTNode *node)
{
  assert(node->nodeType == FuncCall);

  // Create TF for parameters
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  printInstruction(&createFrameInst, target_outputStream);

  // Add parameters
  int result = generateFunctionCallParameters(node->left);
  if (result != 0)
  {
    return result;
  }

  // Push frame
  Instruction pushFrameInst = initInstr0(INST_PUSHFRAME);
  printInstruction(&pushFrameInst, target_outputStream);

  // Call function
  Instruction callInst = initInstr1(INST_CALL, initStringOperand(OP_CONST_STRING, "TODO:FunctionName"));
  printInstruction(&callInst, target_outputStream);

  // Pop frame
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  printInstruction(&popFrameInst, target_outputStream);

  // Return value is stored in Stack.
  return 0;
}

int generateFunctionCallParameters(ASTNode *node)
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

  return 0;
}
