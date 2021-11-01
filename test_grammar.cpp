#include <iostream>
#include <iomanip>
#include "src/grammar.h"
using namespace std;
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << endl;
}
void print(list<unsigned int> l)
{
    for (int i = 0; i < l.size(); i++)
        if (l[i] == EPSILON)
            cout << "#E ";
        else
            cout << l[i] << " ";
    cout << endl;
}
void print(list<unsigned int> *l, int n)
{
    for (int m = 0; m < n; m++)
    {
        for (int i = 0; i < l[m].size(); i++)
            if (l[m][i] == EPSILON)
                cout << "#E ";
            else
                cout << l[m][i] << " ";
        cout << endl;
    }
    cout << endl;
}
void print(expr e)
{
    cout << e.lhs << " = ";
    for (unsigned int *p = e.rhs; *p != OP_TRMNL; p++)
        switch (*p)
        {
        case OP_ALTER:
            cout << "|";
            break;
        case OP_CMPLM:
            cout << "~";
            break;
        case OP_CNCAT:
            cout << ".";
            break;
        case OP_KLCLS:
            cout << "*";
            break;
        case OP_LPRTH:
            cout << "(";
            break;
        case OP_MINUS:
            cout << "-";
            break;
        case OP_OPTNL:
            cout << "?";
            break;
        case OP_PTCLS:
            cout << "+";
            break;
        case OP_RPRTH:
            cout << ")";
            break;
        case OP_TRMNL:
            cout << "#";
            break;
        case EPSILON:
            cout << "#E";
            break;
        default:
            cout << *p;
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
void print(list<expr> l)
{
    for (int i = 0; i < l.size(); i++)
        print(l[i]);
}
void print(grammar &g)
{
    for (int i = 0; i < g.productions.size(); i++)
    {
        cout << g.productions[i].variable << " = ";
        for (int j = 0; j < g.productions[i].expression.size(); j++)
        {
            cout << (j == 0 ? "" : "| ");
            if (g.productions[i].expression[j].size() == 0)
                cout << "#E ";
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
                cout << g.productions[i].expression[j][k] << " ";
        }
        cout << endl;
    }
    cout << endl;
}
void print(ll1_parsing_table table)
{
    cout << setw(8) << "Table"
         << "|";
    for (int i = 0; i < table.get_col(); i++)
        cout << setw(8) << i;
    cout << endl;
    for (int i = 0; i < table.get_row(); i++)
    {
        cout << setw(8) << i;
        for (int j = 0; j < table.get_col(); j++)
        {
            cout << "|  ";
            if (table[i][j].size() == 0)
                cout << "#E ";
            for (int k = 0; k < table[i][j].size(); k++)
            {
                if (table[i][j][k] == LL1_PARSING_ERROR)
                    cout << "ERROR ";
                else
                    cout << table[i][j][k] << " ";
            }
        }
        cout << endl;
    }
}
int main()
{
    grammar g;
    list<expr> relist, ebnflist;
    unsigned int a0[] = {7, OP_TRMNL};
    unsigned int a1[] = {8, OP_TRMNL};
    unsigned int a2[] = {9, OP_TRMNL};
    unsigned int a3[] = {10, OP_TRMNL};
    unsigned int a4[] = {11, OP_TRMNL};
    unsigned int a5[] = {12, OP_TRMNL};
    unsigned int a6[] = {13, OP_TRMNL};
    relist.append(expr(7, a0));
    relist.append(expr(8, a1));
    relist.append(expr(9, a2));
    relist.append(expr(10, a3));
    relist.append(expr(11, a4));
    relist.append(expr(12, a5));
    relist.append(expr(13, a6));
    unsigned int b7[] = {2, OP_CNCAT, 1, OP_TRMNL};
    unsigned int b8[] = {7, OP_CNCAT, 0, OP_ALTER, EPSILON, OP_TRMNL};
    unsigned int b9[] = {4, OP_CNCAT, 3, OP_TRMNL};
    unsigned int b10[] = {2, OP_ALTER, EPSILON, OP_TRMNL};
    unsigned int b11[] = {6, OP_CNCAT, 5, OP_TRMNL};
    unsigned int b12[] = {8, OP_CNCAT, 5, OP_ALTER, EPSILON, OP_TRMNL};
    unsigned int b13[] = {9, OP_CNCAT, 0, OP_CNCAT, 10, OP_ALTER, 11, OP_ALTER, 12, OP_ALTER, 13, OP_TRMNL};
    ebnflist.append(expr(0, b7));
    ebnflist.append(expr(1, b8));
    ebnflist.append(expr(2, b9));
    ebnflist.append(expr(3, b10));
    ebnflist.append(expr(4, b11));
    ebnflist.append(expr(5, b12));
    ebnflist.append(expr(6, b13));
    cout << (EBNFToGrammar(ebnflist, relist, 0, g, true) ? "true\n" : "false\n");
    print(g);
    print(relist);
    cout << endl;
    print(ebnflist);
    // fa nfa, dfa;
    // REToNFA(relist, nfa);
    // print(nfa);
    // NFAToDFA(nfa, dfa);
    // MinimizeDFA(dfa);
    // unsigned int t[] = {1, 2, 3, 4, 5, 6, 7, 8};
    // dfa_table dt(dfa, t);
    // print(dfa);
    // print(dt);
    ll1_parsing_table t(g);
    print(t);
    return 0;
}