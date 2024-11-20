//
// Created by nur on 16.11.24.
//

#include "structs/ast.h"

#include "target_gen/instructions.h"
#include "target_gen/target_gen.h"

FILE *outputStream;

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

int generateFunctions(ASTNode *node);
int generateStatements(ASTNode *node);
int generateDeclaration(ASTNode *node);
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
int generateTargetCode(ASTNode *root, FILE *output)
{
  if (output == NULL)
  {
    return -1;
  }
  outputStream = output;

  // Every .ifjcode program should start with this
  fprintf(outputStream, ".IFJcode24\n");

  // Call main function
  Operand mainFunc = initOperand(OP_CONST_STRING, initStringAttr("TODO:main"));
  Instruction callMainInst;
  initInstr1(&callMainInst, INST_CALL, mainFunc);
  printInstruction(&callMainInst, outputStream);

  // Jump to the end of the generated file
  Operand endProgramLabel = initOperand(OP_CONST_STRING, initStringAttr("TODO:EndProgramLabel"));
  Instruction jumpToEndInst;
  initInstr1(&jumpToEndInst, INST_JUMP, endProgramLabel);
  printInstruction(&jumpToEndInst, outputStream);

  // Generate functions
  int result = generateFunctions(root->right);
  if (result != 0)
  {
    return result;
  }

  // Add label to the end of the program. After main function is done,
  // the program will jump to this label to end the program.
  Operand var = initOperand(OP_CONST_STRING, initStringAttr("TODO:EndProgramLabel"));
  Instruction inst;
  initInstr1(&inst, INST_LABEL, var);
  printInstruction(&inst, outputStream);

  return 0;
}

/**********************************************************/
/* Function Definitions */
/**********************************************************/

int generateFunctions(ASTNode *node)
{
  int result = 0;

  // Add label for function name
  Operand var = initOperand(OP_CONST_STRING, initStringAttr("TODO:FunctionName"));
  Instruction inst;
  initInstr1(&inst, INST_LABEL, var);
  printInstruction(&inst, outputStream);

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
  // If declaration, run generateDeclaration
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

int generateDeclaration(ASTNode *node)
{
  Operand var = initOperand(OP_VAR, initVarAttr(FRAME_LF, "TODO"));
  Instruction inst;
  initInstr1(&inst, INST_DEFVAR, var);
  printInstruction(&inst, outputStream);

  // TODO: If has value on node->right, run generateAssignment on the same node?
  return 0;
}

int generateAssignment(ASTNode *node)
{
  Operand dest = initOperand(OP_VAR, initStringAttr("TODO"));
  Operand src = initOperand(OP_VAR, initStringAttr("TODO"));
  Instruction inst;
  initInstr2(&inst, INST_MOVE, dest, src);
  printInstruction(&inst, outputStream);

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
  // express condition
  // negative conditional jump to label
  // run generateStatements

  // label
  return 0;
}

int generateFunctionCall(ASTNode *node)
{
  // Create TF for parameters
  Instruction createFrameInst;
  initInstr0(&createFrameInst, INST_CREATEFRAME);
  printInstruction(&createFrameInst, outputStream);

  // Add parameters
  int result = generateFunctionCallParameters(node);
  if (result != 0) {
    return result;
  }

  // Push frame
  Instruction pushFrameInst;
  initInstr0(&pushFrameInst, INST_PUSHFRAME);
  printInstruction(&pushFrameInst, outputStream);

  // Call function
  Instruction callInst;
  initInstr1(&callInst, INST_CALL, initOperand(OP_CONST_STRING, initStringAttr("TODO:FunctionName")));
  printInstruction(&callInst, outputStream);

  // Pop frame
  Instruction popFrameInst;
  initInstr0(&popFrameInst, INST_POPFRAME);
  printInstruction(&popFrameInst, outputStream);

  return 0;
}

int generateFunctionCallParameters(ASTNode *node) {
  // TODO: Some loop to go through all parameters, define and assign them
  return 0;
}
