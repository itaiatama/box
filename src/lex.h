#ifndef  BOX_LEX_H
#define  BOX_LEX_H

#include "type.h"

#define TOKEN(c, p, s) ((Token) {c, p, s})

typedef enum {
    TOKEN_CODE_I64    ,
    TOKEN_CODE_U64    ,
    TOKEN_CODE_KEYWORD,
    TOKEN_CODE_CHAR   ,
    TOKEN_CODE_EOF    ,
    TOKEN_CODE_COUNT  ,
} TokenCode;

typedef struct {
    TokenCode code;
    char*     ptr;
    u64       size;
} Token;

typedef struct {
    char* src;
    u64   ind;
    u64   len;
} Lex;

extern const char* token_as_str[TOKEN_CODE_COUNT];

Lex   lex_init         (const char* path);
Token lex_next         (Lex* lex);
Token lex_next_expected(Lex* lex, TokenCode expect);
void  lex_dump         (Lex* lex);
void  lex_rewind       (Lex* lex);

#endif //BOX_LEX_H
