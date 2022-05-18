#include <fstream>
#include <cstring>
#include <ctime>
#include "FA.h"
#include <iostream>
using namespace std;
// fstream cin, cout;
void print(expr e)
{
    cout << e.lhs << " = ";
    for (int i = 0; i < e.rhs.size(); i++)
        switch (e.rhs[i])
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
        case EPSILON:
            cout << "#";
            break;
        case TERMINAL:
            cout << "$";
            break;
        default:
            cout << e.rhs[i];
        }
    cout << endl;
}
void print(list<expr> l)
{
    for (int i = 0; i < l.size(); i++)
        print(l[i]);
}
void print(fa nfa)
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
        if (nfa.g[i].data.output == NON_ACC)
            cout << "\t";
        else
            cout << "\t" << nfa.g[i].data.output;
        for (int j = 0; j < nfa.g[i].size(); j++)
        {
            cout << "\t" << nfa.g[i][j].to->data.value;
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
    list<expr> relist;
    // cin.open("test_RE.in", ios::in);
    // cout.open("test_RE.out", ios::out);
    unsigned int token = 0;
    while (!cin.fail())
    {
        const int size = 65536;
        char *s = new char[size];
        relist = list<expr>();
        while (true)
        {
            cin.getline(s, size);
            if (cin.fail() || s[0] == '\0')
                break;
            list<unsigned int> a;
            for (int i = 0; i < (int)strlen(s); i++)
                switch (s[i])
                {
                case ' ':
                case '\t':
                    break;
                case '|':
                    a.append(OP_ALTER);
                    break;
                case '*':
                    a.append(OP_KLCLS);
                    break;
                case '~':
                    a.append(OP_CMPLM);
                    break;
                case '.':
                    a.append(OP_CNCAT);
                    break;
                case '(':
                    a.append(OP_LPRTH);
                    break;
                case ')':
                    a.append(OP_RPRTH);
                    break;
                case '!':
                    a.append(EPSILON);
                    break;
                default:
                    a.append(s[i]);
                    break;
                }
            relist.append({token++, a});
            cout << (ContainEpsilon({token++, a}) ? "true\n" : "false\n");
        }
        fa nfa;
        clock_t t_s = clock();
        bool valid = REToNFA(relist, nfa);
        // cout << "Size: " << relist.size() << endl;
        // cout << "Expression: " << s << endl;
        cout << "Elapsed time is " << clock() - t_s << " ms\n";
        cout << (valid ? "true" : "false") << endl;
        print(nfa);
        delete[] s;
    }
    return 0;
}