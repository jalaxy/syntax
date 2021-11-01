#include <fstream>
#include <iomanip>
#include "grammar.h"
using namespace std;
fstream cout;
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << endl;
}
void print(list<unsigned int> l, bool ch = false)
{
    for (int i = 0; i < l.size(); i++)
        if (l[i] == EPSILON)
            cout << "#E ";
        else if (ch)
        {
            wchar_t wstr[2] = {(unsigned short)l[i], 0};
            char str[3] = "\0\0";
            WideCharToMultiByte(CP_ACP, 0, wstr, 1, str, 2, 0, 0);
            cout << str << " ";
        }
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
void print(list<list<unsigned int>> names)
{
    for (int i = 0; i < names.size(); i++)
    {
        for (int j = 0; j < names[i].size(); j++)
        {
            wchar_t wstr[2] = {(unsigned short)names[i][j], 0};
            char str[3] = "\0\0";
            WideCharToMultiByte(CP_ACP, 0, wstr, 1, str, 2, 0, 0);
            cout << str << " ";
        }
        cout << endl;
    }
}
int main()
{
    FILE *fp = fopen("test_EBNF.in", "rb");
    list<expr> ebnflist, relist;
    list<unsigned int> sep;
    list<list<unsigned int>> names;
    cout << ReadEBNFFromFile(fp, CP_UTF8, ebnflist, relist, sep, names) << endl;
    fclose(fp);
    cout.open("test_EBNF.out", ios::out);
    // print(relist);
    // cout << endl;
    // print(names);
    // cout << endl;
    // print(ebnflist);
    // cout << endl;
    // print(sep, true);
    grammar g;
    cout << (EBNFToGrammar(ebnflist, relist, 0, g) ? "true" : "false") << endl;
    // print(g);
    cout << endl;
    // print(relist);
    fa nfa, dfa;
    REToNFA(relist, nfa, sep.size() + 1);
    print(nfa);
    NFAToDFA(nfa, dfa);
    print(dfa);
    // MinimizeDFA(dfa);
    return 0;
}