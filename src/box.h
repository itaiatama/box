#ifndef  BOX_H
#define  BOX_H

#include "type.h"
#include "value.h"

#define BOX_STACK_CAP 512

#define OP(C, V) ((Op) {.code = C, .operand = V})

typedef enum {
    OP_CODE_PUT  ,
    OP_CODE_ADD  ,
    OP_CODE_PRINT,
    OP_CODE_FLIP ,
    OP_CODE_HALT ,
    OP_CODE_COUNT,
} OpCode;

typedef struct {
    OpCode  code;
    Value operand;
} Op;

typedef enum {
    ERR_CODE_NONE             ,
    ERR_CODE_STACK_OVERFLOW   ,
    ERR_CODE_STACK_UNDERFLOW  ,
    ERR_CODE_UNKNOWN_OPERATION,
    ERR_CODE_UNKNOWN_TYPE     ,
    ERR_CODE_TYPE_MISSMATCH   ,
    ERR_CODE_INVALID_TYPE     ,
    ERR_CODE_INVALID_OPERAND  ,
    ERR_CODE_COUNT            ,
} ErrCode;

typedef struct {
    u64   ip;
    Value stack[BOX_STACK_CAP];
    Op    last;
} Box;

extern const char* err_as_str[ERR_CODE_COUNT];
extern const char* op_as_str[OP_CODE_COUNT];

void    box_dump   (Box* box);
ErrCode box_exec_op(Box* box, Op op);
ErrCode box_exec   (Box* box, Op ops[]);
void    box_report (Box* box, ErrCode code);

#endif //BOX_H
