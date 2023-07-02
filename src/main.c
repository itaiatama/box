#include <stdio.h>
#include <stdlib.h>

#include "box.h"
#include "lex.h"
#include "pack.h"

int main(void) {

    Lex lex = lex_init("./example/hello.box");
    lex_dump(&lex);
    printf("--------------------\n");
    lex_rewind(&lex);
    Ops ops = pack_parse(&lex);
    pack_consts_dump(&ops);
    printf("--------------------\n");
    pack_ops_dump(&ops);
    printf("--------------------\n");

    Box box = {0};
    ErrCode code = box_exec(&box, ops.data);
    if(code != ERR_CODE_NONE) {
      box_report(&box, code);
      return 1;
    }
    printf("--------------------\n");
    box_dump(&box);
    printf("--------------------\n");

    pack_consts_free(&ops);
    pack_ops_free(&ops);

    return 0;
}
