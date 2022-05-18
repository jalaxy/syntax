#include "LALR.h"
#include <cstdio>
#include <windows.h>
void print(expr);
void print(list<expr>);
void print(list<wchar_t>);
void print(list<list<wchar_t>>, list<unsigned int>);
void print(grammar, list<list<wchar_t>>);
void print(fa nfa);
void print(parsing_tree &tr, list<list<wchar_t>> names, FILE *fp);
void print(list<token_info> l, list<list<wchar_t>> names);
void print(list<unsigned int> l, bool ch);
void print(list<int> l);
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

    fa dfa;
    REToNFA(relist, dfa, sep.size() + 1);
    NFAToDFA(dfa, dfa);
    MinimizeDFA(dfa);
    print(dfa);
    transition_table tr_t(dfa, sep, types);
    lr1_parsing_table lr_t(g);
    char *tb_buf = (char *)malloc(32767);
    printf("%d %d\n", lr_t.store(), lr_t.store(tb_buf));
    // lr_t = tb_buf;
    free(tb_buf);

    // printf("\nnames:\n");
    // print(names, types);
    // printf("\nEBNF list:\n");
    // print(ebnflist);
    // printf("\nRE list:\n");
    // print(relist);
    // printf("\n");
    // printf("grammar:\n");
    // print(g, names);

    char *s = new char[1024];
    while (true)
    {
        printf("Please input file name to parse: ");
        scanf("%s", s);
        if (strcmp(s, "quit") == 0)
            break;
        fp = fopen(s, "rb");
        if (fp == NULL)
        {
            strcpy(s, "string.txt");
            fp = fopen(s, "rb");
            printf("  Using default file: \"string.txt\"\n");
            // printf("Could not open the file, please try again.\n");
            // continue;
        }
        fseek(fp, 0, SEEK_END);
        int len, wlen;
        len = ftell(fp);
        rewind(fp);
        buf = (char *)malloc(sizeof(char) * (len + 1));
        fread(buf, 1, len, fp);
        buf[len] = 0;
#ifdef _WIN32
        wlen = MultiByteToWideChar(CP_UTF8, 0, buf, len, NULL, 0);
#else
        wlen = mbstowcs(NULL, buf, len);
#endif
        wbuf = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
#ifdef _WIN32
        MultiByteToWideChar(CP_UTF8, 0, buf, len, wbuf, wlen);
#else
        mbstowcs(wbuf, buf, len);
#endif
        wbuf[wlen] = L'\0';
        fclose(fp);

        parsing_tree tr;
        // 这个函数是LL(1)的分析过程, 输出结果为一个语法树tr
        list<token_info> tk;
        bool suc = tr_t.token_stream(wbuf, tk);
        suc &= lr_t.parse(tk, tr);

        // 输出语法分析树, 左边是根节点, 右边是它的子结点
        fp = fopen("tree.dot", "wt");
        fprintf(fp, "digraph {\nnode [fontname=\"Times New Roman, SimSun\"]\n");
        fprintf(fp, "graph [dpi=300]\n");
        print(tr, names, fp);
        fprintf(fp, "\n}\n");
        fclose(fp);
        printf(suc ? "success\n" : "failure\n");
#ifdef _WIN32
        system(".\\Graphviz\\dot.exe -Tjpg tree.dot -o tree.jpg");
        system(".\\tree.jpg");
#endif
        free(buf);
        free(wbuf);
    }
    delete[] s;

    return 0;
}
void print(parsing_tree &tr, list<list<wchar_t>> names, FILE *fp)
{
    for (int i = 0; i < names.size(); i++)
        for (int j = 0; j < names[i].size(); j++)
            if (names[i][j] == L'"')
                names[i][j] = L'\'';
    static unsigned short id = 0xffff;
    fprintf(fp, "%d [label=\"", tr.id);
    if (tr.symbol < (unsigned int)names.size())
    {
        for (int j = 0; j < names[tr.symbol].size(); j++)
        {
            wchar_t wstr[2] = {(unsigned short)names[tr.symbol][j], 0};
            char str[8] = "\0\0\0\0\0\0\0";
            WideCharToMultiByte(CP_UTF8, 0, wstr, 2, str, 8, 0, NULL);
            fprintf(fp, "%s", str);
        }
    }
    else if (tr.symbol == AUGMNTED)
        fprintf(fp, "S'");
    fprintf(fp, "\"];\n");
    if (tr.subtree.size() == 0)
    {
        fprintf(fp, "%d->%d;\n", tr.id, id);
        fprintf(fp, "%d [label=\"", id--);
        if (tr.token.lexeme == NULL)
            fprintf(fp, "#E");
        else
        {
            int len = WideCharToMultiByte(
                CP_UTF8, 0, tr.token.lexeme, wcslen(tr.token.lexeme), NULL, 0, 0, NULL);
            char *str = (char *)malloc(sizeof(wchar_t) * (len + 1));
            WideCharToMultiByte(
                CP_UTF8, 0, tr.token.lexeme, wcslen(tr.token.lexeme), str, len, 0, NULL);
            str[len] = '\0';
            for (int i = 0; i < (int)strlen(str); i++)
                if (str[i] == '\n' || str[i] == '\r' || str[i] == '\t')
                    fprintf(fp, "#x%X", str[i]);
                else if (str[i] == '"')
                    fputc('\'', fp);
                else
                    fputc(str[i], fp);
            free(str);
        }
        fprintf(fp, "\"];\n");
    }
    for (int i = 0; i < tr.subtree.size(); i++)
        fprintf(fp, "%d->%d;\n", tr.id, tr.subtree[i].id);
    for (int i = 0; i < tr.subtree.size(); i++)
        print(tr.subtree[i], names, fp);
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
    printf("sigma:\t[0, %u)\nstart:\t0x%x\nfinal:\t", nfa.sigma_range, (unsigned int)(long long)nfa.s);
    for (int i = 0; i < nfa.f.size(); i++)
        printf("0x%x ", (unsigned int)(long long)nfa.f[i]);
    printf("\ngraph:\t\n\tvertex\t\t\ttoken\t\tedges\n");
    for (int i = 0; i < nfa.g.size(); i++)
    {
        printf("\t%u\t0x%x", nfa.g[i].data.value, (unsigned int)(long long)&nfa.g[i]);
        if (!nfa.g[i].data.output.empty())
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
void print(list<token_info> l, list<list<wchar_t>> names)
{
    for (int i = 0; i < l.size(); i++)
    {
        if ((int)l[i].token < names.size() && (int)l[i].token >= 0)
            print(names[l[i].token]);
        else
            printf("temp%u", l[i].token);
        printf(": ");
#ifdef _WIN32
        int len = WideCharToMultiByte(
            CP_ACP, 0, l[i].lexeme, wcslen(l[i].lexeme), NULL, 0, 0, NULL);
#else
        int len = wcstombs(NULL, l[i].lexeme, 0);
#endif
        char *str = (char *)malloc(sizeof(wchar_t) * (len + 1));
#ifdef _WIN32
        WideCharToMultiByte(
            CP_ACP, 0, l[i].lexeme, wcslen(l[i].lexeme), str, len, 0, NULL);
#else
        wcstombs(str, l[i].lexeme, len);
#endif
        str[len] = '\0';
        printf("%s\n", str);
        free(str);
    }
}
void print(list<unsigned int> l, bool ch)
{
    for (int i = 0; i < l.size(); i++)
        if (l[i] == EPSILON)
            printf("# ");
        else if (l[i] == TERMINAL)
            printf("$ ");
        else if (ch)
        {
            wchar_t wstr[2] = {(unsigned short)l[i], 0};
            char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
            WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
            wcstombs(str, wstr, 8);
#endif
            printf("%s", str);
        }
        else
            printf("%u ", l[i]);
    if (!ch)
        printf("\n");
}
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        printf("%d ", l[i]);
    printf("\n");
}
