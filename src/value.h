#ifndef  BOX_VALUE_H
#define  BOX_VALUE_H

#include <assert.h>

#include "type.h"

#define VAL_VOID()  ((Value) {.code = VAL_CODE_VOID})
#define VAL_I64(V)  ((Value) {.code = VAL_CODE_I64 , .data = {.I64  = V}})
#define VAL_U64(V)  ((Value) {.code = VAL_CODE_U64 , .data = {.U64  = V}})
#define VAL_CHAR(V) ((Value) {.code = VAL_CODE_CHAR, .data = {.CHAR = V}})

typedef enum {
    VAL_CODE_VOID ,
    VAL_CODE_I64  ,
    VAL_CODE_U64  ,
    VAL_CODE_CHAR ,
    VAL_CODE_COUNT,
} ValueCode;

typedef struct {
    ValueCode code;
    union {
        i64  I64;
        u64  U64;
        char CHAR;
    } data;
} Value;

extern const char* val_as_str[VAL_CODE_COUNT];

void  value_dump     (Value v);
char* value_data     (Value v, u64* size);
void  value_append   (char* str, Value v);
int   value_can_print(Value v);

#endif //BOX_VALUE_H
