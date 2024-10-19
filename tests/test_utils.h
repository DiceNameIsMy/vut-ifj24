//
// Created by nur on 12.10.24.
//

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>

int failure_count = 0;

typedef void (*test_case_fn_t)(void);

// Store up to 100 tests. Run them with RUN_TESTS macro.
static test_case_fn_t test_registry[100];
static int test_count = 0;


// Add a function to the test_registry.
#define REGISTER_TEST(NAME)                                                \
void register_##NAME(void) __attribute__((constructor));                   \
void register_##NAME(void) {                                               \
    test_registry[test_count++] = NAME;                                    \
}

#define TEST(NAME)                                                         \
void NAME(void);                                                           \
REGISTER_TEST(NAME)                                                        \
void NAME(void) {                                                          \
    int failures_before = failure_count;                                   \
    printf("\n[%s]\n", #NAME);                                             \

#define ENDTEST                                                            \
    if (failure_count == failures_before)                                  \
        printf("\033[32m[Success]\033[0m\n");                              \
}

#define FAIL(FMT, ...) failure_count++;                                    \
printf("\033[0;31m[FAILED]\033[0m ");                                      \
printf(FMT, ##__VA_ARGS__);                                                \
printf("\n")

#define FAILCOMPI(msg, expected, got) failure_count++;                     \
printf("\033[0;31m[FAILED]\033[0m ");                                      \
printf("%s. Expected: '%i', Got: '%i'", msg, expected, got);               \
printf("\n")

#define FAILCOMPF(msg, expected, got) failure_count++;                     \
printf("\033[0;31m[FAILED]\033[0m ");                                      \
printf("%s. Expected: '%f', Got: '%f'", msg, expected, got);               \
printf("\n")

#define FAILCOMPS(msg, expected, got) failure_count++;                     \
printf("\033[0;31m[FAILED]\033[0m ");                                      \
printf("%s. Expected: '%s', Got: '%s'", msg, expected, got);               \
printf("\n")


// Run all tests accumulated in the test registry
// TODO: Print the result of running the tests
#define RUN_TESTS()                                                        \
for (int i = 0; i < test_count; i++) {                                     \
    test_registry[i]();                                                    \
}

#endif //TEST_UTILS_H
