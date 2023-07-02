#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pack.h"

#define ARR_SIZE(a) ((sizeof(a) / sizeof(a[0])))
#define MIN(a, b) (a > b ? b : a)

ValueCode PUT_ARGS[]   = {VAL_CODE_U64, VAL_CODE_I64, VAL_CODE_CHAR};
ValueCode PRINT_ARGS[] = {VAL_CODE_U64};
ValueCode FLIP_ARGS[]  = {VAL_CODE_U64};

ValueCode ANY_ARGS[] = {VAL_CODE_VOID, VAL_CODE_U64, VAL_CODE_I64, VAL_CODE_CHAR};

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

Prep preps[PREP_CODE_COUNT] = {
    [PREP_CODE_CONST] = { "const", 5, PREP_CODE_CONST},
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

static Prep match_prep(Token token) {
        for(u64 i = 0; i < PREP_CODE_COUNT; i++) {
        Prep prep = preps[i];
        if(strncmp(token.ptr, prep.word, prep.size) == 0) {
            return prep;
        }
    }
    return ((Prep){ NULL, 0, -1 });
}

static Const find_const(Ops* ops, char* label, u64 size) {
    for(u64 i = 0; i < ops->consts.size; i++) {
        Const c = ops->consts.data[i];
        if(strncmp(label, c.name, MIN(size, c.len)) == 0) {
            return c;
        }
    }

    return (Const) {.len = 0};
}

static int type_check(Ops* ops, Lex* lex, ValueCode* codes, u64 len, Token* token, Value* v) {
    Token operand = lex_next(lex);
    *token = operand;

    if(token->code == TOKEN_CODE_WORD) {
        Const c = find_const(ops, token->ptr, token->size);
        if(c.len != 0) { *v = c.val; return 1; }
        else {
            return 0;
        }
    }

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

static void parse_prep(Ops* ops, Lex* lex, Token token) {
    Prep prep = match_prep(token);

    if(prep.code == -1) {
        fprintf(stderr, "[ERROR]: Unknown preprocessor command %.*s\n", (int) token.size, token.ptr);
        return;
    }

    switch(prep.code) {
        case PREP_CODE_CONST:
        {
            Token label = lex_next_expected(lex, TOKEN_CODE_WORD);
            lex_next_expected(lex, TOKEN_CODE_EQUAL);
            Token o;
            Value v;
            if(type_check(ops, lex, ANY_ARGS, VAL_CODE_COUNT, &o, &v) == 0) { assert(0 && "Unreachable"); }

            pack_consts_add(ops, CONST(label.ptr, label.size, v));
        } break;

        default: { assert(0 && "Unreachable"); }
    }
}

static Op keyword(Ops* ops,Lex* lex, Token token) {
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
            if(type_check(ops, lex, kw.rval, kw.rcap, &o, &v) == 0) {
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
            if(type_check(ops, lex, kw.rval, kw.rcap, &o, &v) == 0) {
                fprintf(stderr, "[ERROR]: Incorrect type for [%s] keyword operand -> {%s}\n", op_as_str[kw.code], token_as_str[o.code]);
                return OP(OP_CODE_HALT, VAL_VOID());
            }
            return OP(OP_CODE_PRINT, v);
        }

        case OP_CODE_FLIP: 
        {

            Token o;
            Value v;
            if(type_check(ops, lex, kw.rval, kw.rcap, &o, &v) == 0) {
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

    ops.cap    = 8;
    ops.data   = malloc(sizeof(Op) * ops.cap);
    ops.size   = 0;

    ops.consts = pack_consts_init();

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

ConstPool pack_consts_init(void) {
    ConstPool pool = {0};
    pool.cap  = 8;
    pool.size = 0;
    pool.data = calloc(pool.cap, sizeof(Const));
    return pool;
}

void pack_consts_add(Ops* ops, Const c) {
    ConstPool* pool = &ops->consts;
    if(pool->size + 1 >= pool->cap) {
        pool->cap *= 2;
        pool->data = realloc(pool->data, pool->cap * sizeof(Const));
        pack_consts_add(ops, c);
        return;
    }

    pool->data[pool->size++] = c;
}

void pack_consts_free(Ops* ops) { free(ops->consts.data); }

void pack_consts_dump(Ops* ops) {
    ConstPool* pool = &ops->consts;

    printf("[CONSTS]:\n");
    if(pool->size == 0) { printf("  [None]\n"); return; }
    for(u64 i = 0; i < pool->size; i++) {
        Const c = pool->data[i];

        switch(c.val.code) {
            case VAL_CODE_U64 : printf("  [%zu]: CONST [%.*s] with U64 -> {%zu}\n", i, (int) c.len, c.name, c.val.data.U64);  break;
            case VAL_CODE_I64 : printf("  [%zu]: CONST [%.*s] with I64 -> {%zu}\n", i, (int) c.len, c.name, c.val.data.I64);  break;
            case VAL_CODE_CHAR: printf("  [%zu]: CONST [%.*s] with CHAR -> {%d}\n", i, (int) c.len, c.name, c.val.data.CHAR); break;
            default           : printf("  [%zu]: CONST [%.*s] with ? -> {%zu}\n", i  , (int) c.len, c.name, c.val.data.U64);  break;
        }
    }
}

Ops pack_parse(Lex* lex) {
    Ops ops    = pack_ops_init();
    Token root = lex_next(lex);

    while(root.code != TOKEN_CODE_EOF) {
        if(ops.data[ops.size - 1].code == OP_CODE_HALT) { break; }

        switch(root.code) {
            case TOKEN_CODE_PREP:
            {
                parse_prep(&ops, lex, root);
            }break;

            case TOKEN_CODE_WORD:
            {
                pack_ops_add(&ops, keyword(&ops, lex, root));
            } break;

            default: 
            {
                fprintf(stderr, "[ERROR]: Unexpected token [%s]\n", token_as_str[root.code]);
                pack_ops_free(&ops);
                return (Ops) {.data = NULL};
            } break;
        }

        root = lex_next(lex);
    }

    return ops;
}
