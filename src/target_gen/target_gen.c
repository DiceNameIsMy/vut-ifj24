//
// Created by nur on 16.11.24.
//

#include <string.h>
#include <assert.h>

#include "logging.h"

#include "structs/dynBuffer.h"
#include "structs/ast.h"

#include "target_gen/id_indexer.h"
#include "target_gen/instructions.h"
#include "target_gen/target_gen.h"
#include "target_gen/target_func_context.h"

FILE *outputStream;

SymTable *symTable = NULL;

TargetFuncContext *funcScope = NULL;
IdIndexer *funcVarsIndexer = NULL;

IdIndexer *labelIndexer = NULL;

/**********************************************************/
/* Private Function Declarations */
/**********************************************************/

/// @brief If there is scope defined, add it there. Otherwise, print it to the output stream.
void addInstruction(Instruction inst);

/// @brief Call this function if a new variable must be defined for a
///        function that is being generated.
void addVarDefinition(Variable *var);

void generateFunction(ASTNode *node);
void generateFunctionParametersInitialization(Param *param);

void generateStatement(ASTNode *node);
void generateDeclaration(ASTNode *node);
void generateAssignment(ASTNode *node);

/// @brief Expression is converted to a sequence of
///        instructions that lead to outVar value to be computed.
void generateExpression(ASTNode *node, Operand *outVar);
void generateBinaryExpression(ASTNode *node, Operand *outVar);

void generateConditionalStatement(ASTNode *node);
void unrollConditionalStatements(ASTNode *node, Operand endLabel, bool firstEvaluation);
void unrollLastConditionalStatement(ASTNode *node, Operand endLabel, bool firstEvaluation);

void generateWhileStatement(ASTNode *node);
void generateBuiltInFunctionCall(ASTNode *node, Operand *outVar);
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
  char *mainLabel = IdIndexer_GetOrCreate(labelIndexer, "main");
  Operand mainFunc = initStringOperand(OP_LABEL, mainLabel);
  Instruction callMainInst = initInstr1(INST_CALL, mainFunc);
  addInstruction(callMainInst);

  // Pop the frame since its no longer needed
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  addInstruction(popFrameInst);

  // Jump to the end of the generated file
  char *endProgramLabelName = IdIndexer_GetOrCreate(labelIndexer, "end_program");
  Operand endProgramLabel = initStringOperand(OP_LABEL, endProgramLabelName);
  Instruction jumpToEndInst = initInstr1(INST_JUMP, endProgramLabel);
  addInstruction(jumpToEndInst);

  // Generate functions
  ASTNode *funcNode = root->right;
  while (funcNode != NULL)
  {
    generateFunction(funcNode);
    funcNode = funcNode->binding;
  }

  // Add label to the end of the program. After main function is done,
  // the program will jump to this label to end the program.
  Operand var = initStringOperand(OP_LABEL, endProgramLabelName);
  Instruction inst = initInstr1(INST_LABEL, var);
  addInstruction(inst);

  IdIndexer_Destroy(labelIndexer);
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

void addVarDefinition(Variable *var)
{
  if (funcScope == NULL)
  {
    loginfo("Attemted to add var definition outside of a function scope");
    exit(99);
  }
  TFC_AddVar(funcScope, *var);
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
  char *funcLabel = IdIndexer_GetOrCreate(labelIndexer, node->value.string);
  Instruction inst = initInstr1(INST_LABEL, initStringOperand(OP_LABEL, funcLabel));
  TFC_SetFuncLabel(funcScope, funcLabel);

  // Load function parameters from stack
  Param *param = SymTable_GetParamList(symTable, node->value.string);
  generateFunctionParametersInitialization(param);

  // Generate function body
  ASTNode *statementNode = node->next;
  while (statementNode != NULL)
  {
    generateStatement(statementNode);
    statementNode = statementNode->next;
  }

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
  while (param != NULL)
  {
    loginfo("Generating function parameter: %s", param->name);
    // Add variable definition
    char *varName = IdIndexer_GetOrCreate(funcVarsIndexer, param->name);
    Operand var = initVarOperand(OP_VAR, FRAME_LF, varName);
    addVarDefinition(&var.attr.var);

    // Load variable from stack
    Instruction inst = initInstr1(INST_POPS, var);
    addInstruction(inst);

    param = param->next;
  }
}

void generateStatement(ASTNode *node)
{
  loginfo("Generating statement: %s", nodeTypeToString(node->nodeType));

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
  case IfStatement:
    generateConditionalStatement(node);
    break;
  case WhileStatement:
    generateWhileStatement(node);
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
  char *varName = IdIndexer_GetOrCreate(funcVarsIndexer, node->value.string);
  Operand varOperand = initVarOperand(OP_VAR, FRAME_LF, varName);

  // Add to declarations
  addVarDefinition(&varOperand.attr.var);

  // Make an assignment
  Operand value;
  generateExpression(node->right, &value);
  addInstruction(initInstr2(INST_MOVE, varOperand, value));
}

void generateAssignment(ASTNode *node)
{
  assert(node->nodeType == Assignment);

  // If variable name is _, generate a temporary variable name
  char *varName;
  if (strcmp(node->left->value.string, "_") == 0)
  {
    // TODO: A value is assigned but would never be used. Can it be done in some better way?
    varName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
  }
  else
  {
    varName = IdIndexer_GetOrCreate(funcVarsIndexer, node->value.string);
  }

  Operand dest;
  dest = initVarOperand(OP_VAR, FRAME_LF, varName);

  // Generate assigment evaluation
  generateExpression(node->left, &dest);
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
    char *identifierName = IdIndexer_GetOrCreate(funcVarsIndexer, node->value.string);
    *outVar = initVarOperand(OP_VAR, FRAME_LF, identifierName);
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
    leftOperand = initVarOperand(OP_VAR, FRAME_LF, IdIndexer_GetOrCreate(funcVarsIndexer, node->left->value.string));
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
    rightOperand = initVarOperand(OP_VAR, FRAME_LF, IdIndexer_GetOrCreate(funcVarsIndexer, node->right->value.string));
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
    char *tmpVarName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
    rightOperand = initVarOperand(OP_VAR, FRAME_LF, tmpVarName);
    addVarDefinition(&rightOperand.attr.var);

    generateExpression(node->right, &rightOperand);
  }

  if (!outVarInitialized)
  {
    char *outVarName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
    *outVar = initVarOperand(OP_VAR, FRAME_LF, outVarName);
    addVarDefinition(&outVar->attr.var);
  }

  Instruction inst;
  inst = initInstr3(binaryNodeToInstructionType(node), *outVar, leftOperand, rightOperand);
  addInstruction(inst);

  bool negate = negateBinaryInstruction(node);
  if (negate)
  {
    Instruction negateInst = initInstr2(INST_NOT, *outVar, *outVar);
    addInstruction(negateInst);
  }
}

void generateConditionalStatement(ASTNode *node)
{
  // TODO: null binding
  Operand endLabel = initStringOperand(OP_LABEL, IdIndexer_CreateOneTime(labelIndexer, "end_if"));

  loginfo("Generating conditional statement");
  unrollConditionalStatements(node, endLabel, true);

  addInstruction(initInstr1(INST_LABEL, endLabel));
}

void unrollConditionalStatements(ASTNode *node, Operand endLabel, bool firstEvaluation)
{
  assert(node->nodeType == IfStatement);

  bool isLastEvaluation = node->next == NULL || node->next->nodeType != IfStatement;
  if (isLastEvaluation)
  {
    unrollLastConditionalStatement(node, endLabel, firstEvaluation);
    return;
  }

  // Evaluate condition
  Operand ifCondition;
  generateExpression(node->left, &ifCondition);

  // If not last evaluation, JUMP on positive condition
  Instruction jumpToBlockInst = initInstr3(
      INST_JUMPIFEQ,
      endLabel,
      ifCondition,
      initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = true}));
  addInstruction(jumpToBlockInst);

  // Unroll next evaluation.
  unrollConditionalStatements(node->next, endLabel, false);

  // Generate body
  ASTNode *ifBlockStatement = node->right;
  while (ifBlockStatement != NULL)
  {
    generateStatement(ifBlockStatement);
    ifBlockStatement = ifBlockStatement->next;
  }

  Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
  addInstruction(jumpToEnd);
}

void unrollLastConditionalStatement(ASTNode *node, Operand endLabel, bool firstEvaluation)
{
  loginfo("Generating a last `if` evaluation");

  // Evaluate condition
  Operand ifCondition;
  generateExpression(node->left, &ifCondition);

  bool hasTrailingElse = node->next != NULL && node->next->nodeType != IfStatement;
  if (hasTrailingElse)
  {
    // Jump to the code to execute if the condition is true.
    Instruction jumpToBlockInst = initInstr3(
        INST_JUMPIFEQ,
        endLabel,
        ifCondition,
        initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = true}));
    addInstruction(jumpToBlockInst);

    // Unroll the else block.
    ASTNode *elseBlockStatement = node->next;
    while (elseBlockStatement != NULL)
    {
      generateStatement(elseBlockStatement);
      elseBlockStatement = elseBlockStatement->next;
    }
    // Jump to the end
    Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
    addInstruction(jumpToEnd);

    // Generate body on true condition
    ASTNode *ifBlockStatement = node->right;
    while (ifBlockStatement != NULL)
    {
      generateStatement(ifBlockStatement);
      ifBlockStatement = ifBlockStatement->next;
    }
  }
  else
  {
    // Optimization: If it's a last evaluation, jump to end on negative condition. Put the code right after.
    Instruction jumpToEndOnNegativeCondition = initInstr3(
        INST_JUMPIFEQ,
        endLabel,
        ifCondition,
        initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = false}));
    addInstruction(jumpToEndOnNegativeCondition);

    // Generate body
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

void generateWhileStatement(ASTNode *node)
{
  // Create a label for the beginning of the while loop
  char *whileIterLabelName = IdIndexer_CreateOneTime(labelIndexer, "while_iteration");
  Operand whileIterLabel = initStringOperand(OP_LABEL, whileIterLabelName);
  addInstruction(initInstr1(INST_LABEL, whileIterLabel));

  // Define a label name to jump to when the condition is false
  char *endWhileLabelName = IdIndexer_CreateOneTime(labelIndexer, "end_while");
  Operand endWhileLabel = initStringOperand(OP_LABEL, endWhileLabelName);

  // Evaluate while (...)
  Operand whileCondition;
  generateExpression(node->left, &whileCondition);

  // Jump to the end of the loop if the condition is false
  Instruction instrCondEndLoop = initInstr3(
      INST_JUMPIFEQ,
      endWhileLabel,
      whileCondition,
      initOperand(OP_CONST_BOOL, (OperandAttribute){.boolean = false}));
  addInstruction(instrCondEndLoop);

  // Generate a body of while () { ... }
  ASTNode *bodyStatement = node->right;
  while (bodyStatement != NULL)
  {
    generateStatement(bodyStatement);
    bodyStatement = bodyStatement->next;
  }

  // Jump to the beginning of the loop
  addInstruction(initInstr1(INST_JUMP, whileIterLabel));

  // Add label for the end of the while loop
  addInstruction(initInstr1(INST_LABEL, endWhileLabel));
}

void generateBuiltInFunctionCall(ASTNode *node, Operand *outVar)
{
  if (strcmp(node->value.string, "ifj.write") == 0)
  {
    Operand writeOperand;
    generateExpression(node->left, &writeOperand);
    Instruction writeInst = initInstr1(INST_WRITE, writeOperand);
    addInstruction(writeInst);
    return;
  }

  *outVar = initVarOperand(OP_VAR, FRAME_LF, IdIndexer_CreateOneTime(funcVarsIndexer, "tmp"));
  addVarDefinition(&outVar->attr.var);

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
        initStringOperand(OP_TYPE, "i64"));
    addInstruction(readInst);
  }
  else if (strcmp(node->value.string, "ifj.readf64") == 0)
  {
    Instruction readInst = initInstr2(
        INST_READ,
        *outVar,
        initStringOperand(OP_TYPE, "f64"));
    addInstruction(readInst);
  }
  else if (strcmp(node->value.string, "ifj.write") == 0)
  {
    Operand writeOperand;
    generateExpression(node->left, &writeOperand);
    Instruction writeInst = initInstr1(INST_WRITE, writeOperand);
    addInstruction(writeInst);
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
    // TODO: Read parameters from a function call properly
    Operand firstOperand;
    generateExpression(node->left, &firstOperand);
    Operand secondOperand;
    generateExpression(node->left->left, &secondOperand);

    Instruction concatInst = initInstr3(INST_CONCAT, *outVar, firstOperand, secondOperand);
  }
  else if (strcmp(node->value.string, "ifj.substring") == 0)
  {
    // TODO: Implement substring
  }
  else if (strcmp(node->value.string, "ifj.strcmp") == 0)
  {
    // TODO: Implement strcmp
  }
  else if (strcmp(node->value.string, "ifj.ord") == 0)
  {
    // TODO: Implement ord
  }
  else if (strcmp(node->value.string, "ifj.chr") == 0)
  {
    // TODO: Implement chr
  }
  else
  {
    loginfo("Unexpected builtin function call: %s", node->value.string);
    exit(99);
  }
}

void generateFunctionCall(ASTNode *node, Operand *outVar)
{
  loginfo("Generating function call: %s", node->value.string);

  if (strncmp(node->value.string, "ifj.", 4) == 0)
  {
    generateBuiltInFunctionCall(node, outVar);
    return;
  }

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

  loginfo("Calling function: %s", node->value.string);

  // Call function
  char *funcNameLabel = IdIndexer_GetOrCreate(labelIndexer, node->value.string);
  Instruction callInst = initInstr1(INST_CALL, initStringOperand(OP_LABEL, funcNameLabel));
  addInstruction(callInst);

  // Pop frame
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  addInstruction(popFrameInst);

  // If function returns a value, put it into outVar
  bool returnsVoid = node->valType == NONETYPE;
  if (!returnsVoid)
  {
    char *outVarName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
    *outVar = initVarOperand(OP_VAR, FRAME_LF, outVarName);
    addVarDefinition(&outVar->attr.var);
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

void generateReturn(ASTNode *node)
{
  if (node->valType == NONETYPE) // Return void
  {
    Instruction returnInst = initInstr0(INST_RETURN);
    addInstruction(returnInst);
    return;
  }

  ASTNode *returnValueNode = node->left;

  Operand returnOperand;

  if (node->left->nodeType == Identifier)
  {
    // Identifiers can be inlined.
    char *idName = IdIndexer_GetOrCreate(funcVarsIndexer, returnValueNode->value.string);
    returnOperand = initVarOperand(OP_VAR, FRAME_LF, idName);
  }
  else if (isConstant(returnValueNode))
  {
    // Constants can be inlined.
    returnOperand = initConstantOperand(returnValueNode);
  }
  else
  {
    // Other expressions must be evaluated with extra instructions.
    char *tmpVarName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
    returnOperand = initVarOperand(OP_VAR, FRAME_LF, tmpVarName);

    // Generate an expression that assigns the result to the returnOperand
    generateExpression(node->left, &returnOperand);
  }

  // Push the return value to the stack
  Instruction pushInst = initInstr1(INST_PUSHS, returnOperand);
  addInstruction(pushInst);
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
    loginfo("Converted string literal: %s -> %s", node->value.string, literalString);
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
  if (initDynBuffer(&buffer, -1) != 0)  {
    loginfo("Failed to initialize dynamic buffer");
    exit(99);
  }

  for (int i = 0; i < strlen(stringLiteral); i++)
  {
    char c = stringLiteral[i];
    if (c <= 32 || c == '#' || c == '\\') {
      // Reformat special characters to \xxx
      appendStringDynBuffer(&buffer, "\\___");
      sprintf(&buffer.data[buffer.nextIdx-3], "%03d", c);
    } else {
      appendDynBuffer(&buffer, c);
    }
  }
  char *result;
  if (copyFromDynBuffer(&buffer, &result) != 0) {
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
