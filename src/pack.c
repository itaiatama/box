#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pack.h"

#define ARR_SIZE(a) ((sizeof(a) / sizeof(a[0])))

ValueCode PUT_ARGS[]   = {VAL_CODE_U64, VAL_CODE_I64, VAL_CODE_CHAR};
ValueCode PRINT_ARGS[] = {VAL_CODE_U64};
ValueCode FLIP_ARGS[]  = {VAL_CODE_U64};

Keyword keywords[OP_CODE_COUNT] = {
    [OP_CODE_PUT] = 
    {
        "put",
        3,
        OP_CODE_PUT,
        PUT_ARGS,
        ARR_SIZE(PUT_ARGS)
    },

    [OP_CODE_ADD] = 
    {
        "add",
        3,
        OP_CODE_ADD,
        NULL,
        0
    },

    [OP_CODE_PRINT] = 
    {
        "print", 
        5, 
        OP_CODE_PRINT, 
        PRINT_ARGS, 
        ARR_SIZE(PRINT_ARGS)
    },

    [OP_CODE_FLIP] = 
    {
        "flip" ,
        4,
        OP_CODE_FLIP,
        FLIP_ARGS, 
        ARR_SIZE(FLIP_ARGS)
    },

    [OP_CODE_HALT] = 
    {
        "halt",
        4, 
        OP_CODE_HALT,
        NULL, 
        0
    },
};

static Keyword match_kw(Token token) {
    for(u64 i = 0; i < OP_CODE_COUNT; i++) {
        Keyword kw = keywords[i];
        if(strncmp(token.ptr, kw.word, kw.size) == 0) {
            return kw;
        }
    }
    return ((Keyword){ NULL, 0, -1, NULL, 0});
}

static int type_check(Lex* lex, OpCode code, Token* token, Value* v) {
    ValueCode* codes = keywords[code].rval;
    u64 len = keywords[code].rcap;

    Token operand = lex_next(lex);
    *token = operand;

    switch(operand.code) {
        case TOKEN_CODE_U64: {
            for(u64 i = 0; i < len; i++) {
                if(codes[i] == VAL_CODE_U64) {
                    u64 data = strtoull(operand.ptr, NULL, 10);
                    *v = VAL_U64(data);
                    return 1;
                }
            }
        } break;

        case TOKEN_CODE_I64: {
            for(u64 i = 0; i < len; i++) {
                if(codes[i] == VAL_CODE_I64) {
                    i64 data = strtoll(operand.ptr, NULL, 10);
                    *v = VAL_I64(data);
                    return 1;
                }
            }
        } break;

        case TOKEN_CODE_CHAR: {
            for(u64 i = 0; i < len; i++) {
                if(codes[i] == VAL_CODE_CHAR) {
                    *v = VAL_CHAR(operand.size);
                    return 1;
                }
            }
        } break;

        default: {

        }
    }

    return 0;
}

static Op keyword(Lex* lex, Token token) {
    Keyword kw = match_kw(token);
    
    if(kw.code == -1) { 
        fprintf(stderr, "[ERROR]: Unknown keyword %.*s\n", (int) token.size, token.ptr);
        return ((Op) { .code = -1 });
    }

    switch(kw.code) {
        case OP_CODE_PUT: 
        {
            Token o;
            Value v;
            if(type_check(lex, kw.code, &o, &v) == 0) {
                fprintf(stderr, "[ERROR]: Incorrect type for [%s] keyword operand -> {%s}\n", op_as_str[kw.code], token_as_str[o.code]);
                return OP(OP_CODE_HALT, VAL_VOID());
            }
            return OP(OP_CODE_PUT, v);
        }

        case OP_CODE_ADD:  
        {
            return OP(OP_CODE_ADD, VAL_VOID());
        }

        case OP_CODE_PRINT:
        {
            Token o;
            Value v;
            if(type_check(lex, kw.code, &o, &v) == 0) {
                fprintf(stderr, "[ERROR]: Incorrect type for [%s] keyword operand -> {%s}\n", op_as_str[kw.code], token_as_str[o.code]);
                return OP(OP_CODE_HALT, VAL_VOID());
            }
            return OP(OP_CODE_PRINT, v);
        }

        case OP_CODE_FLIP: 
        {

            Token o;
            Value v;
            if(type_check(lex, kw.code, &o, &v) == 0) {
                fprintf(stderr, "[ERROR]: Incorrect type for [%s] keyword operand -> {%s}\n", op_as_str[kw.code], token_as_str[o.code]);
                return OP(OP_CODE_HALT, VAL_VOID());
            }
            return OP(OP_CODE_FLIP, v);
        }

        case OP_CODE_HALT: 
        {
            return OP(OP_CODE_HALT, VAL_VOID());
        }

        default: { assert(0 && "Unreachable"); }
    }

    return OP(OP_CODE_HALT, VAL_VOID());
}

Ops pack_ops_init(void) {
    Ops ops = {0};

    ops.cap  = 8;
    ops.data = malloc(sizeof(Op) * ops.cap);
    ops.size = 0;

    return ops;
}

void pack_ops_add(Ops* ops, Op op) {
    if(ops->size + 1 >= ops->cap) {
        ops->cap *= 2;
        ops->data = realloc(ops->data, ops->cap * sizeof(Op));
        pack_ops_add(ops, op);
        return;
    }

    ops->data[ops->size++] = op;
}

void pack_ops_free(Ops* ops) { free(ops->data); }

void pack_ops_dump(Ops* ops) {
    printf("[OPS]:\n");
    if(ops->size == 0) { printf("  [None]\n"); return; }
    for(u64 i = 0; i < ops->size; i++) {
        Op op      = ops->data[i];
        Keyword kw =  keywords[op.code];

        if(kw.rval) {
            switch(op.operand.code) {
                case VAL_CODE_U64 : printf("  [%zu]: [%s] with U64 -> {%zu}\n", i, op_as_str[op.code], op.operand.data.U64);  break;
                case VAL_CODE_I64 : printf("  [%zu]: [%s] with I64 -> {%zu}\n", i, op_as_str[op.code], op.operand.data.I64);  break;
                case VAL_CODE_CHAR: printf("  [%zu]: [%s] with CHAR -> {%d}\n", i, op_as_str[op.code], op.operand.data.CHAR); break;
                default           : printf("  [%zu]: [%s] with ? -> {%zu}\n", i, op_as_str[op.code], op.operand.data.U64);  break;
            }
            
        } else {
            printf("  [%zu]: [%s]\n", i, op_as_str[op.code]);
        }
    }
}

Ops pack_parse(Lex* lex) {
    Ops ops    = pack_ops_init();
    Token root = lex_next(lex);

    while(root.code != TOKEN_CODE_EOF) {
        if(ops.data[ops.size - 1].code == OP_CODE_HALT) { break; }
        switch(root.code) {
            case TOKEN_CODE_KEYWORD:
            {
                pack_ops_add(&ops, keyword(lex, root));
            } break;

            default: 
            {
                fprintf(stderr, "[ERROR]: Unecpected token [%s]\n", token_as_str[root.code]);
                pack_ops_free(&ops);
                return (Ops) {.data = NULL};
            } break;
        }

        root = lex_next(lex);
    }

    return ops;
}
