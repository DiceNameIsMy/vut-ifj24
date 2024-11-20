//
// Created by nur on 16.11.24.
//

#include "structs/ast.h"

#include "target_gen/instructions.h"
#include "target_gen/target_gen.h"

FILE *outputStream;

// TODO: Proper error hanlding

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

  return generateFunctions(root->right);
}

int generateFunctions(ASTNode *node)
{
  int result = 0;

  // generate label

  result = generateBlock(node->right);
  if (result != 0)
  {
    return result;
  }

  if (node->next != NULL)
  {
    return generateFunctions(node->next);
  }

  return 0;
}

int generateBlock(ASTNode *node)
{
  // run generateStatements

  // dispose of variables defined in a block
  return 0;
}

int generateStatements(ASTNode *node)
{
  // If declaration, run generateDeclaration
  // If assignment, run generateAssignment
  // If an if statement, run generateConditionalBlock
  // If a new block, run generateBlock

  // If next statement exists, run generateStatements
  return 0;
}

int generateDeclaration(ASTNode *node)
{
  Operand var = initOperand(OP_VAR, (OperandAttribute){.var.name = "TODO", .var.frame = FRAME_TF});
  Instruction inst;
  initInstr1(&inst, INST_DEFVAR, var);

  printInstruction(&inst, outputStream);

  // TODO: If has value on node->right, run generateAssignment on the same node?
  return 0;
}

int generateAssignment(ASTNode *node)
{
  Operand dest = initOperand(OP_VAR, (OperandAttribute){.string = "TODO"});
  Operand src = initOperand(OP_VAR, (OperandAttribute){.string = "TODO"});
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
  // run generateBlock

  // label
  return 0;
}

int generateFunctionCall(ASTNode *node)
{
  // push arguments to the stack
  // call function
  return 0;
}
