#include <iostream>
#include "FA.h"
using namespace std;
int main()
{
    unsigned int aa[] = {1, 2, 3, 4, OP_TRMNL}, bb[] = {5, 6, 7, OP_TRMNL}, cc[] = {8, 9, OP_TRMNL};
    expr a(0, aa), b(0, bb), c(0, cc);
    expr d = *(a << b) | c | ~c;
    for (unsigned int *p = d.rhs; *p != OP_TRMNL; p++)
        if (*p == OP_ALTER)
            cout << "|";
        else if (*p == OP_LPRTH)
            cout << "(";
        else if (*p == OP_RPRTH)
            cout << ")";
        else if (*p == OP_KLCLS)
            cout << "*";
        else if (*p == OP_CNCAT)
            cout << ".";
        else if (*p == OP_CMPLM)
            cout << "~";
        else
            cout << *p;
    return 0;
}