#include <fstream>
#include <cstring>
#include <ctime>
#include "FA.h"
// #include <iostream>
using namespace std;
fstream cin, cout;
void print(fa_t nfa)
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
}
int main()
{
    // test-1: (1|2)*.(((a.b|3.2*.c)*|c|d*.e.a).4.e|((d|e).(e|a*)).b|(2.1.3*)*.((a.c)*|c))|c.e
    list<re_t> relist;
    cin.open("test_RE.in", ios::in);
    cout.open("test_RE.out", ios::out);
    while (!cin.fail())
    {
        const int size = 65536;
        char *s = new char[size];
        cin.getline(s, size);
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
                a[i] = OP_ENCLS;
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
        relist = list<re_t>();
        relist.append({a, 12});
        fa_t nfa;
        id = 0;
        clock_t t_s = clock();
        bool valid = REToNFA(relist, nfa) ? "true" : "false";
        cout << "Elapsed time is " << clock() - t_s << " ms\n";
        cout << valid << endl;
        for (int i = 0; i < nfa.g.size(); i++)
            nfa.g[i].data.value = i;
        print(nfa);
        delete[] s;
        delete[] a;
    }
    return 0;
}