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

void generateFunctions(ASTNode *node);
void generateStatements(ASTNode *node);
void generateDeclaration(ASTNode *node);
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
  generateFunctions(root->right);

  // Add label to the end of the program. After main function is done,
  // the program will jump to this label to end the program.
  Operand var = initStringOperand(OP_LABEL, endProgramLabelName);
  Instruction inst = initInstr1(INST_LABEL, var);
  addInstruction(inst);

  IdIndexer_Destroy(labelIndexer);
  labelIndexer = NULL;

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

void generateFunctions(ASTNode *node)
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
  generateStatements(node->next);

  // Print every instruction accumulated for the current scope(function)
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

  // Generate every other function
  if (node->binding != NULL)
  {
    return generateFunctions(node->binding);
  }
}

void generateStatements(ASTNode *node)
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
    generateStatements(node->left);
    break;
  case IfStatement:
    loginfo("If statement not implemented yet");
    exit(99);
  case WhileStatement:
    loginfo("While statement not implemented yet");
    exit(99);
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
    inspectAstNode(node);
    loginfo("Unexpected statement type: %s", nodeTypeToString(node->nodeType));
    exit(99);
  }

  // Generate every consecutive statement
  if (node->next != NULL)
  {
    generateStatements(node->next);
  }
}

void generateDeclaration(ASTNode *node)
{
  inspectAstNode(node);

  // Get var unique name
  char *varName = IdIndexer_GetOrCreate(funcVarsIndexer, node->value.string);
  Operand varOperand = initVarOperand(OP_VAR, FRAME_LF, varName);

  // Add to declarations
  addVarDefinition(&varOperand.attr.var);

  // Make an assignment
  generateExpression(node->right, &varOperand);
}

void generateAssignment(ASTNode *node)
{
  inspectAstNode(node);
  assert(node->nodeType == Assignment);
  assert(node->left->nodeType == Identifier);

  char *varName;
  Operand dest;
  if (strcmp(node->left->value.string, "_") == 0)
  {
    // TODO: A value is assigned but would never be used. Can it be done in some better way?
    varName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
  }
  else
  {
    varName = IdIndexer_GetOrCreate(funcVarsIndexer, node->left->value.string);
  }
  dest = initVarOperand(OP_VAR, FRAME_LF, varName);
  addVarDefinition(&dest.attr.var);

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
      addInstruction(inst);
    }
    break;

  case BuiltInFunctionCall:
    generateBuiltInFunctionCall(node, outVar);
    break;

  case Identifier:
    char *idName = IdIndexer_GetOrCreate(funcVarsIndexer, node->value.string);
    *outVar = initVarOperand(OP_VAR, FRAME_LF, idName);
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

    char *tmpVarName = IdIndexer_CreateOneTime(funcVarsIndexer, "tmp");
    Operand defVar = initVarOperand(OP_VAR, FRAME_LF, tmpVarName);
    addVarDefinition(&defVar.attr.var);

    // defVar = evaluate(right)
    generateExpression(node->right, &defVar);

    // outVar = outVar <op> defVar
    inst = initInstr3(
        instType,
        *outVar,
        *outVar,
        defVar);
  }

  addInstruction(inst);
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

bool isConstant(ASTNode *node)
{
  return node->nodeType == IntLiteral || node->nodeType == FloatLiteral || node->nodeType == StringLiteral || node->nodeType == NullLiteral;
}

bool isVarOrConstant(ASTNode *node)
{
  return node->nodeType == Identifier || isConstant(node);
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

void generateFunctionCall(ASTNode *node)
{
  assert(node->nodeType == FuncCall);

  // Create TF for parameters
  Instruction createFrameInst = initInstr0(INST_CREATEFRAME);
  addInstruction(createFrameInst);

  // Add parameters
  generateFunctionCallParameters(node->left);

  // Push frame
  Instruction pushFrameInst = initInstr0(INST_PUSHFRAME);
  addInstruction(pushFrameInst);

  // Call function
  char *funcNameLabel = IdIndexer_GetOrCreate(labelIndexer, node->value.string);
  Instruction callInst = initInstr1(INST_CALL, initStringOperand(OP_LABEL, funcNameLabel));
  addInstruction(callInst);

  // Pop frame
  Instruction popFrameInst = initInstr0(INST_POPFRAME);
  addInstruction(popFrameInst);
}

void generateFunctionCallParameters(ASTNode *node)
{
  inspectAstNode(node);

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
  addInstruction(instr);
}
