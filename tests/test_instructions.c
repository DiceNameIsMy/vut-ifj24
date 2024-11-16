#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "target_gen/instructions.h"

#include "test_utils.h"

FILE *tempFile;


TEST(print_0_operand_instruction)
    fseek(tempFile, 0, SEEK_END);
    long startedAt = ftell(tempFile);

    Instruction inst;
    if (initInstr0(&inst, INST_RETURN) == -1) {
        FAIL("Failed to initialize instruction");
        return;
    }
    printInstruction(&inst, tempFile);
    destroyInstruction(&inst);

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), tempFile) == NULL) {
        FAIL("Failed to read from temporary file");
        return;
    }

    fseek(tempFile, startedAt, SEEK_SET);

    char *expected = "RETURN\n";
    if (strcmp(buffer, expected) != 0) {
        FAIL("Expected '%s', got '%s'", expected, buffer);
    }

ENDTEST

TEST(print_3_operand_instruction)
    fseek(tempFile, 0, SEEK_END);
    long startedAt = ftell(tempFile);

    Operand op1 = initOperand(OP_VAR, (OperandAttribute) {.var = (Variable) {.frame = GF, .name = "var_name"}});
    Operand op2 = initOperand(OP_CONST_INT64, (OperandAttribute) {.i64 = 10});
    Operand op3 = initOperand(OP_CONST_FLOAT64, (OperandAttribute) {.f64 = 10.5});
    Instruction inst;
    if (initInstr3(&inst, INST_ADD, op1, op2, op3) == -1) {
        FAIL("Failed to initialize instruction");
        return;
    }
    printInstruction(&inst, tempFile);
    destroyInstruction(&inst);

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), tempFile) == NULL) {
        FAIL("Failed to read from temporary file");
        return;
    }

    fseek(tempFile, startedAt, SEEK_SET);

    char *expected = "ADD GF@var_name int@10 float@10.5p+0\n";
    if (strcmp(buffer, expected) != 0) {
        FAIL("Expected '%s', got '%s'", expected, buffer);
    }

ENDTEST

int main() {
    // Create a temporary file
    tempFile = tmpfile();
    if (tempFile == NULL) {
        FAIL("Failed to create temporary file");
        return -1;
    }

    RUN_TESTS();

    fclose(tempFile);

    SUMMARIZE()
}