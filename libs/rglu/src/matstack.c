#include "matstack.h"

void matstack_init(MatStack* s)
{
    R_ASSERT(s);

    STACK_INIT(&s->stack, mat4);
}

void matstack_free(MatStack* s)
{
    R_ASSERT(s);

    stack_free(&s->stack);
}

void matstack_push(MatStack* s, mat4 m)
{
    R_ASSERT(s);

    stack_push(&s->stack, m);
}

void matstack_pushTop(MatStack* s)
{
    R_ASSERT(s);

    mat4 top;
    if (matstack_top(s, top))
    {
        stack_push(&s->stack, top);
    }
}

bool matstack_pop(MatStack* s, mat4 ret)
{
    R_ASSERT(s);

    return stack_pop(&s->stack, ret);
}

bool matstack_top(MatStack* s, mat4 ret)
{
    R_ASSERT(s);

    return stack_peek(&s->stack, ret);
}

void matstack_mul(MatStack* s, mat4 m)
{
    R_ASSERT(s);

    mat4 top;
    matstack_pop(s, top);
    glm_mul(top, m, top);
    matstack_push(s, top);
}
