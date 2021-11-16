#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include "grammar.h"
#ifdef _WIN32
#include <windows.h>
#include <cstring>
#endif
using namespace std;
void print(parsing_tree &, list<list<wchar_t>> &, FILE *);
void print(parsing_tree &, list<list<wchar_t>> &);
void print(list<int>);
void print(list<unsigned int>, bool = false);
void print(list<unsigned int> *, int);
void print(expr);
void print(fa &);
void print(dfa_table);
void print(list<expr>);
void print(grammar &);
void print(ll1_parsing_table);
void print(list<list<wchar_t>>);
void print(list<hash_symbol_info>);
void print(list<token_info> l);
void print(lr1_item);
void print(list<lr1_item>);
void print(list<list<int>>);
void print(list<list<unsigned int>>);
void print(lr1_parsing_table &);

list<list<wchar_t>> names; // 表达式名称

int main()
{
    char *s = new char[1024];
    FILE *fp;
    while (true)
    {
        cout << "Please input the file name of rules: ";
        cin >> s;
        fp = fopen(s, "rb");
#ifndef _WIN32
        setlocale(LC_ALL, "C.UTF-8");
#endif
        if (fp != NULL)
            break;
        cout << "Could not open file, please try again.\n";
    }

    cout << "Analysing...\n";
    list<expr> ebnflist, relist; // 扩展BNF和正则表达式的列表
    unsigned int start;          // 起始符号
    list<unsigned int> sep;      // 字符分隔数组
    // 这个函数是从文件中读取扩充巴克斯瑙尔范式(EBNF), 以构造文法
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    char *buf = (char *)malloc(sizeof(char) * (len + 1));
    fread(buf, 1, len, fp);
    buf[len] = 0;
#ifdef _WIN32
    size_t wlen = MultiByteToWideChar(CP_UTF8, 0, buf, len, NULL, 0);
#else
    size_t wlen = mbstowcs(NULL, buf, len);
#endif
    wchar_t *wbuf = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
#ifdef _WIN32
    MultiByteToWideChar(CP_UTF8, 0, buf, len, wbuf, wlen);
#else
    mbstowcs(wbuf, buf, len);
#endif
    wbuf[wlen] = L'\0';
    ReadEBNFFromString(wbuf, ebnflist, start, relist, sep, names);
    fclose(fp);

    grammar g;
    // 这个函数是将初步处理的EBNF和RE转化为文法, 最后的bool值表示是否将文法
    // 中可以化为正则表达式的产生式化简为正则表达式
    EBNFToGrammar(ebnflist, relist, start, g, true);

    // 这个构造函数是将文法转换为LL(1)预测分析表
    lr1_parsing_table lr1_tb(g);
    if (lr1_tb.get_error_code() != 0)
    {
        cout << "This is not a LR(1) grammar.\n";
        return -1;
    }
    cout << "Syntax analysis finished. Lexical analysing...\n";

    fa dfa, nfa;
    // 这些函数是将正则表达式化简为最小化DFA的, 并转换为状态转移表.
    REToNFA(relist, nfa, sep.size() + 1);
    NFAToDFA(nfa, dfa);
    MinimizeDFA(dfa);
    dfa_table dfa_tb(dfa, sep);

    free(wbuf);
    free(buf);

    cout << "Ready for parsing. Enter 'quit' to terminate.\n";
    while (true)
    {
        cout << "Please input file name to parse: ";
        cin >> s;
        if (strcmp(s, "quit") == 0)
            break;
        fp = fopen(s, "rb");
        if (fp == NULL)
        {
            cout << "Could not open the file, please try again.\n";
            continue;
        }
        fseek(fp, 0, SEEK_END);
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
        bool suc = LR1Parsing(lr1_tb, dfa_tb, tr, wbuf);

        // 输出语法分析树, 左边是根节点, 右边是它的子结点
        fp = fopen("tree.dot", "wt");
        fprintf(fp, "digraph {\nnode [fontname=\"Times New Roman, SimSun\"]\n");
        fprintf(fp, "graph [dpi=300]\n");
        print(tr, names, fp);
        fprintf(fp, "\n}\n");
        fclose(fp);
        cout << (suc ? "success\n" : "failure\n");
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
void print(parsing_tree &tr, list<list<wchar_t>> &names, FILE *fp)
{
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
void print(parsing_tree &tr, list<list<wchar_t>> &names)
{
    if (tr.symbol < (unsigned int)names.size())
    {
        for (int j = 0; j < names[tr.symbol].size(); j++)
        {
            wchar_t wstr[2] = {(unsigned short)names[tr.symbol][j], 0};
            char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
            WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
            wcstombs(str, wstr, 8);
#endif
            cout << str;
        }
        cout << ": ";
    }
    else
        cout << tr.symbol << ": ";
    if (tr.subtree.size() == 0)
    {
        if (tr.token.lexeme == NULL)
            cout << "#";
        else
        {
#ifdef _WIN32
            int len = WideCharToMultiByte(
                CP_ACP, 0, tr.token.lexeme, wcslen(tr.token.lexeme), NULL, 0, 0, NULL);
#else
            int len = wcstombs(NULL, tr.token.lexeme, 0);
#endif
            char *str = (char *)malloc(sizeof(wchar_t) * (len + 1));
#ifdef _WIN32
            WideCharToMultiByte(
                CP_ACP, 0, tr.token.lexeme, wcslen(tr.token.lexeme), str, len, 0, NULL);
#else
            wcstombs(str, tr.token.lexeme, len);
#endif
            str[len] = '\0';
            cout << str;
            free(str);
        }
    }
    for (int i = 0; i < tr.subtree.size(); i++)
        if (tr.subtree[i].symbol < (unsigned int)names.size())
        {
            for (int j = 0; j < names[tr.subtree[i].symbol].size(); j++)
            {
                wchar_t wstr[2] = {(unsigned short)names[tr.subtree[i].symbol][j], 0};
                char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
                WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
                wcstombs(str, wstr, 8);
#endif
                cout << str;
            }
            cout << " ";
        }
        else
            cout << tr.subtree[i].symbol << " ";
    cout << endl;
    for (int i = 0; i < tr.subtree.size(); i++)
        print(tr.subtree[i], names);
}
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << endl;
}
void print(list<unsigned int> l, bool ch)
{
    for (int i = 0; i < l.size(); i++)
        if (l[i] == EPSILON)
            cout << "# ";
        else if (l[i] == TERMINAL)
            cout << "$ ";
        else if (ch)
        {
            wchar_t wstr[2] = {(unsigned short)l[i], 0};
            char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
            WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
            wcstombs(str, wstr, 8);
#endif
            cout << str;
        }
        else
            cout << l[i] << " ";
    if (!ch)
        cout << endl;
}
void print(list<wchar_t> l)
{
    for (int i = 0; i < l.size(); i++)
        if (l[i] == EPSILON)
            cout << "# ";
        else if (l[i] == TERMINAL)
            cout << "$";
        else
        {
            wchar_t wstr[2] = {(unsigned short)l[i], 0};
            char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
            WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
            wcstombs(str, wstr, 8);
#endif
            cout << str;
        }
}
void print(list<unsigned int> *l, int n)
{
    for (int m = 0; m < n; m++)
    {
        for (int i = 0; i < l[m].size(); i++)
            if (l[m][i] == EPSILON)
                cout << "# ";
            else if (l[m][i] == TERMINAL)
                cout << "$";
            else
                cout << l[m][i] << " ";
        cout << endl;
    }
    cout << endl;
}
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
void print(fa &nfa)
{
    cout << "sigma: [0, " << nfa.sigma_range << ")\n";
    cout << "start: " << nfa.s << "\n";
    cout << "final: ";
    for (int i = 0; i < nfa.f.size(); i++)
        cout << nfa.f[i] << " ";
    cout << endl;
    cout << "graph:" << endl;
    cout << setw(24) << "vertex"
         << setw(8) << "token"
         << setw(8) << "edges\n";
    for (int i = 0; i < nfa.g.size(); i++)
    {
        cout << setw(8) << nfa.g[i].data.value << setw(16) << &nfa.g[i];
        if (nfa.g[i].data.token == NON_ACC)
            cout << setw(8) << " ";
        else
            cout << setw(8) << nfa.g[i].data.token;
        for (int j = 0; j < nfa.g[i].size(); j++)
        {
            cout << setw(8) << nfa.g[i][j].to->data.value << ":";
            if (nfa.g[i][j].data.value == EPSILON)
                cout << "#";
            else if (nfa.g[i][j].data.value == TERMINAL)
                cout << "$";
            else
                cout << (nfa.g[i][j].data.value);
        }
        cout << endl;
    }
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
void print(list<list<wchar_t>> names)
{
    for (int i = 0; i < names.size(); i++)
    {
        for (int j = 0; j < names[i].size(); j++)
        {
            wchar_t wstr[2] = {(unsigned short)names[i][j], 0};
            char str[8] = "\0\0\0\0\0\0\0";
#ifdef _WIN32
            WideCharToMultiByte(CP_ACP, 0, wstr, 2, str, 8, 0, NULL);
#else
            wcstombs(str, wstr, 8);
#endif
            cout << str;
        }
        cout << endl;
    }
}
void print(list<hash_symbol_info> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i].symbol << " ";
    cout << endl;
}

void print(list<token_info> l)
{
    for (int i = 0; i < l.size(); i++)
    {
        if ((int)l[i].token < names.size() && (int)l[i].token >= 0)
            print(names[l[i].token]);
        else
            cout << "PANIC: " << l[i].token;
        cout << ": ";
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
        cout << str << endl;
        free(str);
    }
}

void print(lr1_item i)
{
    cout << i.pd.variable << " ::= ";
    int j;
    for (j = 0; j < i.pd.expression.size(); j++)
    {
        if (j == i.dot)
            cout << ". ";
        cout << i.pd.expression[j] << " ";
    }
    if (j == i.dot)
        cout << ". ";
    if (i.sym == TERMINAL)
        cout << ", $\n";
    else
        cout << ", " << i.sym << endl;
}

void print(list<lr1_item> l)
{
    for (int i = 0; i < l.size(); i++)
    {
        cout << l[i].pd.variable << " ::= ";
        int j;
        for (j = 0; j < l[i].pd.expression.size(); j++)
        {
            if (j == l[i].dot)
                cout << ". ";
            cout << l[i].pd.expression[j] << " ";
        }
        if (j == l[i].dot)
            cout << ". ";
        if (l[i].sym == TERMINAL)
            cout << ", $\n";
        else
            cout << ", " << l[i].sym << endl;
    }
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

void print(list<list<unsigned int>> l)
{
    for (int i = 0; i < l.size(); i++)
    {
        for (int j = 0; j < l[i].size(); j++)
            cout << l[i][j] << " ";
        cout << endl;
    }
    cout << endl;
}

void print(lr1_parsing_table &table)
{
    cout << setw(8) << "Table"
         << "|";
    for (int i = 0; i < table.get_col(); i++)
        cout << setw(8) << i;
    cout << endl;
    for (int i = 0; i < table.get_row(); i++)
    {
        if (i == table.get_start())
            if (table.get_token(i) != NON_ACC)
                cout << " f s" << setw(4) << i << "|";
            else
                cout << " s" << setw(6) << i << "|";
        else if (table.get_token(i) != NON_ACC)
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
