#ifndef VUT_TYPES_H
#define VUT_TYPES_H

typedef enum {NULL_LITERAL, STR_LITERAL, I32_LITERAL, F64_LITERAL, U8_ARRAY, I32, F64, U8_ARRAY_NULLABLE, 
              I32_NULLABLE, F64_NULLABLE, BOOL, FUNCTION, UNDEFINED, NONE} type_t;

typedef union CTValue{
              char *string;
              int integer;
              double real;             
} CTValue;
#endif
