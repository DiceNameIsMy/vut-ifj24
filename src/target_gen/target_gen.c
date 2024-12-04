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

/// @brief These are bodies of complex builtin functions.
///        They were generated manually and are added to the
///        resulting code if they are used within the program.
///
///        Indexes collision issue is avoided by extending the pre-generated labels by a leading 0.
///        To do that, `sed -E 's/([0-9]{4})/0\1/g' "generated_function.txt" > "generated_function.out"` command was used.
const char *ifjStrcmpFunctionBody =
    "DEFVAR LF@b_00000\n"
    "DEFVAR LF@a_00001\n"
    "DEFVAR LF@lena_00002\n"
    "DEFVAR LF@tmp_00003\n"
    "DEFVAR LF@lenb_00004\n"
    "DEFVAR LF@tmp_00005\n"
    "DEFVAR LF@minLen_00006\n"
    "DEFVAR LF@tmp_00007\n"
    "DEFVAR LF@i_00008\n"
    "DEFVAR LF@tmp_00009\n"
    "DEFVAR LF@aChar_00010\n"
    "DEFVAR LF@tmp_00011\n"
    "DEFVAR LF@bChar_00012\n"
    "DEFVAR LF@tmp_00013\n"
    "DEFVAR LF@tmp_00014\n"
    "DEFVAR LF@tmp_00015\n"
    "DEFVAR LF@tmp_00016\n"
    "DEFVAR LF@tmp_00017\n"
    "DEFVAR LF@tmp_00018\n"
    "DEFVAR LF@tmp_00019\n"
    "DEFVAR LF@tmp_00020\n"
    "POPS LF@b_00000\n"
    "POPS LF@a_00001\n"
    "STRLEN LF@tmp_00003 LF@a_00001\n"
    "MOVE LF@lena_00002 LF@tmp_00003\n"
    "STRLEN LF@tmp_00005 LF@b_00000\n"
    "MOVE LF@lenb_00004 LF@tmp_00005\n"
    "MOVE LF@minLen_00006 LF@lena_00002\n"
    "LT LF@tmp_00007 LF@lenb_00004 LF@lena_00002\n"
    "JUMPIFEQ end_if_00005 LF@tmp_00007 bool@false\n"
    "MOVE LF@minLen_00006 LF@lenb_00004\n"
    "LABEL end_if_00005\n"
    "MOVE LF@i_00008 int@0\n"
    "LABEL while_iteration_00006\n"
    "LT LF@tmp_00009 LF@i_00008 LF@minLen_00006\n"
    "JUMPIFEQ end_while_00007 LF@tmp_00009 bool@false\n"
    "STRI2INT LF@tmp_00011 LF@a_00001 LF@i_00008\n"
    "MOVE LF@aChar_00010 LF@tmp_00011\n"
    "STRI2INT LF@tmp_00013 LF@b_00000 LF@i_00008\n"
    "MOVE LF@bChar_00012 LF@tmp_00013\n"
    "LT LF@tmp_00014 LF@aChar_00010 LF@bChar_00012\n"
    "JUMPIFEQ if_true_00009 LF@tmp_00014 bool@true\n"
    "GT LF@tmp_00015 LF@aChar_00010 LF@bChar_00012\n"
    "JUMPIFEQ end_if_00010 LF@tmp_00015 bool@false\n"
    "PUSHS int@1\n"
    "RETURN\n"
    "LABEL end_if_00010\n"
    "JUMP end_if_00008\n"
    "LABEL if_true_00009\n"
    "SUB LF@tmp_00016 int@0 int@1\n"
    "PUSHS LF@tmp_00016\n"
    "RETURN\n"
    "LABEL end_if_00008\n"
    "ADD LF@tmp_00017 LF@i_00008 int@1\n"
    "MOVE LF@i_00008 LF@tmp_00017\n"
    "JUMP while_iteration_00006\n"
    "LABEL end_while_00007\n"
    "LT LF@tmp_00018 LF@lena_00002 LF@lenb_00004\n"
    "JUMPIFEQ if_true_00012 LF@tmp_00018 bool@true\n"
    "GT LF@tmp_00019 LF@lena_00002 LF@lenb_00004\n"
    "JUMPIFEQ end_if_00013 LF@tmp_00019 bool@false\n"
    "PUSHS int@1\n"
    "RETURN\n"
    "LABEL end_if_00013\n"
    "JUMP end_if_00011\n"
    "LABEL if_true_00012\n"
    "SUB LF@tmp_00020 int@0 int@1\n"
    "PUSHS LF@tmp_00020\n"
    "RETURN\n"
    "LABEL end_if_00011\n"
    "PUSHS int@0\n"
    "RETURN\n";

const char *ifjSubstringFunctionBody =
    "DEFVAR LF@j_00000\n"
    "DEFVAR LF@i_00001\n"
    "DEFVAR LF@s_00002\n"
    "DEFVAR LF@len_00003\n"
    "DEFVAR LF@tmp_00004\n"
    "DEFVAR LF@tmp_00005\n"
    "DEFVAR LF@tmp_00006\n"
    "DEFVAR LF@tmp_00007\n"
    "DEFVAR LF@tmp_00008\n"
    "DEFVAR LF@tmp_00009\n"
    "DEFVAR LF@ssub_00010\n"
    "DEFVAR LF@tmp_00011\n"
    "DEFVAR LF@idx_00012\n"
    "DEFVAR LF@tmp_00013\n"
    "DEFVAR LF@charStr_00014\n"
    "DEFVAR LF@tmp_00015\n"
    "DEFVAR LF@tmp_00016\n"
    "DEFVAR LF@tmp_00017\n"
    "DEFVAR LF@tmp_00018\n"
    "POPS LF@j_00000\n"
    "POPS LF@i_00001\n"
    "POPS LF@s_00002\n"
    "STRLEN LF@tmp_00004 LF@s_00002\n"
    "MOVE LF@len_00003 LF@tmp_00004\n"
    "LT LF@tmp_00005 LF@i_00001 int@0\n"
    "JUMPIFEQ end_if_00014 LF@tmp_00005 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_00014\n"
    "LT LF@tmp_00006 LF@j_00000 int@0\n"
    "JUMPIFEQ end_if_00015 LF@tmp_00006 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_00015\n"
    "GT LF@tmp_00007 LF@i_00001 LF@j_00000\n"
    "JUMPIFEQ end_if_00016 LF@tmp_00007 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_00016\n"
    "LT LF@tmp_00008 LF@i_00001 LF@len_00003\n"
    "NOT LF@tmp_00008 LF@tmp_00008\n"
    "JUMPIFEQ end_if_00017 LF@tmp_00008 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_00017\n"
    "LT LF@tmp_00009 LF@j_00000 LF@len_00003\n"
    "NOT LF@tmp_00009 LF@tmp_00009\n"
    "JUMPIFEQ end_if_00018 LF@tmp_00009 bool@false\n"
    "PUSHS nil@nil\n"
    "RETURN\n"
    "LABEL end_if_00018\n"
    "MOVE LF@tmp_00011 string@\n"
    "MOVE LF@ssub_00010 LF@tmp_00011\n"
    "MOVE LF@idx_00012 LF@i_00001\n"
    "LABEL while_iteration_00019\n"
    "LT LF@tmp_00013 LF@idx_00012 LF@j_00000\n"
    "JUMPIFEQ end_while_00020 LF@tmp_00013 bool@false\n"
    "STRI2INT LF@tmp_00016 LF@s_00002 LF@idx_00012\n"
    "INT2CHAR LF@tmp_00015 LF@tmp_00016\n"
    "MOVE LF@charStr_00014 LF@tmp_00015\n"
    "CONCAT LF@tmp_00017 LF@ssub_00010 LF@charStr_00014\n"
    "MOVE LF@ssub_00010 LF@tmp_00017\n"
    "ADD LF@tmp_00018 LF@idx_00012 int@1\n"
    "MOVE LF@idx_00012 LF@tmp_00018\n"
    "JUMP while_iteration_00019\n"
    "LABEL end_while_00020\n"
    "PUSHS LF@ssub_00010\n"
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
void generateIfjSubstringFunction();

/// @brief
/// @param node
/// @return Returns a pointer to a next node to process.
void generateStatement(ASTNode *node);
void generateDeclaration(ASTNode *node);
void generateAssignment(ASTNode *node);

/// @brief Expression is converted to a sequence of
///        instructions that lead to outVar value to be computed.
void generateExpression(ASTNode *node, Operand *outVar);
void generateBinaryExpression(ASTNode *node, Operand *outVar);

void generateNullBindingCheck(ASTNode *node, Operand *outVar);
void generateNullBindingAssignment(ASTNode *node);

void generateIfCondition(ASTNode *node);
void unrollIfConditions(ASTNode *node, Operand endLabel, bool firstEvaluation);
void unrollLastIfConditional(ASTNode *node, Operand endLabel, bool firstEvaluation);

void generateWhileConditional(ASTNode *node);
void generateSimpleBuiltInFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCallParameter(ASTNode *node);
void generateReturn(ASTNode *node);

Operand initConstantOperand(ASTNode *node);
Operand initCompileTimeOperand(ASTNode *node);

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
  if (created)
  {
    loginfo("Creating label: %s", labelName);
  }
  else
  {
    loginfo("Getting existing label: %s", labelName);
  }

  Operand var = initStringOperand(OP_LABEL, labelName);
  return var;
}

Operand createTmpLabel(char *name)
{
  char *labelName = IdIndexer_CreateOneTime(labelIndexer, name);
  loginfo("Creating temporary label: %s", labelName);

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
  loginfo("Generating function %s", node->name);

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
  Operand funcNameLabel = getOrCreateLabel(node->name);
  Instruction inst = initInstr1(INST_LABEL, funcNameLabel);
  TFC_SetFuncLabel(funcScope, funcNameLabel.attr.string);

  // Load function parameters from stack
  Param *param = SymTable_GetParamList(symTable, node->name);
  generateFunctionParametersInitialization(param);

  // Generate function body
  ASTNode *statementNode = node->next;
  while (statementNode != NULL)
  {
    generateStatement(statementNode);
    statementNode = statementNode->next;
  }

  // TODO: generate only if last instruction was not return
  loginfo("Generating function return: %s", node->right->name);
  if (strcmp(node->right->name, "void") == 0)
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

void generateStatement(ASTNode *node)
{
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
    generateStatement(node->left);
    break;
  case ConditionalStatement:
    assert(node->left != NULL);
    loginfo("Generating conditional statement: %s", nodeTypeToString(node->left->nodeType));
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
}

void generateDeclaration(ASTNode *node)
{
  // Get var unique name
  Operand varOperand = getOrCreateVar(node->name, FRAME_LF);

  // Make an assignment
  bool isCompileTimeValue = node->right == NULL;
  if (isCompileTimeValue)
  {
    addInstruction(initInstr2(INST_MOVE, varOperand, initCompileTimeOperand(node)));
  }
  else
  {
    Operand value;
    generateExpression(node->right, &value);
    addInstruction(initInstr2(INST_MOVE, varOperand, value));
  }
}

void generateAssignment(ASTNode *node)
{
  assert(node->nodeType == Assignment);
  loginfo("Generating assignment to: %s", node->name);
  inspectAstNode(node);

  // If variable name is _, generate a temporary variable name
  Operand dest;
  if (strcmp(node->name, "_") == 0)
  {
    // TODO: A value is assigned but would never be used. Can it be done in some better way?
    dest = createTmpVar("_", FRAME_LF);
  }
  else
  {
    dest = getOrCreateVar(node->name, FRAME_LF);
  }

  // Generate assigment evaluation
  bool isCompileTimeValue = node->left == NULL;
  if (isCompileTimeValue)
  {
    addInstruction(initInstr2(INST_MOVE, dest, initCompileTimeOperand(node)));
  }
  else
  {
    Operand src;
    generateExpression(node->left, &src);
    addInstruction(initInstr2(INST_MOVE, dest, src));
  }
}

void generateExpression(ASTNode *node, Operand *outVar)
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
    generateBinaryExpression(node, outVar);
    break;

  case FuncCall:
  case BuiltInFunctionCall: // TODO: Delete this enum value
    generateFunctionCall(node, outVar);
    break;

  case Identifier:
    *outVar = getOrCreateVar(node->name, FRAME_LF);
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
    leftOperand = getOrCreateVar(node->left->name, FRAME_LF);
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
    rightOperand = getOrCreateVar(node->right->name, FRAME_LF);
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
}

void generateNullBindingCheck(ASTNode *node, Operand *outVar)
{
  *outVar = createTmpVar("not_null", FRAME_LF);

  Operand varTypeOperand = createTmpVar("var_type", FRAME_LF);
  Instruction getTypeInstruction = initInstr2(
      INST_TYPE,
      varTypeOperand,
      getOrCreateVar(node->left->name, FRAME_LF));
  addInstruction(getTypeInstruction);

  // Put true if the value is not NULL
  Instruction putIsNullCheck = initInstr3(
      INST_EQ,
      *outVar,
      varTypeOperand,
      initStringOperand(OP_CONST_STRING, "nil"));
  addInstruction(putIsNullCheck);

  Instruction negateInst = initInstr2(INST_NOT, *outVar, *outVar);
  addInstruction(negateInst);
}

void generateNullBindingAssignment(ASTNode *node)
{
  Operand nullBindingResult = getOrCreateVar(node->binding->name, FRAME_LF);
  char *nullableVarName = node->left->name;
  addInstruction(
      initInstr2(
          INST_MOVE,
          nullBindingResult,
          getOrCreateVar(nullableVarName, FRAME_LF)));
}

void generateIfCondition(ASTNode *node)
{
  Operand endLabel = createTmpLabel("end_if");
  loginfo("Got label to end if: %s", endLabel.attr.string);

  unrollIfConditions(node, endLabel, true);

  addInstruction(initInstr1(INST_LABEL, endLabel));
}

/// @brief Processes all nodes of the conditional statement.
/// @param node
/// @param endLabel
/// @param firstEvaluation
/// @return Pointer to a next node to process.
void unrollIfConditions(ASTNode *node, Operand endLabel, bool firstEvaluation)
{
  assert(node->nodeType == IfCondition);

  bool isLastEvaluation = node->next == NULL || node->next->nodeType != IfCondition;
  if (isLastEvaluation)
  {
    unrollLastIfConditional(node, endLabel, firstEvaluation);
    return;
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

  // If not last evaluation, JUMP on positive condition
  Instruction jumpToBlockInst = initInstr3(
      INST_JUMPIFEQ,
      endLabel,
      ifCondition,
      initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = true}));
  addInstruction(jumpToBlockInst);

  // Unroll next evaluation.
  unrollIfConditions(node->next, endLabel, false);

  // Generate body
  if (node->binding != NULL)
  {
    generateNullBindingAssignment(node);
  }
  ASTNode *ifBlockStatement = node->right;
  while (ifBlockStatement != NULL)
  {
    generateStatement(ifBlockStatement);
    ifBlockStatement = ifBlockStatement->next;
  }

  Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
  addInstruction(jumpToEnd);

  return;
}

void unrollLastIfConditional(ASTNode *node, Operand endLabel, bool firstEvaluation)
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

      generateStatement(elseBlockStatement);
      elseBlockStatement = elseBlockStatement->next;
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
      generateStatement(ifBlockStatement);
      ifBlockStatement = ifBlockStatement->next;
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
      generateStatement(ifBlockStatement);
      ifBlockStatement = ifBlockStatement->next;
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
    generateStatement(bodyStatement);
    bodyStatement = bodyStatement->next;
  }

  // Jump to the beginning of the loop
  addInstruction(initInstr1(INST_JUMP, whileIterLabel));

  // Add label for the end of the while loop
  addInstruction(initInstr1(INST_LABEL, endWhileLabel));

  loginfo("While loop generated");
}

void generateFunctionCall(ASTNode *node, Operand *outVar)
{
  char *functionName = node->name;
  if (strcmp(node->name, "ifj.strcmp") == 0)
  {
    functionName = "ifj_strcmp";
  }
  else if (strcmp(node->name, "ifj.substring") == 0)
  {
    functionName = "ifj_substring";
  }
  else if (strncmp(node->name, "ifj.", 4) == 0)
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
  loginfo("Generating builtin function call: %s", node->name);

  // ifj.write does not return a value.
  // Check for it early to avoid repeating code for operand creation.
  if (strcmp(node->name, "ifj.write") == 0)
  {
    Operand writeOperand;
    generateExpression(node->left, &writeOperand);
    Instruction writeInst = initInstr1(INST_WRITE, writeOperand);
    addInstruction(writeInst);
    return;
  }

  *outVar = createTmpVar("tmp", FRAME_LF);

  if (strcmp(node->name, "ifj.readstr") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "string"));
    addInstruction(readInst);
  }
  else if (strcmp(node->name, "ifj.readi32") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "int"));
    addInstruction(readInst);
  }
  else if (strcmp(node->name, "ifj.readf64") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "float"));
    addInstruction(readInst);
  }
  else if (strcmp(node->name, "ifj.i2f") == 0)
  {
    Operand convertOperand;
    generateExpression(node->left, &convertOperand);
    Instruction convertInst = initInstr2(
        INST_INT2FLOAT,
        *outVar,
        convertOperand);
    addInstruction(convertInst);
  }
  else if (strcmp(node->name, "ifj.f2i") == 0)
  {
    Operand convertOperand;
    generateExpression(node->left, &convertOperand);
    Instruction convertInst = initInstr2(
        INST_FLOAT2INT,
        *outVar,
        convertOperand);
    addInstruction(convertInst);
  }
  else if (strcmp(node->name, "ifj.string") == 0)
  {
    assert(node->left->nodeType == StringLiteral);
    Operand literalToConvert = initConstantOperand(node->left);
    Instruction toStringInst = initInstr2(
        INST_MOVE,
        *outVar,
        literalToConvert);
    addInstruction(toStringInst);
  }
  else if (strcmp(node->name, "ifj.length") == 0)
  {
    Operand strlenOperand;
    generateExpression(node->left, &strlenOperand);
    Instruction toStringInst = initInstr2(INST_STRLEN, *outVar, strlenOperand);
    addInstruction(toStringInst);
  }
  else if (strcmp(node->name, "ifj.concat") == 0)
  {
    Operand firstOperand;
    generateExpression(node->left, &firstOperand);
    Operand secondOperand;
    generateExpression(node->left->next, &secondOperand);

    Instruction concatInst = initInstr3(INST_CONCAT, *outVar, firstOperand, secondOperand);
    addInstruction(concatInst);
  }
  else if (strcmp(node->name, "ifj.ord") == 0)
  {
    // Convert character at given index to int
    Operand u8;
    generateExpression(node->left, &u8);
    Operand idx;
    generateExpression(node->left->next, &idx);
    Instruction strToIntInst = initInstr3(INST_STRI2INT, *outVar, u8, idx);
    addInstruction(strToIntInst);
  }
  else if (strcmp(node->name, "ifj.chr") == 0)
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
    loginfo("Unexpected builtin function call: %s", node->name);
    exit(99);
  }
}

void generateReturn(ASTNode *node)
{
  loginfo("Generating return statement");

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

Operand initCompileTimeOperand(ASTNode *node)
{
  switch (node->valType)
  {
  case I32_LITERAL:
    return initOperand(OP_CONST_INT64, (OperandAttribute){.i64 = node->value.integer});
  case F64_LITERAL:
    return initOperand(OP_CONST_FLOAT64, (OperandAttribute){.f64 = node->value.real});
  case STR_LITERAL:
    char *literalString = convertToCompatibleStringLiteral(node->value.string);
    return initStringOperand(OP_CONST_STRING, literalString);
  default:
    loginfo("Unexpected compile time type: %s", nodeTypeToString(node->valType));
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
