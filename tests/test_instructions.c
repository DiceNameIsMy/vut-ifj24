#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "target_gen/instructions.h"

#include "test_utils.h"

FILE *tempFile = NULL;

char* getTempFileContent() {
    // Flush any pending writes
    fflush(tempFile);

    // Get file size
    fseek(tempFile, 0, SEEK_END);
    const long size = ftell(tempFile);

    // Allocate buffer for content
    char* buffer = malloc(size + 1);
    if (!buffer) return NULL;

    // Read content
    fseek(tempFile, 0, SEEK_SET);
    const size_t read = fread(buffer, 1, size, tempFile);
    buffer[read] = '\0';

    return buffer;
}

TEST(print_0_operand_instruction)
    tempFile = tmpfile();
    if (tempFile == NULL) {
        FAIL("Failed to create temporary file");
        return;
    }

    Instruction inst = initInstr0(INST_RETURN);
    printInstruction(&inst, tempFile);
    destroyInstruction(&inst);

    char *output = getTempFileContent();
    fclose(tempFile);
    tempFile = NULL;

    char *expected = "RETURN\n";
    if (strcmp(output, expected) != 0) {
        FAIL("Expected '%s', got '%s'", expected, output);
    }
    free(output);

ENDTEST

TEST(print_3_operand_instruction)
    tempFile = tmpfile();
    if (tempFile == NULL) {
        FAIL("Failed to create temporary file");
        return;
    }

    OperandAttribute varAttr = initVarAttr(FRAME_GF, "var_name");
    Operand op1 = initOperand(OP_VAR, varAttr);
    Operand op2 = initOperand(OP_CONST_INT64, (OperandAttribute) {.i64 = 10});
    Operand op3 = initOperand(OP_CONST_FLOAT64, (OperandAttribute) {.f64 = 10.5});
    Instruction inst = initInstr3(INST_ADD, op1, op2, op3);
    printInstruction(&inst, tempFile);
    destroyInstruction(&inst);

    char *output = getTempFileContent();
    fclose(tempFile);
    tempFile = NULL;

    char *expected = "ADD GF@var_name int@10 float@10.5p+0\n";
    if (strcmp(output, expected) != 0) {
        FAIL("Expected '%s', got '%s'", expected, output);
    }
    free(output);

ENDTEST

int main() {
    RUN_TESTS();

    SUMMARIZE()
}