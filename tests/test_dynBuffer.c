//
// Created by nur on 19.10.24.
//

#include <string.h>

#include "structs/dynBuffer.h"
#include "test_utils.h"

DynBuffer buffer;

TEST(simple)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, -1);
    appendDynBuffer(&buffer, 'a');
    appendDynBuffer(&buffer, 'b');
    appendDynBuffer(&buffer, 'c');

    if (buffer.nextIdx != 3) {
        FAILCOMPI("Buffer of invalid size", 3, buffer.capacity);
        return;
    }
    if (strcmp(buffer.data, "abc") != 0) {
        FAILCOMPS("Buffer with invalid data", "abc", buffer.data);
    }
ENDTEST

TEST(buffer_expansion)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, 2);
    appendDynBuffer(&buffer, 'a');
    appendDynBuffer(&buffer, 'b');

    if (buffer.capacity != 4) {
        FAILCOMPI("Buffer of invalid capacity", 4, buffer.capacity);
    }
    if (buffer.nextIdx != 2) {
        FAILCOMPI("Buffer of invalid taken size", 2, buffer.nextIdx);
        return;
    }
    if (strcmp(buffer.data, "ab") != 0) {
        FAILCOMPS("Buffer with invalid data", "ab", buffer.data);
    }
ENDTEST

TEST(buffer_append_after_expansion)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, 2);
    appendDynBuffer(&buffer, 'a');
    appendDynBuffer(&buffer, 'b');
    appendDynBuffer(&buffer, 'c');

    if (buffer.capacity != 4) {
        FAILCOMPI("Buffer of invalid capacity", 4, buffer.capacity);
    }
    if (buffer.nextIdx != 3) {
        FAILCOMPI("Buffer of invalid taken size", 3, buffer.nextIdx);
        return;
    }
    if (strcmp(buffer.data, "abc") != 0) {
        FAILCOMPS("Buffer with invalid data", "abc", buffer.data);
    }
ENDTEST

TEST(copy_empty_buffer)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, 2);

    char *str;
    copyFromDynBuffer(&buffer, &str);

    if (strlen(str) != 0) {
        FAILCOMPI("Copied string with invalid length", 0, strlen(str));
    }
ENDTEST

TEST(copy_buffer)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, 4);
    appendDynBuffer(&buffer, 'a');
    appendDynBuffer(&buffer, 'b');
    appendDynBuffer(&buffer, 'c');

    char *str;
    copyFromDynBuffer(&buffer, &str);

    if (strcmp(str, "abc") != 0) {
        FAILCOMPS("Copied string is different", "abc", str);
    }
ENDTEST

TEST(empty_buffer)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, 4);
    appendDynBuffer(&buffer, 'a');
    appendDynBuffer(&buffer, 'b');

    emptyDynBuffer(&buffer);

    if (strlen(buffer.data) != 0) {
        FAIL("Buffer was not cleared out. Current length: %i", (int)strlen(buffer.data));
    }
    for (int i = 0; i < buffer.capacity; i++) {
        if (buffer.data[i] != '\0') {
            FAIL("Buffer was not cleared out. Non-zero at: %i", i);
        }
    }
ENDTEST

TEST(add_string_to_buffer)
    freeDynBuffer(&buffer);
    initDynBuffer(&buffer, 2);
    appendDynBuffer(&buffer, 'a');

    appendStringDynBuffer(&buffer, "bcdef");

    if (strlen(buffer.data) != 6) {
        FAILCOMPI("String was not properly appended to a buffer", 6, (int)strlen(buffer.data));
    }
    if (buffer.capacity != 7) {
        FAILCOMPI("Buffer size was not expanded properly", 7, buffer.capacity);
    }
ENDTEST

int main () {
    initDynBuffer(&buffer, 1);
    RUN_TESTS();
}
