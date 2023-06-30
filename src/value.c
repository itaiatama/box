#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

const char* val_as_str[VAL_CODE_COUNT] = {
    [VAL_CODE_VOID] = "VOID",
    [VAL_CODE_I64 ] = "I64" ,
    [VAL_CODE_U64 ] = "U64" ,
    [VAL_CODE_CHAR] = "CHAR",
};

void value_dump(Value v) {
    assert(VAL_CODE_COUNT == 4 && "Update value_dump function inside value.c");

    switch(v.code) {
        case VAL_CODE_VOID: printf("{VOID} [-]\n");               break;
        case VAL_CODE_I64 : printf("{I64 } `%zu`\n", v.data.I64); break;
        case VAL_CODE_U64 : printf("{U64 } `%zu`\n", v.data.U64); break;
        case VAL_CODE_CHAR: printf("{CHAR} `%d`\n", v.data.CHAR); break;
        default: return;
    }
}

char* value_data(Value v, u64* size) {
    assert(VAL_CODE_COUNT == 4 && "Update value_data function inside value.c");

    char* buffer = calloc(sizeof(char), 20);

    switch(v.code) {
        case VAL_CODE_I64 : sprintf(buffer, "%zu", v.data.I64 ); *size = strlen(buffer); break;
        case VAL_CODE_U64 : sprintf(buffer, "%zu", v.data.U64 ); *size = strlen(buffer); break;
        case VAL_CODE_CHAR: sprintf(buffer, "%c" , v.data.CHAR); *size = strlen(buffer); break;
        default: return NULL;
    }

    return buffer;
}

void value_append(char* str, Value v) {
    assert(VAL_CODE_COUNT == 4 && "Update value_append function inside value.c");

    switch(v.code) {
        case VAL_CODE_I64 : sprintf(str, "%s%zu", str, v.data.I64);  break;
        case VAL_CODE_U64 : sprintf(str, "%s%zu", str, v.data.U64);  break;
        case VAL_CODE_CHAR: sprintf(str, "%s%c" , str, v.data.CHAR); break;
        default: return;
    }
}

int value_can_print(Value v) {
    assert(VAL_CODE_COUNT == 4 && "Update value_can_print function inside value.c");
    return v.code ==  VAL_CODE_I64 || v.code ==  VAL_CODE_U64 || v.code == VAL_CODE_CHAR;
}
