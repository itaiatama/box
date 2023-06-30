#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "box.h"

typedef struct {
    u64   cap;
    u64   len;
    char* val;
} StrBuf;

const char* err_as_str[ERR_CODE_COUNT] = {
    [ERR_CODE_NONE             ] = "No errors"                                                                           ,
    [ERR_CODE_STACK_OVERFLOW   ] = "%s Data stack is overflow on operation [#%zu | %s]\n"                                ,
    [ERR_CODE_STACK_UNDERFLOW  ] = "%s Data stack is underflow on operation [#%zu | %s]\n"                               ,
    [ERR_CODE_TYPE_MISSMATCH   ] = "%s Could not execute operation [#%zu | %s], because types miss-match. [%s] != [%s]\n",
    [ERR_CODE_UNKNOWN_OPERATION] = "%s Could not execute operation #%zu, because operation code [%zu] is unknown\n"      ,
    [ERR_CODE_INVALID_TYPE     ] = "%s Could not execute operation [#%zu | %s] with type [%s]\n"                         ,
    [ERR_CODE_INVALID_OPERAND  ] = "%s Could not execute operation [#%zu | %s] with operand type [%s]\n"                 ,
    [ERR_CODE_UNKNOWN_TYPE     ] = "%s Could not execute operation #%zu, because value code [%zu] is unknown\n"          ,
};

const char* op_as_str[OP_CODE_COUNT] = {
    [OP_CODE_PUT  ] = "PUT" , 
    [OP_CODE_ADD  ] = "ADD"  ,  
    [OP_CODE_PRINT] = "PRINT",
    [OP_CODE_FLIP ] = "FLIP" ,
    [OP_CODE_HALT ] = "HALT" , 
};

static ErrCode add(Box* box) {
    assert(VAL_CODE_COUNT == 4 && "Update add function inside box.c");
    if(box->ip < 2) { return ERR_CODE_STACK_UNDERFLOW; }

    Value a = box->stack[box->ip - 2];
    Value b = box->stack[box->ip - 1];

    if(a.code != b.code) { return ERR_CODE_TYPE_MISSMATCH; }

    switch(a.code) {
        case VAL_CODE_I64 : { box->stack[box->ip - 2].data.I64 = a.data.I64 + b.data.I64; } break;
        case VAL_CODE_U64 : { box->stack[box->ip - 2].data.U64 = a.data.U64 + b.data.U64; } break;

        case VAL_CODE_CHAR: { return ERR_CODE_INVALID_TYPE;  }
        default           : { return ERR_CODE_UNKNOWN_TYPE; }
    }

    box->ip -= 1;
    return ERR_CODE_NONE;
}

static StrBuf strbuf_init(void) {
    StrBuf buf = {0};
    buf.cap = 8;
    buf.len = 0;
    buf.val = calloc(buf.cap, sizeof(char));
    return buf;
}

static void strbuff_add(StrBuf* buf, char* val, u64 len) {
    if(buf->len + len >= buf->cap) {
        buf->cap *= 2;
        buf->val = realloc(buf->val, buf->cap * (sizeof(char)));
        strbuff_add(buf, val, len);
        return;
    }

    memcpy(buf->val + buf->len, val, len);
    buf->len += len;
}

static void strbuff_free(StrBuf* buf) { free(buf->val); }

ErrCode box_exec_op(Box* box, Op op) {
    assert(OP_CODE_COUNT == 5 && "Update box_exec_op function inside box.c");
    box->last = op;

    switch(op.code) {
        case OP_CODE_PUT:
        {
            if(box->ip >= BOX_STACK_CAP) { return ERR_CODE_STACK_OVERFLOW; }
            box->stack[box->ip++] = op.operand;
        } break;

        case OP_CODE_ADD:
        {
            ErrCode code = add(box);
            if(code != ERR_CODE_NONE) { return code; }
        } break;

        case OP_CODE_PRINT:
        {
            if(op.operand.code != VAL_CODE_U64) { return ERR_CODE_INVALID_OPERAND; }
            u64 times = op.operand.data.U64;

            if(box->ip < times) { return ERR_CODE_STACK_UNDERFLOW; }

            StrBuf buf = strbuf_init();
            for(u64 i = 0; i < times; i++) {
                Value v = box->stack[box->ip - 1];
                if(!value_can_print(v)) {
                    strbuff_free(&buf);
                    return ERR_CODE_INVALID_TYPE;
                }

                u64 len = 0;
                char* data = value_data(v, &len);
                strbuff_add(&buf, data, len);

                free(data);
                box->ip -= 1;
            }

            printf("%.*s", (int) buf.len, buf.val);
            strbuff_free(&buf);
        } break;

        case OP_CODE_FLIP:
        {
            if(op.operand.code != VAL_CODE_U64) { return ERR_CODE_INVALID_OPERAND; }
            u64 size = op.operand.data.U64 - 1;

            Value tmp = {0};
            for(u64 i = 0; i < size; i++) {
                tmp = box->stack[i];
                box->stack[i] = box->stack[size];
                box->stack[size] = tmp;
                size -= 1;
            }
        } break;

        case OP_CODE_HALT: {} break;
        default          : { return ERR_CODE_UNKNOWN_OPERATION; }
    }

    return ERR_CODE_NONE;
}

ErrCode box_exec(Box* box, Op ops[]) {
    for(u64 i = 0; ops[i].code != OP_CODE_HALT; i++) {
        ErrCode code = box_exec_op(box, ops[i]);
        if(code != ERR_CODE_NONE) { return code; }
    }

    box_exec_op(box, OP(OP_CODE_HALT, VAL_VOID()));
    return ERR_CODE_NONE;
}

void box_dump(Box* box) {
    printf("[Stack]:\n");
    if(box->ip == 0) { printf("  [None]\n"); }

    for(i64 i = box->ip - 1; i >= 0; i--) {
        printf("  [%zu]: ", i);
        value_dump(box->stack[i]);
    }
}

void box_report (Box* box, ErrCode code) {
    switch(code) {
        case ERR_CODE_STACK_OVERFLOW: 
        {
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, op_as_str[box->last.code]);
        } break;

        case ERR_CODE_STACK_UNDERFLOW: 
        {
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, op_as_str[box->last.code]);
        } break;

        case ERR_CODE_TYPE_MISSMATCH:
        {
            Value a = box->stack[box->ip - 2];
            Value b = box->stack[box->ip - 1];
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, op_as_str[box->last.code], val_as_str[a.code], val_as_str[b.code]);
        } break;

        case ERR_CODE_UNKNOWN_OPERATION:
        {
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, box->last.code);
        } break;

        case ERR_CODE_INVALID_TYPE:
        {
            Value a = box->stack[box->ip - 1];
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, op_as_str[box->last.code], val_as_str[a.code]);
        } break;

        case ERR_CODE_INVALID_OPERAND:
        {
            Value a = box->last.operand;
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, op_as_str[box->last.code], val_as_str[a.code]);
        } break;

        case ERR_CODE_UNKNOWN_TYPE:
        {
            Value a = box->stack[box->ip - 1];
            fprintf(stderr, err_as_str[code], "[ERROR]:", box->ip, a.code);
        } break;

        default: fprintf(stderr, "[ERROR]: %s\n", err_as_str[code]); break;  
    }
}
