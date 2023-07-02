#ifndef  BOX_PACK_H
#define  BOX_PACK_H

#include "type.h"
#include "box.h"
#include "lex.h"

#define CONST(n, l, v) ((Const){n, l, v})

typedef struct {
    const char* word;
    u64         size;
    OpCode      code;
    ValueCode*  rval;
    u64         rcap;
} Keyword;

typedef enum {
    PREP_CODE_CONST,
    PREP_CODE_COUNT,
} PrepCode;

typedef struct {
    const char* word;
    u64         size;
    PrepCode    code;
} Prep;

typedef struct {
    char* name;
    u64   len;
    Value val;
} Const;

typedef struct {
    Const* data;
    u64    size;
    u64    cap;
} ConstPool;

typedef struct {
    u64        cap;
    u64        size;
    Op*        data;
    ConstPool consts;
} Ops;

extern Keyword keywords[OP_CODE_COUNT];
extern Prep    preps   [PREP_CODE_COUNT];

Ops       pack_ops_init   (void);
void      pack_ops_add    (Ops* ops, Op op);
void      pack_ops_free   (Ops* ops);
void      pack_ops_dump   (Ops* ops);

ConstPool pack_consts_init(void);
void      pack_consts_add (Ops* ops, Const c);
void      pack_consts_free(Ops* ops);
void      pack_consts_dump(Ops* ops);

Ops       pack_parse      (Lex* lex);

#endif //BOX_PACK_H
