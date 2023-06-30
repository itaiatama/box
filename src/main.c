#include <stdio.h>
#include <stdlib.h>

#include "box.h"
#include "lex.h"
#include "pack.h"

int main(void) {

    Lex lex = lex_init("./example/hello.box");
    lex_dump(&lex);
    lex_rewind(&lex);
    Ops ops = pack_parse(&lex);
    pack_ops_dump(&ops);

    Box box = {0};
    ErrCode code = box_exec(&box, ops.data);
    if(code != ERR_CODE_NONE) {
      box_report(&box, code);
      return 1;
    }
    box_dump(&box);

    return 0;
}
