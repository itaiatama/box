#ifndef  BOX_PACK_H
#define  BOX_PACK_H

#include "type.h"
#include "box.h"
#include "lex.h"

typedef struct {
    const char* word;
    u64         size;
    OpCode      code;
    ValueCode*  rval;
    u64         rcap;
} Keyword;

typedef struct {
    u64  cap;
    u64 size;
    Op* data;
} Ops;

extern Keyword keywords[OP_CODE_COUNT];

Ops  pack_ops_init(void);
void pack_ops_add (Ops* ops, Op op);
void pack_ops_free(Ops* ops);
void pack_ops_dump(Ops* ops);
Ops  pack_parse   (Lex* lex);

#endif //BOX_PACK_H
