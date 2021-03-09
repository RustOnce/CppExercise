#include "TemplateFunction.h"


template void f(int);

template<typename T>
void f(T t)
{
    return;
}
