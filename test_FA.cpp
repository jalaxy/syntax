#include "FA.h"
#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;
void EpsilonClosure(fa &nfa);
void print(list<vertex_t *> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i]->data.value << " ";
    cout << endl;
}
void print(fa &nfa)
{
    cout << "sigma:\t[0, " << nfa.sigma_range << ")\n";
    cout << "start:\t" << nfa.s << "\n";
    cout << "final:\t";
    for (int i = 0; i < nfa.f.size(); i++)
        cout << nfa.f[i] << " ";
    cout << "\n";
    cout << "graph:\t" << endl;
    cout << "\tvertex\t\t\ttoken\t\tedges\n";
    for (int i = 0; i < nfa.g.size(); i++)
    {
        cout << "\t" << nfa.g[i].data.value << "\t" << &nfa.g[i];
        if (nfa.g[i].data.token == NON_TERMINAL)
            cout << "\t";
        else
            cout << "\t" << nfa.g[i].data.token;
        for (int j = 0; j < nfa.g[i].size(); j++)
        {
            cout << "\t" << nfa.g[i][j].get_to()->data.value;
            if (nfa.g[i][j].data.value == EPSILON)
                cout << "\tE";
            else
                cout << "\t'" << (char)nfa.g[i][j].data.value << "'";
        }
        cout << endl;
    }
    cout << "****************************************************************" << endl;
}
int main()
{
    fa nfa, dfa;
    char s[10000];
    cin.getline(s, 10000);
    unsigned int a[10000];
    for (int i = 0; i < strlen(s); i++)
        switch (s[i])
        {
        case ' ':
        case '\t':
            break;
        case '|':
            a[i] = OP_ALTER;
            break;
        case '*':
            a[i] = OP_KLCLS;
            break;
        case '.':
            a[i] = OP_CNCAT;
            break;
        case '(':
            a[i] = OP_LPRTH;
            break;
        case ')':
            a[i] = OP_RPRTH;
            break;
        default:
            a[i] = s[i];
            break;
        }
    a[strlen(s)] = OP_RPRTH;
    list<re> rl;
    rl.append(re(a, 0));
    clock_t ts = clock();
    REToNFA(rl, nfa);
    clock_t t1 = clock() - ts;
    print(nfa);
    ts = clock();
    NFAToDFA(nfa, dfa);
    clock_t t2 = clock() - ts;
    print(dfa);
    cout << "Elapsed time: " << t1 << " ms : " << t2 << " ms\n";
    return 0;
}
