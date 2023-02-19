#include <string.h>
#include "doctest.h"
#include "stack.hpp"



TEST_CASE('stack functions'){

    sp st ;
    init_stack(st);
    char answer[1024];
    CHECK_NOTHROW(push(st, "hi"));
    CHECK_NOTHROW(push(st, "hi1"));
    CHECK_NOTHROW(push(st, "hi2"));
    //here the stack grows
    CHECK_NOTHROW(push(st, "hi3"));
    CHECK_NOTHROW(push(st, "hi4"));
    CHECK_NOTHROW(push(st, "hi5"));


    //pop
    CHECK_NOTHROW(pop(st, answer));
    CHECK_NOTHROW(pop(st, answer));
    CHECK_NOTHROW(pop(st, answer));
    CHECK_NOTHROW(pop(st, answer));

    //Peek
    CHECK_NOTHROW(peek(st, answer));
}

