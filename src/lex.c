#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "lex.h"

const char* token_as_str[TOKEN_CODE_COUNT] = {
    [TOKEN_CODE_I64    ] = "I64"    ,
    [TOKEN_CODE_U64    ] = "U64"    ,
    [TOKEN_CODE_KEYWORD] = "KEYWORD",
    [TOKEN_CODE_CHAR   ] = "CHAR"   ,
    [TOKEN_CODE_EOF    ] = "EOF"    ,
};

static void move(Lex* lex) {
    if(lex->ind + 1 > lex->len) { return; }
    lex->ind +=1;
}

static char current(Lex* lex) {
    return lex->src[lex->ind];
}

static char next(Lex* lex, u64 offset) {
    return lex->src[lex->ind + offset];
}

static void skip_ws(Lex* lex) {
    char c = current(lex);
    while(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
        move(lex);
        c = current(lex);
    }
}

static void skip_comment(Lex* lex) {
    char c = current(lex);
    if(c == '#') {
        move(lex);
        while(c != '\n' && c != '\0') {
            move(lex);
            c = current(lex);
        }
        // move(lex);
        skip_ws(lex);
    }
}

static int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static int is_alpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static int is_alphanum(char c) {
    return is_alpha(c) || is_digit(c);
}

static Token number(Lex* lex) {
    u64 len = 0;
    char c = current(lex);
    while(is_digit(c) || c == 'u') { len += 1; move(lex); c = current(lex); }
    if(lex->src[lex->ind - 1] == 'u') {
        return TOKEN(TOKEN_CODE_U64, lex->src + lex->ind - len, len - 1);
    } else {
        return TOKEN(TOKEN_CODE_I64, lex->src + lex->ind - len, len);
    }
}

static Token keyword(Lex* lex) {
    u64 len = 0;
    char c = current(lex);
    while(is_alphanum(c)) { len += 1; move(lex); c = current(lex); }

    return TOKEN(TOKEN_CODE_KEYWORD, lex->src + lex->ind - len, len);
}

// TODO: Handle all errors 
static char* read(const char* path, u64* len) {
    FILE* stream = fopen(path, "rb");

    if(stream == NULL) {
        fprintf(stderr, "[ERROR]: Could not read file `%s`\n", path);
        return NULL;
    }

    fseek(stream, 0, SEEK_END);
    *len = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    char* buffer = calloc(*len + 1, sizeof(char));
    fread(buffer, sizeof(char), *len, stream);

    return buffer;
}

Lex lex_init(const char* path) {
    Lex lex = {0};
    lex.src = read(path, &lex.len);
    lex.ind = 0;

    return lex;
}

Token lex_next(Lex* lex) {
    assert(TOKEN_CODE_COUNT == 5 && "Update lex_next function inside lex.c");

    LEX_NEXT_START:
    skip_ws(lex);

    char c = current(lex);
    if(is_digit(c)) { return number(lex);  }
    if(is_alpha(c)) { return keyword(lex); }

    switch(c) {

        case '\0':
        {
            return TOKEN(TOKEN_CODE_EOF, NULL, 0);
        } break;

        case '\'':
        {
            if(next(lex, 1) == '\\') {

                char z = next(lex, 2);
                move(lex);
                move(lex);
                move(lex);
                move(lex);

                switch(z) {
                    case 'n': return TOKEN(TOKEN_CODE_CHAR, lex->src + lex->ind - 3, '\n');
                    default : return TOKEN(TOKEN_CODE_CHAR, lex->src + lex->ind - 3, '\0');
                }
            } else if(next(lex, 2) == '\'') {
                char z = next(lex, 1);
                move(lex);
                move(lex);
                move(lex);
                char b[32] = {0};
                sprintf(b, "%d", z);
                int v = strtol(b, NULL, 10);
                return TOKEN(TOKEN_CODE_CHAR, lex->src + lex->ind - 2, v);
            }

            fprintf(stderr, "[ERROR]: Unexpected opened single quote\n");
            return TOKEN(TOKEN_CODE_EOF, NULL, 0);
        } break;

        case '#':
        {
            skip_comment(lex);
            goto LEX_NEXT_START;
        } break;

        default:
        {
            fprintf(stderr, "[ERROR]: Unexpected charecter `%c`\n", c);
            return TOKEN(TOKEN_CODE_EOF, NULL, 0);
        }
    }
}

Token lex_next_expected(Lex* lex, TokenCode expect) {
    Token token = lex_next(lex);
    if(token.code != expect) {
        fprintf(stderr, "[ERROR]: Unexpected token code [%s] -> {%.*s}, expected code [%s]\n", token_as_str[token.code], (int) token.size, token.ptr, token_as_str[expect]);
        return TOKEN(TOKEN_CODE_EOF, NULL, 0);
    }

    return token;    
}

void lex_dump(Lex* lex) {
    Token token = lex_next(lex);
    printf("[LEXER]:\n");
    while(token.code != TOKEN_CODE_EOF) {
        if(token.code == TOKEN_CODE_CHAR) {
            printf("  [%s] -> {%zu}\n", token_as_str[token.code], token.size);
        } else {
            printf("  [%s] -> {%.*s}\n", token_as_str[token.code], (int) token.size, token.ptr);
        }
        token = lex_next(lex);
    }
}

void lex_rewind(Lex* lex) { lex->ind = 0; }
