#include <fstream>
#include <cstring>
#include <ctime>
#include "FA.h"
// #include <iostream>
using namespace std;
fstream cin, cout;
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
    list<re> relist;
    cin.open("test_RE.in", ios::in);
    cout.open("test_RE.out", ios::out);
    unsigned int token = 0;
    while (!cin.fail())
    {
        const int size = 65536;
        char *s = new char[size];
        relist = list<re>();
        while (true)
        {
            cin.getline(s, size);
            if (cin.fail() || s[0] == '\0')
                break;
            unsigned int *a = new unsigned int[size];
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
            relist.append(re(a, token++));
            delete[] a;
        }
        fa nfa;
        id = 0;
        clock_t t_s = clock();
        bool valid = REToNFA(relist, nfa) ? "true" : "false";
        cout << "Expression: " << s << endl;
        cout << "Elapsed time is " << clock() - t_s << " ms\n";
        cout << valid << endl;
        print(nfa);
        delete[] s;
    }
    return 0;
}