#include "LALR.h"
#include <cstdio>
#include <windows.h>
void print(expr);
void print(list<expr>);
void print(list<wchar_t>);
void print(list<list<wchar_t>>, list<unsigned int>);
void print(grammar, list<list<wchar_t>>);
void print(fa nfa);
int main()
{
    FILE *fp = fopen("EBNF.txt", "rb");
    char *buf = (char *)malloc(sizeof(char) * 32767);
    wchar_t *wbuf = (wchar_t *)malloc(sizeof(wchar_t) * 32767);
    memset(buf, 0, sizeof(char) * 32767);
    memset(wbuf, 0, sizeof(wchar_t) * 32767);
    fread(buf, sizeof(char), 32767, fp);
    for (int i = 0; i < 32767; i++)
        wbuf[i] = buf[i];
    free(buf);
    fclose(fp);
    list<expr> ebnflist, relist;
    list<unsigned int> sep, types;
    list<list<wchar_t>> names;
    unsigned int start;
    int err = ReadEBNFFromString(wbuf, ebnflist, start, relist, sep, names, types);
    free(wbuf);
    if (err != 0)
    {
        printf("EBNF syntax error near line %d\n", err);
        return 0;
    }

    grammar g;
    list<expr> ebnflist_v, ebnflist_t;
    for (int i = 0; i < ebnflist.size(); i++)
        if (types[ebnflist[i].lhs] == TERMINAL || types[ebnflist[i].lhs] == IGNORSYM)
            ebnflist_t.append(ebnflist[i]);
        else if (types[ebnflist[i].lhs] == VARIABLE)
            ebnflist_v.append(ebnflist[i]);
    err = EBNFToGrammar(ebnflist_v, g, ebnflist_v[0].lhs, relist.top().lhs + 1);
    if (err != -1)
    {
        printf("Grammar conversion error, please check variable \"");
        print(names[ebnflist_v[err].lhs]);
        printf("\"\n");
        return 0;
    }
    err = EBNFToRE(ebnflist_t, relist);
    if (err != -1)
    {
        printf("RegEx conversion error, please check terminal \"");
        print(names[ebnflist_t[err].lhs]);
        printf("\"\n");
        return 0;
    }
    EliminateUnreachableRE(relist, g, types);

    lr1_parsing_table lr_t(g);

    printf("\nnames:\n");
    print(names, types);
    printf("\nEBNF list:\n");
    print(ebnflist);
    printf("\nRE list:\n");
    print(relist);
    printf("\n");
    printf("grammar:\n");
    print(g, names);

    return 0;
}
void print(expr e)
{
    printf("%u = ", e.lhs);
    for (int i = 0; i < e.rhs.size(); i++)
        switch (e.rhs[i])
        {
        case OP_ALTER:
            printf("|");
            break;
        case OP_CMPLM:
            printf("~");
            break;
        case OP_CNCAT:
            printf(".");
            break;
        case OP_KLCLS:
            printf("*");
            break;
        case OP_LPRTH:
            printf("(");
            break;
        case OP_MINUS:
            printf("-");
            break;
        case OP_OPTNL:
            printf("?");
            break;
        case OP_PTCLS:
            printf("+");
            break;
        case OP_RPRTH:
            printf(")");
            break;
        case EPSILON:
            printf("#");
            break;
        case ENDMARK:
            printf("$");
            break;
        default:
            printf("%u", e.rhs[i]);
        }
    printf("\n");
}
void print(list<expr> l)
{
    for (int i = 0; i < l.size(); i++)
        print(l[i]);
}
void print(list<wchar_t> name)
{
    for (int j = 0; j < name.size(); j++)
    {
        wchar_t wstr[2] = {(unsigned short)name[j], 0};
        char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
        WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
        wcstombs(str, wstr, 8);
#endif
        printf("%s", str);
    }
}
void print(list<list<wchar_t>> names, list<unsigned int> types)
{
    for (int i = 0; i < names.size(); i++)
    {
        printf("%d\t", i);
        print(names[i]);
        if (types[i] == TERMINAL)
            printf("\tT\n");
        else if (types[i] == VARIABLE)
            printf("\tV\n");
        else
            printf("\tI\n");
    }
}
void print(grammar g, list<list<wchar_t>> names)
{
    for (int i = 0; i < g.productions.size(); i++)
    {
        if (g.productions[i].variable < (unsigned int)names.size())
            print(names[g.productions[i].variable]);
        else
            printf("temp%u", g.productions[i].variable);
        printf(" = ");
        for (int j = 0; j < g.productions[i].expression.size(); j++)
        {
            printf(j == 0 ? "" : "| ");
            if (g.productions[i].expression[j].size() == 0)
                printf("#E ");
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
            {
                if (g.productions[i].expression[j][k] < (unsigned int)names.size())
                    print(names[g.productions[i].expression[j][k]]);
                else
                    printf("temp%u", g.productions[i].expression[j][k]);
                printf(" ");
            }
        }
        printf("\n");
    }
}

void print(fa nfa)
{
    printf("sigma:\t[0, %u)\nstart:\t0x%x\nfinal:\t", nfa.sigma_range, (unsigned int)nfa.s);
    for (int i = 0; i < nfa.f.size(); i++)
        printf("0x%x ", (unsigned int)nfa.f[i]);
    printf("\ngraph:\t\n\tvertex\t\t\ttoken\t\tedges\n");
    for (int i = 0; i < nfa.g.size(); i++)
    {
        printf("\t%u\t0x%x", nfa.g[i].data.value, (unsigned int)&nfa.g[i]);
        if (nfa.g[i].data.output == NON_ACC)
            printf("\t");
        else
            printf("\t%u", nfa.g[i].data.output);
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
