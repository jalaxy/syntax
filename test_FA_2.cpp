#include <cstdio>
#include "FA.h"
void print(fa nfa)
{
    printf("sigma:\t[0, %u)\nstart:\t0x%x\nfinal:\t", nfa.sigma_range, (unsigned int)(long long)nfa.s);
    for (int i = 0; i < nfa.f.size(); i++)
        printf("0x%x ", (unsigned int)(long long)nfa.f[i]);
    printf("\ngraph:\t\n\tvertex\t\t\ttoken\t\tedges\n");
    for (int i = 0; i < nfa.g.size(); i++)
    {
        printf("\t%u\t0x%x", nfa.g[i].data.value, (unsigned int)(long long)&nfa.g[i]);
        if (nfa.g[i].data.output.empty())
            printf("\t");
        else
            printf("\t%u", nfa.g[i].data.output.front());
        for (int j = 0; j < nfa.g[i].size(); j++)
        {
            printf("\t%u", nfa.g[i][j].to->data.value);
            if (nfa.g[i][j].data.value == EPSILON)
                printf("\tE");
            else
                printf("\t%u", nfa.g[i][j].data.value);
        }
        printf("\n");
    }
    printf("****************************************************************\n");
}
int main()
{
    list<expr> lexp;
    lexp.append({0, list<unsigned int>()});
    lexp.top().rhs.append(0);
    lexp.top().rhs.append(OP_KLCLS);
    lexp.top().rhs.append(OP_ALTER);
    lexp.top().rhs.append(2);
    lexp.top().rhs.append(OP_CNCAT);
    lexp.top().rhs.append(1);
    fa nfa, dfa;
    REToNFA(lexp, nfa);
    print(nfa);
    NFAToDFA(nfa, dfa);
    print(dfa);
}