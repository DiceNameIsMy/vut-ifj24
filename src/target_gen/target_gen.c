//
// Created by nur on 16.11.24.
//

#include <string.h>
#include <assert.h>

#include "logging.h"

#include "structs/ast.h"

#include "target_gen/id_indexer.h"
#include "target_gen/instructions.h"
#include "target_gen/target_gen.h"
#include "target_gen/target_func_context.h"

FILE *outputStream;

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
void generateStatement(ASTNode *node);
void generateDeclaration(ASTNode *node);
void generateAssignment(ASTNode *node);

/// @brief Expression is converted to a sequence of
///        instructions that lead to outVar value to be computed.
void generateExpression(ASTNode *node, Operand *outVar);
void generateBinaryExpression(ASTNode *node, Operand *outVar);

void generateConditionalStatement(ASTNode *node);
void unrollConditionalStatements(ASTNode *node, Operand endLabel, bool firstEvaluation);
void generateWhileStatement(ASTNode *node);
void generateBuiltInFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCall(ASTNode *node, Operand *outVar);
void generateFunctionCallParameter(ASTNode *node);

InstType binaryNodeToInstructionType(ASTNode *node);
bool negateBinaryInstruction(ASTNode *node);
Operand initConstantOperand(ASTNode *node);
bool isConstant(ASTNode *node);
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
int generateTargetCode(ASTNode *root, FILE *output)
{
  loginfo("Generating target code");

  if (output == NULL)
  {
    return -1;
  }
  outputStream = output;

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

  return 0;
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

  // Generate function body
  ASTNode *statementNode = node->next;
  while (statementNode != NULL)
  {
    generateStatement(statementNode);
    statementNode = statementNode->next;
  }

  // TODO: Add return statement if it's implicit
  loginfo("Function %s returns %i", node->value.string, node->right->valType);
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

    if (node->valType == NONETYPE) // Return void
    {
      Instruction returnInst = initInstr0(INST_RETURN);
      addInstruction(returnInst);
    }
    else
    {
      ASTNode *returnNode = node->left;
      Operand returnOperand;

      bool generateInline = isVarOrConstant(returnNode);
      if (node->left->nodeType == Identifier)
      {
        // Identifiers can be inlined.
        char *idName = IdIndexer_GetOrCreate(funcVarsIndexer, returnNode->value.string);
        returnOperand = initVarOperand(OP_VAR, FRAME_LF, idName);
      }
      else if (isConstant(returnNode))
      {
        // Constants can be inlined.
        returnOperand = initConstantOperand(returnNode);
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
    generateFunctionCall(node, outVar);
    break;

  case BuiltInFunctionCall:
    generateBuiltInFunctionCall(node, outVar);
    break;

  case Identifier:
    char *identifierName = IdIndexer_GetOrCreate(funcVarsIndexer, node->value.string);
    *outVar = initVarOperand(OP_VAR, FRAME_LF, identifierName);
    break;

  case IntLiteral:
    *outVar = initOperand(OP_CONST_INT64, (OperandAttribute){.i64 = node->value.integer});
    break;

  case FloatLiteral:
    *outVar = initOperand(OP_CONST_FLOAT64, (OperandAttribute){.f64 = node->value.real});
    break;

  case StringLiteral:
    *outVar = initStringOperand(OP_CONST_STRING, node->value.string);
    break;

  case NullLiteral:
    *outVar = initOperand(OP_CONST_NIL, (OperandAttribute){});
    break;

  default:
    loginfo("Unexpected factor type: %s", nodeTypeToString(node->nodeType));
    inspectAstNode(node);
    exit(99);
  }
}

void generateBinaryExpression(ASTNode *node, Operand *outVar)
{
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

bool isConstant(ASTNode *node)
{
  return node->nodeType == IntLiteral || node->nodeType == FloatLiteral || node->nodeType == StringLiteral || node->nodeType == NullLiteral;
}

bool isVarOrConstant(ASTNode *node)
{
  return node->nodeType == Identifier || isConstant(node);
}

void generateConditionalStatement(ASTNode *node)
{
  // TODO: null binding
  inspectAstNode(node);
  Operand endLabel = initStringOperand(OP_LABEL, IdIndexer_CreateOneTime(labelIndexer, "end_if"));

  unrollConditionalStatements(node, endLabel, true);

  addInstruction(initInstr1(INST_LABEL, endLabel));
}

void unrollConditionalStatements(ASTNode *node, Operand endLabel, bool firstEvaluation)
{
  assert(node->nodeType == IfStatement);

  // Evaluate condition
  Operand ifCondition;
  generateExpression(node->left, &ifCondition);
  loginfo("Generated if condition that's stored in %s", ifCondition.attr.var.name);

  bool isLastEvaluation = node->next->nodeType != IfStatement;
  if (isLastEvaluation)
  {
    // If last evaluation, JUMP to end on negative condition
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

    if (firstEvaluation)
    {
      // It's a first and last evaluation, no need to jump.
    }
    else
    {
      Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
      addInstruction(jumpToEnd);
    }
  }
  else
  {
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

    if (firstEvaluation)
    {
      // Instructions for first if check are located at the bottom.
      // There is no need to jump since the next instruction is the endLabel itself.
    }
    else
    {
      Instruction jumpToEnd = initInstr1(INST_JUMP, endLabel);
      addInstruction(jumpToEnd);
    }
  }
}

void generateWhileStatement(ASTNode *node)
{
  inspectAstNode(node);
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
  assert(node->nodeType == BuiltInFunctionCall);

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

void generateFunctionCall(ASTNode *node, Operand *outVar)
{
  loginfo("Generating function call: %s", node->value.string);

  assert(node->nodeType == FuncCall);

  // Create TF for parameters
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  addInstruction(createFrameInst);

  // Add parameters
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
  return;

  // TODO: From local frame move to temporary frame with a name defined in the function declaration.

  // TODO: Find the parameter name from the function's declaration
  Operand funcParamName = initVarOperand(OP_VAR, FRAME_TF, "called_func_param_name");

  // Get the value to set to the parameter
  Operand valueToSetTo;
  generateExpression(node, &valueToSetTo);

  // Set the function parameter
  Instruction instr = initInstr2(INST_MOVE, funcParamName, valueToSetTo);
  addInstruction(instr);
}
