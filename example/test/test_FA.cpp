#include "FA.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
using namespace std;
void EpsilonClosure(fa &nfa);
void print(list<vertex_t *> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i]->data.value << " ";
    cout << endl;
}
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << endl;
}
void print(list<unsigned int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << endl;
}
void print(list<list<int>> l)
{
    for (int i = 0; i < l.size(); i++)
    {
        for (int j = 0; j < l[i].size(); j++)
            cout << l[i][j] << " ";
        cout << endl;
    }
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
            cout << "\t" << nfa.g[i][j].to->data.value;
            if (nfa.g[i][j].data.value == EPSILON)
                cout << "\tE";
            else
                cout << "\t\'" << (char)(nfa.g[i][j].data.value + '0') << "\'";
        }
        cout << endl;
    }
    cout << "****************************************************************" << endl;
}
void print(dfa_table table)
{
    cout << setw(8) << "Table"
         << "|";
    for (int i = 0; i < table.get_col(); i++)
        cout << setw(8) << i;
    cout << endl;
    for (int i = 0; i < table.get_row(); i++)
    {
        if (i == table.get_start())
            if (table.is_acceptable(i))
                cout << " f s" << setw(4) << i << "|";
            else
                cout << " s" << setw(6) << i << "|";
        else if (table.is_acceptable(i))
            cout << " f" << setw(6) << i << "|";
        else
            cout << setw(8) << i << "|";
        for (int j = 0; j < table.get_col(); j++)
            if (table[i][j] == UNDEFINED)
                cout << setw(8) << "U";
            else
                cout << setw(8) << table[i][j];
        cout << endl;
    }
}
int main()
{
    fa nfa, dfa;
    char s[10000];
    cin.getline(s, 10000);
    unsigned int a[10000];
    for (int i = 0; i < (int)strlen(s); i++)
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
        case '~':
            a[i] = OP_CMPLM;
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
        case '!':
            a[i] = EPSILON;
            break;
        default:
            a[i] = s[i] - '0';
            break;
        }
    a[strlen(s)] = OP_TRMNL;
    list<expr> rl;
    rl.append(expr(0, a));
    clock_t ts = clock();
    REToNFA(rl, nfa, 0x110000);
    clock_t t1 = clock() - ts;
    print(nfa);
    ts = clock();
    NFAToDFA(nfa, dfa);
    clock_t t2 = clock() - ts;
    print(dfa);
    ts = clock();
    MinimizeDFA(dfa);
    clock_t t3 = clock() - ts;
    print(dfa);
    // unsigned int sep[] = {'0', '9' + 1, 'A', 'Z' + 1, 'a', 'z' + 1};
    // // 1 -> number, 3 -> uppercase, 5 -> lowercase
    // dfa_table t(dfa, sep);
    // print(dfa_table(dfa, sep));
    // while (!cin.fail())
    // {
    //     int ch;
    //     cin >> ch;
    //     cout << char(ch) << " " << t.next(0, ch) << endl;
    // }
    cout << "Elapsed time: " << t1 << " ms : " << t2 << "ms : " << t3 << " ms\n";
    return 0;
}
