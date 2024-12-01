//
// Created by nur on 16.11.24.
//

#include <string.h>
#include <assert.h>

#include "logging.h"

#include "dynBuffer.h"
#include "ast.h"

#include "id_indexer.h"
#include "instructions.h"
#include "target_gen.h"
#include "target_func_context.h"

FILE *outputStream;

SymTable *symTable = NULL;

TargetFuncContext *funcScope = NULL;
IdIndexer *funcVarsIndexer = NULL;

IdIndexer *labelIndexer = NULL;

const char *ifjStrcmpFunctionBody =
    "DEFVAR LF@b_0000\n"
    "DEFVAR LF@a_0001\n"
    "DEFVAR LF@lena_0002\n"
    "DEFVAR LF@tmp_0003\n"
    "DEFVAR LF@lenb_0004\n"
    "DEFVAR LF@tmp_0005\n"
    "DEFVAR LF@minLen_0006\n"
    "DEFVAR LF@tmp_0007\n"
    "DEFVAR LF@i_0008\n"
    "DEFVAR LF@tmp_0009\n"
    "DEFVAR LF@aChar_0010\n"
    "DEFVAR LF@tmp_0011\n"
    "DEFVAR LF@bChar_0012\n"
    "DEFVAR LF@tmp_0013\n"
    "DEFVAR LF@tmp_0014\n"
    "DEFVAR LF@tmp_0015\n"
    "DEFVAR LF@tmp_0016\n"
    "DEFVAR LF@tmp_0017\n"
    "DEFVAR LF@tmp_0018\n"
    "DEFVAR LF@tmp_0019\n"
    "DEFVAR LF@tmp_0020\n"
    "POPS LF@b_0000\n"
    "POPS LF@a_0001\n"
    "STRLEN LF@tmp_0003 LF@a_0001\n"
    "MOVE LF@lena_0002 LF@tmp_0003\n"
    "STRLEN LF@tmp_0005 LF@b_0000\n"
    "MOVE LF@lenb_0004 LF@tmp_0005\n"
    "MOVE LF@minLen_0006 LF@lena_0002\n"
    "LT LF@tmp_0007 LF@lenb_0004 LF@lena_0002\n"
    "JUMPIFEQ end_if_0005 LF@tmp_0007 bool@false\n"
    "MOVE LF@minLen_0006 LF@lenb_0004\n"
    "LABEL end_if_0005\n"
    "MOVE LF@i_0008 int@0\n"
    "LABEL while_iteration_0006\n"
    "LT LF@tmp_0009 LF@i_0008 LF@minLen_0006\n"
    "JUMPIFEQ end_while_0007 LF@tmp_0009 bool@false\n"
    "STRI2INT LF@tmp_0011 LF@a_0001 LF@i_0008\n"
    "MOVE LF@aChar_0010 LF@tmp_0011\n"
    "STRI2INT LF@tmp_0013 LF@b_0000 LF@i_0008\n"
    "MOVE LF@bChar_0012 LF@tmp_0013\n"
    "LT LF@tmp_0014 LF@aChar_0010 LF@bChar_0012\n"
    "JUMPIFEQ if_true_0009 LF@tmp_0014 bool@true\n"
    "GT LF@tmp_0015 LF@aChar_0010 LF@bChar_0012\n"
    "JUMPIFEQ end_if_0010 LF@tmp_0015 bool@false\n"
    "PUSHS int@1\n"
    "RETURN\n"
    "LABEL end_if_0010\n"
    "JUMP end_if_0008\n"
    "LABEL if_true_0009\n"
    "SUB LF@tmp_0016 int@0 int@1\n"
    "PUSHS LF@tmp_0016\n"
    "RETURN\n"
    "LABEL end_if_0008\n"
    "ADD LF@tmp_0017 LF@i_0008 int@1\n"
    "MOVE LF@i_0008 LF@tmp_0017\n"
    "JUMP while_iteration_0006\n"
    "LABEL end_while_0007\n"
    "LT LF@tmp_0018 LF@lena_0002 LF@lenb_0004\n"
    "JUMPIFEQ if_true_0012 LF@tmp_0018 bool@true\n"
    "GT LF@tmp_0019 LF@lena_0002 LF@lenb_0004\n"
    "JUMPIFEQ end_if_0013 LF@tmp_0019 bool@false\n"
    "PUSHS int@1\n"
    "RETURN\n"
    "LABEL end_if_0013\n"
    "JUMP end_if_0011\n"
    "LABEL if_true_0012\n"
    "SUB LF@tmp_0020 int@0 int@1\n"
    "PUSHS LF@tmp_0020\n"
    "RETURN\n"
    "LABEL end_if_0011\n"
    "PUSHS int@0\n"
    "RETURN\n";

const char *ifjSubstringFunctionBody =
    "DEFVAR LF@j_0000\n"
    "DEFVAR LF@i_0001\n"
    "DEFVAR LF@s_0002\n"
    "DEFVAR LF@len_0003\n"
    "DEFVAR LF@tmp_0004\n"
    "DEFVAR LF@tmp_0005\n"
    "DEFVAR LF@tmp_0006\n"
    "DEFVAR LF@tmp_0007\n"
    "DEFVAR LF@tmp_0008\n"
    "DEFVAR LF@tmp_0009\n"
    "DEFVAR LF@ssub_0010\n"
    "DEFVAR LF@tmp_0011\n"
    "DEFVAR LF@idx_0012\n"
    "DEFVAR LF@tmp_0013\n"
    "DEFVAR LF@charStr_0014\n"
    "DEFVAR LF@tmp_0015\n"
    "DEFVAR LF@tmp_0016\n"
    "DEFVAR LF@tmp_0017\n"
    "DEFVAR LF@tmp_0018\n"
    "POPS LF@j_0000\n"
    "POPS LF@i_0001\n"
    "POPS LF@s_0002\n"
    "STRLEN LF@tmp_0004 LF@s_0002\n"
    "MOVE LF@len_0003 LF@tmp_0004\n"
    "LT LF@tmp_0005 LF@i_0001 int@0\n"
    "JUMPIFEQ end_if_0014 LF@tmp_0005 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_0014\n"
    "LT LF@tmp_0006 LF@j_0000 int@0\n"
    "JUMPIFEQ end_if_0015 LF@tmp_0006 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_0015\n"
    "GT LF@tmp_0007 LF@i_0001 LF@j_0000\n"
    "JUMPIFEQ end_if_0016 LF@tmp_0007 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_0016\n"
    "LT LF@tmp_0008 LF@i_0001 LF@len_0003\n"
    "NOT LF@tmp_0008 LF@tmp_0008\n"
    "JUMPIFEQ end_if_0017 LF@tmp_0008 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_0017\n"
    "LT LF@tmp_0009 LF@j_0000 LF@len_0003\n"
    "NOT LF@tmp_0009 LF@tmp_0009\n"
    "JUMPIFEQ end_if_0018 LF@tmp_0009 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_0018\n"
    "MOVE LF@tmp_0011 string@\n"
    "MOVE LF@ssub_0010 LF@tmp_0011\n"
    "MOVE LF@idx_0012 LF@i_0001\n"
    "LABEL while_iteration_0019\n"
    "LT LF@tmp_0013 LF@idx_0012 LF@j_0000\n"
    "JUMPIFEQ end_while_0020 LF@tmp_0013 bool@false\n"
    "STRI2INT LF@tmp_0016 LF@s_0002 LF@idx_0012\n"
    "INT2CHAR LF@tmp_0015 LF@tmp_0016\n"
    "MOVE LF@charStr_0014 LF@tmp_0015\n"
    "CONCAT LF@tmp_0017 LF@ssub_0010 LF@charStr_0014\n"
    "MOVE LF@ssub_0010 LF@tmp_0017\n"
    "ADD LF@tmp_0018 LF@idx_0012 int@1\n"
    "MOVE LF@idx_0012 LF@tmp_0018\n"
    "JUMP while_iteration_0019\n"
    "LABEL end_while_0020\n"
    "PUSHS LF@ssub_0010\n"
    "RETURN\n";

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

/// @brief If there is scope defined, add it there. Otherwise, print it to the output stream.
void addInstruction(Instruction inst);

Operand getOrCreateLabel(char *name);
Operand createTmpLabel(char *name);
Operand getOrCreateVar(char *name, VarFrameType frame);
Operand createTmpVar(char *name, VarFrameType frame);

void generateFunction(ASTNode *node);
void generateFunctionParametersInitialization(Param *param);
void generateIfjStrcmpFunction();

/// @brief
/// @param node
/// @return Returns a pointer to a next node to process.
ASTNode *generateStatement(ASTNode *node);
void generateDeclaration(ASTNode *node);
void generateAssignment(ASTNode *node);

/// @brief Expression is converted to a sequence of
///        instructions that lead to outVar value to be computed.
void generateExpression(ASTNode *node, Operand *outVar);
void generateBinaryExpression(ASTNode *node, Operand *outVar);

void generateNullBindingCheck(ASTNode *node, Operand *outVar);
void generateNullBindingAssignment(ASTNode *node);

ASTNode *generateIfCondition(ASTNode *node);
ASTNode *unrollIfConditions(ASTNode *node, Operand endLabel, bool firstEvaluation);
ASTNode *unrollLastIfConditional(ASTNode *node, Operand endLabel, bool firstEvaluation);

void generateWhileConditional(ASTNode *node);
void generateSimpleBuiltInFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCallParameter(ASTNode *node);
void generateReturn(ASTNode *node);

Operand initConstantOperand(ASTNode *node);

InstType binaryNodeToInstructionType(ASTNode *node);
bool negateBinaryInstruction(ASTNode *node);

bool isConstant(ASTNode *node);

char *convertToCompatibleStringLiteral(char *stringLiteral);

/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/

// TODO: Proper error handling

/// @brief
/// @param root It's assumed that AST has a correct structure.
///             If an unexpected AST is passed, the function might exit the program.
/// @return -1 if parameters are invalid, 0 otherwise.
///         exit() function is called if target generation fails for other reasons.
void generateTargetCode(ASTNode *root, SymTable *symbolTable, FILE *output)
{
  loginfo("Generating target code");

  if (root == NULL || symbolTable == NULL || output == NULL)
  {
    loginfo("Invalid parameters passed to generateTargetCode");
    exit(99);
  }

  outputStream = output;
  symTable = symbolTable;

  // Initialize label indexer
  labelIndexer = malloc(sizeof(IdIndexer));
  if (labelIndexer == NULL)
  {
    loginfo("Failed to allocate memory for label context");
    exit(99);
  }
  IdIndexer_Init(labelIndexer);

  // Every .ifjcode program should start with this
  fprintf(outputStream, ".IFJcode24\n");

  // Create an initial frame for the program
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  addInstruction(createFrameInst);
  Instruction pushFrameInst = initInstr0(INST_PUSHFRAME);
  addInstruction(pushFrameInst);

  // Call main function
  Operand mainFuncLabel = getOrCreateLabel("main");
  Instruction callMainInst = initInstr1(INST_CALL, mainFuncLabel);
  addInstruction(callMainInst);

  // Pop the frame since its no longer needed
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  addInstruction(popFrameInst);

  // Jump to the end of the generated file
  Operand endProgramLabel = getOrCreateLabel("end_program");
  Instruction jumpToEndInst = initInstr1(INST_JUMP, endProgramLabel);
  addInstruction(jumpToEndInst);

  // Generate functions
  ASTNode *funcNode = root->right;
  while (funcNode != NULL)
  {
    generateFunction(funcNode);
    funcNode = funcNode->binding;
  }

  // Generate complex builtin functions if they were used
  char *ifjStrcmpLabel;
  bool created = IdIndexer_GetOrCreate(labelIndexer, "ifj_strcmp", &ifjStrcmpLabel);
  bool generateStrcmpFunction = !created;
  if (generateStrcmpFunction)
  {
    addInstruction(initInstr1(INST_LABEL, initStringOperand(OP_LABEL, ifjStrcmpLabel)));
    fprintf(outputStream, ifjStrcmpFunctionBody);
  }

  char *ifjSubstringLabel;
  created = IdIndexer_GetOrCreate(labelIndexer, "ifj_substring", &ifjSubstringLabel);
  bool generateSubstringFunction = !created;
  if (generateSubstringFunction)
  {
    addInstruction(initInstr1(INST_LABEL, initStringOperand(OP_LABEL, ifjSubstringLabel)));
    fprintf(outputStream, ifjSubstringFunctionBody);
  }

  // Add label to the end of the program. After main function is done,
  // the program will jump to this label to end the program.
  Operand var = initStringOperand(OP_LABEL, endProgramLabel.attr.string);
  Instruction inst = initInstr1(INST_LABEL, var);
  addInstruction(inst);

  IdIndexer_Destroy(labelIndexer);
  free(labelIndexer);
  labelIndexer = NULL;

  loginfo("Target code generation finished");
}

/**********************************************************/
/* Private Functions Definitions */
/**********************************************************/

void addInstruction(Instruction inst)
{
  if (funcScope != NULL)
  {
    TFC_AddInst(funcScope, inst);
  }
  else
  {
    printInstruction(&inst, outputStream);
    destroyInstruction(&inst);
  }
}

Operand getOrCreateLabel(char *name)
{
  char *labelName;
  bool created = IdIndexer_GetOrCreate(labelIndexer, name, &labelName);

  Operand var = initStringOperand(OP_LABEL, labelName);
  return var;
}

Operand createTmpLabel(char *name)
{
  char *labelName = IdIndexer_CreateOneTime(labelIndexer, name);
  Operand var = initStringOperand(OP_LABEL, labelName);
  return var;
}

Operand getOrCreateVar(char *name, VarFrameType frame)
{
  char *varName;
  bool created = IdIndexer_GetOrCreate(funcVarsIndexer, name, &varName);

  Operand var = initVarOperand(OP_VAR, frame, varName);

  assert(funcScope != NULL);
  if (created)
  {
    TFC_AddVar(funcScope, var.attr.var);
  }
  return var;
}

Operand createTmpVar(char *name, VarFrameType frame)
{
  char *varName = IdIndexer_CreateOneTime(funcVarsIndexer, name);
  Operand var = initVarOperand(OP_VAR, frame, varName);

  assert(funcScope != NULL);
  TFC_AddVar(funcScope, var.attr.var);
  return var;
}

void generateFunction(ASTNode *node)
{
  loginfo("Generating function %s", node->value.string);

  // Initialize a function scope
  assert(funcScope == NULL);
  funcScope = malloc(sizeof(TargetFuncContext));
  if (funcScope == NULL)
  {
    loginfo("Failed to allocate memory for function scope");
    exit(99);
  }
  TFC_Init(funcScope);

  // Initialize a function variables indexer
  assert(funcVarsIndexer == NULL);
  funcVarsIndexer = malloc(sizeof(IdIndexer));
  if (funcVarsIndexer == NULL)
  {
    loginfo("Failed to allocate memory for function variables indexer");
    exit(99);
  }
  IdIndexer_Init(funcVarsIndexer);

  // Add label for function name
  Operand funcNameLabel = getOrCreateLabel(node->value.string);
  Instruction inst = initInstr1(INST_LABEL, funcNameLabel);
  TFC_SetFuncLabel(funcScope, funcNameLabel.attr.string);

  // Load function parameters from stack
  Param *param = SymTable_GetParamList(symTable, node->value.string);
  generateFunctionParametersInitialization(param);

  // Generate function body
  ASTNode *statementNode = node->next;
  while (statementNode != NULL)
  {
    statementNode = generateStatement(statementNode);
  }

  loginfo("Generating function return: %s", node->right->value.string);
  inspectAstNode(node->right);
  if (strcmp(node->right->value.string, "void") == 0)
  {
    Instruction returnInst = initInstr0(INST_RETURN);
    addInstruction(returnInst);
  }

  // Print every instruction accumulated for the
  // current scope(function) after statement generation
  loginfo("Generating function body");
  while (!TFC_IsEmpty(funcScope))
  {
    Instruction inst = TFC_PopNext(funcScope);
    printInstruction(&inst, outputStream);
    destroyInstruction(&inst);
  }

  // Remove the function scope
  TFC_Destroy(funcScope);
  free(funcScope);
  funcScope = NULL;

  // Remove the function variables indexer
  IdIndexer_Destroy(funcVarsIndexer);
  free(funcVarsIndexer);
  funcVarsIndexer = NULL;
}

void generateFunctionParametersInitialization(Param *param)
{
  if (param == NULL)
  {
    return;
  }
  generateFunctionParametersInitialization(param->next);

  loginfo("Generating function parameter: %s", param->name);
  // Add variable definition
  Operand var = getOrCreateVar(param->name, FRAME_LF);

  // Load variable from stack
  Instruction inst = initInstr1(INST_POPS, var);
  addInstruction(inst);
}

void generateIfjStrcmpFunction()
{
  fprintf(outputStream, ifjStrcmpFunctionBody);
}

ASTNode *generateStatement(ASTNode *node)
{
  // loginfo("Generating statement: %s", nodeTypeToString(node->nodeType));

  switch (node->nodeType)
  {
  case VarDeclaration:
  case ConstDeclaration:
    generateDeclaration(node);
    break;
  case Assignment:
    generateAssignment(node);
    break;
  case BlockStatement:
    return generateStatement(node->left);
  case ConditionalStatement:
    assert(node->left != NULL);
    if (node->left->nodeType == IfCondition)
    {
      generateIfCondition(node->left);
    }
    else if (node->left->nodeType == WhileCondition)
    {
      generateWhileConditional(node->left);
    }
    else
    {
      loginfo("Unexpected conditional statement type: %s", nodeTypeToString(node->left->nodeType));
      inspectAstNode(node);
      exit(99);
    }
    break;
  case ReturnStatement:
    generateReturn(node);
    break;
  case FuncCall:
    generateFunctionCall(node, NULL);
    break;
  default:
    loginfo("Unexpected statement type: %s", nodeTypeToString(node->nodeType));
    inspectAstNode(node);
    exit(99);
  }
  return node->next;
}

void generateDeclaration(ASTNode *node)
{
  // Get var unique name
  Operand varOperand = getOrCreateVar(node->value.string, FRAME_LF);

  // Make an assignment
  Operand value;
  generateExpression(node->right, &value);
  addInstruction(initInstr2(INST_MOVE, varOperand, value));
}

void generateAssignment(ASTNode *node)
{
  assert(node->nodeType == Assignment);
  loginfo("Generating assignment to: %s", node->value.string);

  // If variable name is _, generate a temporary variable name
  Operand dest;
  if (strcmp(node->value.string, "_") == 0)
  {
    // TODO: A value is assigned but would never be used. Can it be done in some better way?
    dest = createTmpVar("_", FRAME_LF);
  }
  else
  {
    dest = getOrCreateVar(node->value.string, FRAME_LF);
  }

  // Generate assigment evaluation
  Operand src;
  generateExpression(node->left, &src);
  addInstruction(initInstr2(INST_MOVE, dest, src));
}

void generateExpression(ASTNode *node, Operand *outVar)
{
  loginfo("Generating expression: %s", nodeTypeToString(node->nodeType));

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
  case BuiltInFunctionCall: // TODO: Delete this enum value
    generateFunctionCall(node, outVar);
    break;

  case Identifier:
    *outVar = getOrCreateVar(node->value.string, FRAME_LF);
    break;

  case IntLiteral:
  case FloatLiteral:
  case StringLiteral:
  case NullLiteral:
    *outVar = initConstantOperand(node);
    break;

  default:
    loginfo("Unexpected factor type: %s", nodeTypeToString(node->nodeType));
    inspectAstNode(node);
    exit(99);
  }
}

void generateBinaryExpression(ASTNode *node, Operand *outVar)
{
  loginfo("Generating binary expression: %s", nodeTypeToString(node->nodeType));
  inspectAstNode(node);

  bool leftIsVar = node->left->nodeType == Identifier;
  bool leftIsConstant = isConstant(node->left);
  bool leftCanBeInline = leftIsVar || leftIsConstant;

  bool rightIsVar = node->right->nodeType == Identifier;
  bool rightIsConstant = isConstant(node->right);
  bool rightCanBeInline = rightIsVar || rightIsConstant;

  bool outVarInitialized = false;

  Operand leftOperand;
  if (leftIsVar)
  {
    leftOperand = getOrCreateVar(node->left->value.string, FRAME_LF);
    loginfo("Left operand is a variable: %s", node->left->value.string);
  }
  else if (leftIsConstant)
    leftOperand = initConstantOperand(node->left);
  else if (rightCanBeInline)
  {
    // Left operand is not inline but right operand is.
    // Evaluate left at the place of outVar & then use it to set the final value to outVar.
    generateExpression(node->left, outVar);
    leftOperand = *outVar;
    outVarInitialized = true;
  }
  else
  {
    // Both operands can't be inlined. Set left's value to outVar.
    generateExpression(node->left, outVar);
    leftOperand = *outVar;
    outVarInitialized = true;
  }

  Operand rightOperand;
  if (rightIsVar)
    rightOperand = getOrCreateVar(node->right->value.string, FRAME_LF);
  else if (rightIsConstant)
    rightOperand = initConstantOperand(node->right);
  else if (leftCanBeInline)
  {
    // Same as for left, but for right operand.
    generateExpression(node->right, outVar);
    rightOperand = *outVar;
    outVarInitialized = true;
  }
  else
  {
    // Both operands can't be inlined. Create a temporary variable to evaluate right operand.
    rightOperand = createTmpVar("tmp", FRAME_LF);
    generateExpression(node->right, &rightOperand);
  }

  if (!outVarInitialized)
  {
    *outVar = createTmpVar("tmp", FRAME_LF);
  }

  Instruction inst = initInstr3(binaryNodeToInstructionType(node), *outVar, leftOperand, rightOperand);
  addInstruction(inst);

  bool negate = negateBinaryInstruction(node);
  if (negate)
  {
    Instruction negateInst = initInstr2(INST_NOT, *outVar, *outVar);
    addInstruction(negateInst);
  }

  loginfo("Binary expression generated");
}

void generateNullBindingCheck(ASTNode *node, Operand *outVar)
{
  *outVar = createTmpVar("not_null", FRAME_LF);

  Operand isNullOperand = createTmpVar("is_null", FRAME_LF);
  Instruction getTypeInstruction = initInstr2(
      INST_TYPE,
      isNullOperand,
      getOrCreateVar(node->left->value.string, FRAME_LF));
  addInstruction(getTypeInstruction);

  // Put true if the value is not NULL
  Instruction putIsNullCheck = initInstr3(
      INST_EQ,
      *outVar,
      isNullOperand,
      initStringOperand(OP_CONST_STRING, "nil"));
  addInstruction(putIsNullCheck);

  Instruction negateInst = initInstr2(INST_NOT, *outVar, *outVar);
  addInstruction(negateInst);
}

void generateNullBindingAssignment(ASTNode *node)
{
  Operand nullBindingResult = getOrCreateVar(node->binding->value.string, FRAME_LF);
  char *nullableVarName = node->left->value.string;
  addInstruction(
      initInstr2(
          INST_MOVE,
          nullBindingResult,
          getOrCreateVar(nullableVarName, FRAME_LF)));
}

ASTNode *generateIfCondition(ASTNode *node)
{
  loginfo("Generating conditional statement");
  inspectAstNode(node);

  Operand endLabel = createTmpLabel("end_if");

  ASTNode *nextStatementNode = unrollIfConditions(node, endLabel, true);

  addInstruction(initInstr1(INST_LABEL, endLabel));

  return nextStatementNode;
}

/// @brief Processes all nodes of the conditional statement.
/// @param node
/// @param endLabel
/// @param firstEvaluation
/// @return Pointer to a next node to process.
ASTNode *unrollIfConditions(ASTNode *node, Operand endLabel, bool firstEvaluation)
{
  assert(node->nodeType == IfCondition);

  bool isLastEvaluation = node->next == NULL || node->next->nodeType != IfCondition;
  if (isLastEvaluation)
  {
    return unrollLastIfConditional(node, endLabel, firstEvaluation);
  }

  // Evaluate condition
  Operand ifCondition;
  if (node->binding == NULL)
  {
    generateExpression(node->left, &ifCondition);
  }
  else
  {
    generateNullBindingCheck(node, &ifCondition);
  }
  loginfo("Generated condition for if statement");

  // If not last evaluation, JUMP on positive condition
  Instruction jumpToBlockInst = initInstr3(
      INST_JUMPIFEQ,
      endLabel,
      ifCondition,
      initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = true}));
  addInstruction(jumpToBlockInst);

  // Unroll next evaluation.
  ASTNode *nextStatementNode = unrollIfConditions(node->next, endLabel, false);

  // Generate body
  if (node->binding != NULL)
  {
    generateNullBindingAssignment(node);
  }
  ASTNode *ifBlockStatement = node->right;
  while (ifBlockStatement != NULL)
  {
    ifBlockStatement = generateStatement(ifBlockStatement);
  }

  Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
  addInstruction(jumpToEnd);

  return nextStatementNode;
}

ASTNode *unrollLastIfConditional(ASTNode *node, Operand endLabel, bool firstEvaluation)
{
  loginfo("Generating last `if` evaluation");

  // Evaluate condition
  Operand ifCondition;
  if (node->binding == NULL)
  {
    generateExpression(node->left, &ifCondition);
  }
  else
  {
    generateNullBindingCheck(node, &ifCondition);
  }

  bool hasTrailingElse = node->next != NULL && node->next->nodeType != IfCondition;
  if (hasTrailingElse)
  {
    Operand ifTrueLabel = createTmpLabel("if_true");
    loginfo("Final condition has a trailing else block");
    // Jump to the code to execute if the condition is true.
    Instruction jumpToBlockInst = initInstr3(
        INST_JUMPIFEQ,
        ifTrueLabel,
        ifCondition,
        initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = true}));
    addInstruction(jumpToBlockInst);

    // Unroll the else block.
    ASTNode *elseBlockStatement = node->next;
    while (elseBlockStatement != NULL)
    {
      loginfo("Generating else block statement of type %s", nodeTypeToString(elseBlockStatement->nodeType));
      elseBlockStatement = generateStatement(elseBlockStatement);
    }
    // Jump to the end
    Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
    addInstruction(jumpToEnd);

    addInstruction(initInstr1(INST_LABEL, ifTrueLabel));

    // Generate body on true condition
    if (node->binding != NULL)
    {
      generateNullBindingAssignment(node);
    }
    ASTNode *ifBlockStatement = node->right;
    while (ifBlockStatement != NULL)
    {
      ifBlockStatement = generateStatement(ifBlockStatement);
    }
  }
  else
  {
    loginfo("Final condition does not have a trailing else block");
    // Optimization: If it's a last evaluation, jump to end on negative condition. Put the code right after.
    Instruction jumpToEndOnNegativeCondition = initInstr3(
        INST_JUMPIFEQ,
        endLabel,
        ifCondition,
        initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = false}));
    addInstruction(jumpToEndOnNegativeCondition);

    // Generate body
    if (node->binding != NULL)
    {
      generateNullBindingAssignment(node);
    }
    ASTNode *ifBlockStatement = node->right;
    while (ifBlockStatement != NULL)
    {
      ifBlockStatement = generateStatement(ifBlockStatement);
    }
  }

  if (firstEvaluation)
  {
    // Code to execute for the first `if` check are located at the bottom.
    // There is no need to jump to endLabel since the next instruction is the endLabel itself.
  }
  else
  {
    Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
    addInstruction(jumpToEnd);
  }
}

void generateWhileConditional(ASTNode *node)
{
  inspectAstNode(node);

  // Create a label for the beginning of the while loop
  Operand whileIterLabel = createTmpLabel("while_iteration");
  addInstruction(initInstr1(INST_LABEL, whileIterLabel));

  // Define a label name to jump to when the condition is false
  Operand endWhileLabel = createTmpLabel("end_while");

  // Evaluate while (...)
  Operand whileCondition;
  if (node->binding == NULL)
  {
    generateExpression(node->left, &whileCondition);
  }
  else
  {
    generateNullBindingCheck(node, &whileCondition);
  }

  // Jump to the end of the loop if the condition is false
  Instruction instrCondEndLoop = initInstr3(
      INST_JUMPIFEQ,
      endWhileLabel,
      whileCondition,
      initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = false}));
  addInstruction(instrCondEndLoop);

  // Generate a body of while () { ... }
  loginfo("Generating while loop body");
  if (node->binding != NULL)
  {
    generateNullBindingAssignment(node);
  }
  ASTNode *bodyStatement = node->right;
  while (bodyStatement != NULL)
  {
    loginfo("Generating while loop body statement %s", nodeTypeToString(bodyStatement->nodeType));
    inspectAstNode(bodyStatement);
    bodyStatement = generateStatement(bodyStatement);
  }

  // Jump to the beginning of the loop
  addInstruction(initInstr1(INST_JUMP, whileIterLabel));

  // Add label for the end of the while loop
  addInstruction(initInstr1(INST_LABEL, endWhileLabel));

  loginfo("While loop generated");
}

void generateFunctionCall(ASTNode *node, Operand *outVar)
{
  char *functionName = node->value.string;
  if (strcmp(node->value.string, "ifj.strcmp") == 0)
  {
    functionName = "ifj_strcmp";
  }
  else if (strcmp(node->value.string, "ifj.substring") == 0)
  {
    functionName = "ifj_substring";
  }
  else if (strncmp(node->value.string, "ifj.", 4) == 0)
  {
    generateSimpleBuiltInFunctionCall(node, outVar);
    return;
  }
  loginfo("Generating function call: %s", functionName);

  // Create TF for parameters
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  addInstruction(createFrameInst);

  // Add parameters
  inspectAstNode(node);
  ASTNode *paramNode = node->left;
  while (paramNode != NULL)
  {
    generateFunctionCallParameter(paramNode);
    paramNode = paramNode->next;
  }

  // Push frame
  Instruction pushFrameInst = initInstr0(INST_PUSHFRAME);
  addInstruction(pushFrameInst);

  loginfo("Calling function: %s", functionName);

  // Call function
  Instruction callInst = initInstr1(INST_CALL, getOrCreateLabel(functionName));
  addInstruction(callInst);

  // Pop frame
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  addInstruction(popFrameInst);

  // If function returns a value, put it into outVar
  bool returnsVoid = node->valType == NONE;
  if (!returnsVoid)
  {
    *outVar = createTmpVar("tmp", FRAME_LF);
    addInstruction(initInstr1(INST_POPS, *outVar));
  }
}

void generateFunctionCallParameter(ASTNode *node)
{
  loginfo("Generating function call parameter: %s", nodeTypeToString(node->nodeType));
  inspectAstNode(node);

  // Get operand
  Operand paramOperand;
  generateExpression(node, &paramOperand);

  // Push operand to stack
  Instruction pushInst = initInstr1(INST_PUSHS, paramOperand);
  addInstruction(pushInst);
}

void generateSimpleBuiltInFunctionCall(ASTNode *node, Operand *outVar)
{
  loginfo("Generating builtin function call: %s", node->value.string);

  // ifj.write does not return a value. 
  // Check for it early to avoid repeating code for operand creation.
  if (strcmp(node->value.string, "ifj.write") == 0)
  {
    Operand writeOperand;
    generateExpression(node->left, &writeOperand);
    Instruction writeInst = initInstr1(INST_WRITE, writeOperand);
    addInstruction(writeInst);
    return;
  }

  *outVar = createTmpVar("tmp", FRAME_LF);

  if (strcmp(node->value.string, "ifj.readstr") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "string"));
    addInstruction(readInst);
  }
  else if (strcmp(node->value.string, "ifj.readi32") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "int"));
    addInstruction(readInst);
  }
  else if (strcmp(node->value.string, "ifj.readf64") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "float"));
    addInstruction(readInst);
  }
  else if (strcmp(node->value.string, "ifj.i2f") == 0)
  {
    Operand convertOperand;
    generateExpression(node->left, &convertOperand);
    Instruction convertInst = initInstr2(
        INST_INT2FLOAT,
        *outVar,
        convertOperand);
    addInstruction(convertInst);
  }
  else if (strcmp(node->value.string, "ifj.f2i") == 0)
  {
    Operand convertOperand;
    generateExpression(node->left, &convertOperand);
    Instruction convertInst = initInstr2(
        INST_FLOAT2INT,
        *outVar,
        convertOperand);
    addInstruction(convertInst);
  }
  else if (strcmp(node->value.string, "ifj.string") == 0)
  {
    assert(node->left->nodeType == StringLiteral);
    Operand convertOperand = initStringOperand(OP_CONST_STRING, node->left->value.string);
    Instruction toStringInst = initInstr2(
        INST_MOVE,
        *outVar,
        convertOperand);
    addInstruction(toStringInst);
  }
  else if (strcmp(node->value.string, "ifj.length") == 0)
  {
    Operand strlenOperand;
    generateExpression(node->left, &strlenOperand);
    Instruction toStringInst = initInstr2(INST_STRLEN, *outVar, strlenOperand);
    addInstruction(toStringInst);
  }
  else if (strcmp(node->value.string, "ifj.concat") == 0)
  {
    Operand firstOperand;
    generateExpression(node->left, &firstOperand);
    Operand secondOperand;
    generateExpression(node->left->next, &secondOperand);

    Instruction concatInst = initInstr3(INST_CONCAT, *outVar, firstOperand, secondOperand);
    addInstruction(concatInst);
  }
  else if (strcmp(node->value.string, "ifj.ord") == 0)
  {
    // Convert character at given index to int
    Operand u8;
    generateExpression(node->left, &u8);
    Operand idx;
    generateExpression(node->left->next, &idx);
    Instruction strToIntInst = initInstr3(INST_STRI2INT, *outVar, u8, idx);
    addInstruction(strToIntInst);
  }
  else if (strcmp(node->value.string, "ifj.chr") == 0)
  {
    Operand convertOperand;
    generateExpression(node->left, &convertOperand);
    Instruction convertInst = initInstr2(
        INST_INT2CHAR,
        *outVar,
        convertOperand);
    addInstruction(convertInst);
  }
  else
  {
    loginfo("Unexpected builtin function call: %s", node->value.string);
    exit(99);
  }
}

void generateReturn(ASTNode *node)
{
  loginfo("Generating return statement");
  inspectAstNode(node);

  if (node->valType == NONE) // Return void
  {
    Instruction returnInst = initInstr0(INST_RETURN);
    addInstruction(returnInst);
    return;
  }

  ASTNode *returnValueNode = node->left;

  Operand returnOperand;
  generateExpression(returnValueNode, &returnOperand);

  // Push the return value to the stack
  Instruction pushInst = initInstr1(INST_PUSHS, returnOperand);
  addInstruction(pushInst);

  addInstruction(initInstr0(INST_RETURN));
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
    char *literalString = convertToCompatibleStringLiteral(node->value.string);
    return initStringOperand(OP_CONST_STRING, literalString);
  case NullLiteral:
    return initOperand(OP_CONST_NIL, (OperandAttribute){});
  default:
    loginfo("Unexpected constant type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }
}

char *convertToCompatibleStringLiteral(char *stringLiteral)
{
  DynBuffer buffer;
  if (initDynBuffer(&buffer, -1) != 0)
  {
    loginfo("Failed to initialize dynamic buffer");
    exit(99);
  }

  for (int i = 0; i < strlen(stringLiteral); i++)
  {
    char c = stringLiteral[i];
    if (c <= 32 || c == '#' || c == '\\')
    {
      // Reformat special characters to \xxx
      appendStringDynBuffer(&buffer, "\\___");
      sprintf(&buffer.data[buffer.nextIdx - 3], "%03d", c);
    }
    else
    {
      appendDynBuffer(&buffer, c);
    }
  }
  char *result;
  if (copyFromDynBuffer(&buffer, &result) != 0)
  {
    loginfo("Failed to copy from dynamic buffer");
    exit(99);
  }
  freeDynBuffer(&buffer);

  return result;
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
  case GreaterOperation:
  case EqualOperation:
    return false;
  case NotEqualOperation:
  case LessEqOperation:
  case GreaterEqOperation:
    // These relational binary instructions do not have a 1 to 1 mapping to target.
    // They can only to acheived by negating the result of their counterpart.
    return true;
  default:
    loginfo("Unexpected binary instruction type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }
}

bool isConstant(ASTNode *node)
{
  return node->nodeType == IntLiteral || node->nodeType == FloatLiteral || node->nodeType == StringLiteral || node->nodeType == NullLiteral;
}
