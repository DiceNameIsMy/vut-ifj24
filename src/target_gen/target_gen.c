//
// Created by nur on 16.11.24.
//

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
int generateExpression(ASTNode *node);
int generateConditionalBlock(ASTNode *node);
int generateFunctionCall(ASTNode *node);
int generateFunctionCallParameters(ASTNode *node);

/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/

// TODO: Proper error handling
// TODO: When labels, are defined, check they are unique. If not,
//   add a number to the end, and ensure that they would be used correctly
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
  Operand var = initStringOperand(OP_CONST_STRING, "TODO:FunctionName");
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
  // If declaration, skip, as every declaration in this scope has already been done
  // If assignment, run generateAssignment
  // If an if statement, run generateConditionalBlock
  // If a new block, run generateStatements

  // Generate every consecutive statement
  if (node->next != NULL)
  {
    return generateStatements(node->next);
  }
  return 0;
}

int generateAssignment(ASTNode *node)
{
  Operand dest = initVarOperand(OP_VAR, FRAME_LF, "TODO");
  Operand src = initVarOperand(OP_VAR, FRAME_LF, "TODO");
  Instruction inst = initInstr2(INST_MOVE, dest, src);
  printInstruction(&inst, target_outputStream);

  return 0;
}

int generateExpression(ASTNode *node)
{
  // If binary expression, run generateBinaryExpression
  // If unary expression, run generateUnaryExpression
  // If function call, run generateFunctionCall
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

int generateFunctionCall(ASTNode *node)
{
  // Create TF for parameters
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  printInstruction(&createFrameInst, target_outputStream);

  // Add parameters
  int result = generateFunctionCallParameters(node);
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
