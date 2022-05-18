/****************************************************************
 * @file pregen.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Generate source files semantics.* and sdt.*
 * @version 0.1
 * @date 2021-12-12
 *
 * @copyright Copyright (c) 2021
 *
 ****************************************************************/

#include "LALR.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
int int_len(int);
void print(expr, FILE * = NULL);
void print(list<expr>, FILE * = NULL);
void print(list<wchar_t>, FILE * = NULL);
void print(list<list<wchar_t>>, list<unsigned int>, FILE * = NULL,
           const char * = NULL, const char * = NULL, int = 0, int = 0);
void print(grammar, list<list<wchar_t>>, FILE * = NULL,
           const char * = NULL, const char * = NULL, int = 0);
void print(list<unsigned int> l, bool ch, FILE * = NULL);
void print(list<int> l, FILE * = NULL);
int main(int argc, char **argv)
{
    FILE *fp;
    if (argc < 2)
    {
        printf("Error: no input files.\n");
        return 0;
    }
    fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        printf("Error: can't locate file: %s.\n", argv[1]);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size_t buf_len = ftell(fp);
    rewind(fp);
    char *buf = (char *)malloc(sizeof(char) * (buf_len + 1));
    fread(buf, 1, buf_len, fp);
    buf[buf_len] = 0;
    size_t wbuf_len = mbstowcs(NULL, buf, buf_len);
    wchar_t *wbuf = (wchar_t *)malloc(sizeof(wchar_t) * (buf_len + 1));
    if (buf == NULL || wbuf == NULL)
    {
        printf("Error: memory allocation error.\n");
        return 0;
    }
    mbstowcs(wbuf, buf, buf_len);
    wbuf[wbuf_len] = L'\0';
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
        printf("Error: EBNF syntax error near line %d.\n", err);
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
        printf("Error: grammar conversion error, please check variable \"");
        if (ebnflist_v[err].lhs < names.size())
            print(names[ebnflist_v[err].lhs]);
        printf("\".\n");
        return 0;
    }
    err = EBNFToRE(ebnflist_t, relist);
    if (err != -1)
    {
        printf("Error: RegEx conversion error, please check terminal \"");
        if (ebnflist_t[err].lhs < names.size())
            print(names[ebnflist_t[err].lhs]);
        printf("\"\n");
        return 0;
    }
    EliminateUnreachableRE(relist, g, types);
    int gsz = 0;
    for (int i = 0; i < g.productions.size(); i++)
        gsz += g.productions[i].expression.size();
    printf("Grammar size: %d\n", gsz);
    printf("terminal size: %d\n", sep.size() + 1);

    fa dfa;
    REToNFA(relist, dfa, sep.size() + 1);
    printf("Lexical NFA size: %d\n", dfa.g.size());
    NFAToDFA(dfa, dfa);
    printf("Lexical DFA size: %d\n", dfa.g.size());
    MinimizeDFA(dfa);
    printf("Lexical minimized DFA size: %d\n", dfa.g.size());
    transition_table tr_t(dfa, sep, types);
    lr1_parsing_table lr_t(g);

    // write semantics.h
    g.augment();
    fp = fopen("semantics.h", "w");
    fprintf(fp, "/*********************************************************\n");
    fprintf(fp, " * @file semantics.h                                     *\n");
    fprintf(fp, " * @brief Define class of every symbol.                  *\n");
    fprintf(fp, " *        For variables, the class name has prefix \"v_\". *\n");
    fprintf(fp, " *        For terminals, the class name has prefix \"t_\", *\n");
    fprintf(fp, " *                       and has an initial element lex, *\n");
    fprintf(fp, " *                       representing lexeme string.     *\n");
    fprintf(fp, " *        Users should define attributes for each class. *\n");
    fprintf(fp, " *********************************************************/\n");
    fprintf(fp, "\n");
    fprintf(fp, "#include \"LALR.h\"\n");
    int len = 3; // length of Aug
    for (int i = 0; i < names.size(); i++)
        if (names[i].size() > len)
            len = names[i].size();
    for (int i = 0; i < g.productions.size(); i++)
        if (g.productions[i].variable >= names.size() &&
            4 + int_len(g.productions[i].variable) > len)
            len += 4 + int_len(g.productions[i].variable);
    fprintf(fp, "\n");
    fprintf(fp, "/*");
    int star_size = 13 + len > 46 ? 13 + len : 46;
    for (int i = 0; i < star_size; i++)
        fprintf(fp, "*");
    fprintf(fp, "\n");
    fprintf(fp, " * The symbols are listed below:");
    for (int i = 0; i < star_size - 31; i++)
        fprintf(fp, " ");
    fprintf(fp, "*\n");
    print(names, types, fp, " * ", "*", len, star_size);
    fprintf(fp, " * (V -> variable, T -> terminal, I -> ignore)");
    for (int i = 0; i < star_size - 45; i++)
        fprintf(fp, " ");
    fprintf(fp, "*\n");
    fprintf(fp, " ");
    for (int i = 0; i < star_size; i++)
        fprintf(fp, "*");
    fprintf(fp, "*/\n");
    for (int i = 0; i < g.productions.size(); i++)
    {
        fprintf(fp, "\n");
        fprintf(fp, "// class for ");
        if (g.productions[i].variable < names.size())
            print(names[g.productions[i].variable], fp);
        else if (g.productions[i].variable == AUGMNTED)
            fprintf(fp, "Aug");
        else
            fprintf(fp, "Temp%u", g.productions[i].variable);
        fprintf(fp, "\n");
        fprintf(fp, "class v_%d\n", i);
        fprintf(fp, "{\n");
        fprintf(fp, "public:\n");
        fprintf(fp, "};\n");
    }
    for (int i = 0; i < relist.size(); i++)
        if (types[relist[i].lhs] != IGNORSYM)
        {
            fprintf(fp, "\n");
            fprintf(fp, "// class for ");
            if (relist[i].lhs < names.size())
                print(names[relist[i].lhs], fp);
            else if (relist[i].lhs == AUGMNTED)
                fprintf(fp, "Aug");
            fprintf(fp, "\n");
            fprintf(fp, "class t_%d\n", i);
            fprintf(fp, "{\n");
            fprintf(fp, "public:\n");
            fprintf(fp, "};\n");
        }
    fprintf(fp, "\nextern const void *(*f_list[])(const void **);\n");
    fprintf(fp, "extern const void *(*ft_list[])(const token_info &);\n");
    fclose(fp);

    // write semantics.cpp
    list<hash_symbol_info> *aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    if (aidx == NULL)
    {
        printf("Error: memory allocation error.");
        return 0;
    }
    for (int i = 0; i < g.productions.size(); i++)
        aidx[g.productions[i].variable % HASH_SZ].append({false, i, g.productions[i].variable});
    for (int i = 0; i < relist.size(); i++)
        aidx[relist[i].lhs % HASH_SZ].append({true, i, relist[i].lhs});
    fp = fopen("semantics.cpp", "wb");
    fprintf(fp, "/*****************************************************\n");
    fprintf(fp, " * @file semantics.cpp                               *\n");
    fprintf(fp, " * @brief Define semantic rules for each production. *\n");
    fprintf(fp, " *        Parameters of functions are pointer type.  *\n");
    fprintf(fp, " *        New objects will be created in functions.  *\n");
    fprintf(fp, " *****************************************************/\n");
    fprintf(fp, "\n");
    fprintf(fp, "#include \"semantics.h\"\n");
    fprintf(fp, "\n");
    len = 0;
    for (int i = 0; i < g.productions.size(); i++)
    {
        int l = 2;
        if (g.productions[i].variable < (unsigned int)names.size())
            l += names[g.productions[i].variable].size() + 1;
        else if (g.productions[i].variable == AUGMNTED)
            l += 4;
        else
            l += 5 + int_len(g.productions[i].variable);
        for (int j = 0; j < g.productions[i].expression.size(); j++)
        {
            l += 2;
            if (g.productions[i].expression[j].empty())
                l += 3;
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
                if (g.productions[i].expression[j][k] < (unsigned int)names.size())
                    l += names[g.productions[i].expression[j][k]].size() + 1;
                else
                    l += 5 + int_len(g.productions[i].expression[j][k]);
        }
        if (l > len)
            len = l;
    }
    len = len > 29 ? len : 29;
    fprintf(fp, "/*");
    for (int i = 0; i < len + 2; i++)
        fprintf(fp, "*");
    fprintf(fp, "\n");
    fprintf(fp, " * The grammar is listed below:");
    for (int i = 0; i < len - 28; i++)
        fprintf(fp, " ");
    fprintf(fp, "*");
    fprintf(fp, "\n");
    print(g, names, fp, " * ", "*", len);
    fprintf(fp, " ");
    for (int i = 0; i < len + 2; i++)
        fprintf(fp, "*");
    fprintf(fp, "*/\n");
    int func_id = 0;
    for (int i = 0; i < g.productions.size(); i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
        {
            fprintf(fp, "\n");
            list<unsigned int> &exp = g.productions[i].expression[j];
            fprintf(fp, "// function for ");
            if (g.productions[i].variable < names.size())
                print(names[g.productions[i].variable], fp);
            else if (g.productions[i].variable == AUGMNTED)
                fprintf(fp, "Aug");
            else
                fprintf(fp, "Temp%u", g.productions[i].variable);
            fprintf(fp, " ::=");
            if (exp.empty())
                fprintf(fp, " #E");
            for (int k = 0; k < exp.size(); k++)
            {
                fprintf(fp, " ");
                if (exp[k] < names.size())
                    print(names[exp[k]], fp);
                else
                    fprintf(fp, "Temp%u", exp[k]);
            }
            fprintf(fp, "\n");
            fprintf(fp, "const void *f_%d(const void **argv)\n", func_id);
            fprintf(fp, "{\n");
            int l;
            for (l = 0; l < aidx[g.productions[i].variable % HASH_SZ].size(); l++)
                if (aidx[g.productions[i].variable % HASH_SZ][l].symbol ==
                    g.productions[i].variable)
                {
                    fprintf(fp, "    v_%d *retval = new (std::nothrow) v_%d;",
                            aidx[g.productions[i].variable % HASH_SZ][l].idx,
                            aidx[g.productions[i].variable % HASH_SZ][l].idx);
                    break;
                }
            fprintf(fp, "\n");
            fprintf(fp, "    if (retval == NULL)\n        return NULL;\n");
            for (int k = 0; k < exp.size(); k++)
            {
                list<hash_symbol_info> &info = aidx[g.productions[i].expression[j][k] % HASH_SZ];
                for (int l = 0; l < aidx[g.productions[i].variable % HASH_SZ].size(); l++)
                    if (info[l].symbol == g.productions[i].expression[j][k])
                    {
                        fprintf(fp, info[l].regular ? "    const t" : "    const v");
                        fprintf(fp, "_%d *arg_%d = (", info[l].idx, k);
                        fprintf(fp, info[l].regular ? "const t" : "const v");
                        fprintf(fp, "_%d *)argv[%d]; // ", info[l].idx, k);
                        if (exp[k] < names.size())
                            print(names[exp[k]], fp);
                        else if (exp[k] == AUGMNTED)
                            fprintf(fp, "Aug");
                        else
                            fprintf(fp, "Temp%u", exp[k]);
                        fprintf(fp, "\n");
                        break;
                    }
            }
            fprintf(fp, "\n    // add rules here\n");
            fprintf(fp, "\n    return retval;\n");
            fprintf(fp, "}\n");
            func_id++;
        }
    for (int i = 0; i < relist.size(); i++)
        if (types[relist[i].lhs] != IGNORSYM)
        {
            fprintf(fp, "\n");
            fprintf(fp, "// function for ");
            print(names[relist[i].lhs], fp);
            fprintf(fp, "\n");
            fprintf(fp, "const void *ft_%d(const token_info &tk)\n", i);
            fprintf(fp, "{\n");
            fprintf(fp, "    t_%d *retval = new (std::nothrow) t_%d;", i, i);
            fprintf(fp, "\n");
            fprintf(fp, "    if (retval == NULL)\n        return NULL;\n");
            fprintf(fp, "\n    // add rules here\n");
            fprintf(fp, "\n    return retval;\n");
            fprintf(fp, "}\n");
        }
    fprintf(fp, "\nconst void *(*ft_list[])(const token_info &) = {\n    ");
    int re_non_ignore_size = 0;
    for (int i = 0; i < relist.size(); i++)
        if (types[relist[i].lhs] != IGNORSYM)
            re_non_ignore_size++;
    for (int i = 0; i < relist.size(); i++)
        if (types[relist[i].lhs] != IGNORSYM)
        {
            fprintf(fp, i == re_non_ignore_size - 1 ? "ft_%d" : "ft_%d, ", i);
            fprintf(fp, i < re_non_ignore_size - 1 && (i + 1) % 16 == 0 ? "\n    " : "");
        }
    fprintf(fp, "};\n");
    fprintf(fp, "const void *(*f_list[])(const void **) = {\n    ");
    for (int i = 0; i < func_id; i++)
    {
        fprintf(fp, i == func_id - 1 ? "f_%d" : "f_%d, ", i);
        fprintf(fp, i < func_id - 1 && (i + 1) % 16 == 0 ? "\n    " : "");
    }
    fprintf(fp, "};\n");
    fclose(fp);

    // Write sdt.cpp
    fp = fopen("sdt.cpp", "wb");
    fprintf(fp, "/***************************************\n");
    fprintf(fp, " * @file sdt.cpp                       *\n");
    fprintf(fp, " * @brief Store tables and entry point *\n");
    fprintf(fp, " ***************************************/\n");
    fprintf(fp, "\n#include \"semantics.h\"\n");
    fprintf(fp, "#include <cstdio>\n");
    fprintf(fp, "#include <cstdlib>\n");
    fprintf(fp, "\nextern unsigned char tr_buf[], lr_buf[];\n");
    fprintf(fp, "\nint main(int argc, char **argv)\n");
    fprintf(fp, "{\n");
    fprintf(fp, "    if (argc < 2)\n");
    fprintf(fp, "    {\n");
    fprintf(fp, "        printf(\"Error: no input files.\");\n");
    fprintf(fp, "        return 0;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    FILE *fp = fopen(argv[1], \"rb\");\n");
    fprintf(fp, "    if (fp == NULL)\n");
    fprintf(fp, "    {\n");
    fprintf(fp, "        printf(\"Error: can't locate file: %%s.\", argv[1]);\n");
    fprintf(fp, "        return 0;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    fseek(fp, 0, SEEK_END);\n");
    fprintf(fp, "    int len, wlen;\n");
    fprintf(fp, "    len = ftell(fp);\n");
    fprintf(fp, "    rewind(fp);\n");
    fprintf(fp, "    char *buf = (char *)malloc(sizeof(char) * (len + 1));\n");
    fprintf(fp, "    fread(buf, 1, len, fp);\n");
    fprintf(fp, "    buf[len] = 0;\n");
    fprintf(fp, "    wlen = mbstowcs(NULL, buf, len);\n");
    fprintf(fp, "    wchar_t *wbuf = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));\n");
    fprintf(fp, "    mbstowcs(wbuf, buf, len);\n");
    fprintf(fp, "    wbuf[wlen] = L'\\0';\n");
    fprintf(fp, "    fclose(fp);\n");
    fprintf(fp, "    transition_table tr_t(tr_buf);\n");
    fprintf(fp, "    lr1_parsing_table lr_t(lr_buf);\n");
    fprintf(fp, "    list<token_info> tk;\n");
    fprintf(fp, "    bool suc = tr_t.token_stream(wbuf, tk);\n");
    fprintf(fp, "    suc &= lr_t.postfixtrans(tk, f_list, ft_list);\n");
    fprintf(fp, "    printf(suc ? \"true\\n\" : \"false\\n\");\n");
    fprintf(fp, "    return 0;\n");
    fprintf(fp, "}\n");
    void *tb_buf;
    int sz;
    sz = tr_t.store();
    tb_buf = malloc(sz);
    tr_t.store(tb_buf);
    fprintf(fp, "\nunsigned char tr_buf[] = {\n");
    for (int i = 0; i < sz; i++)
    {
        if (i % 16 == 0)
            fprintf(fp, "    ");
        fprintf(fp, "0x%02x", ((unsigned char *)tb_buf)[i]);
        if (i < sz - 1)
            fprintf(fp, (i + 1) % 16 ? ", " : ",\n");
    }
    fprintf(fp, "};\n");
    free(tb_buf);
    sz = lr_t.store();
    tb_buf = malloc(sz);
    lr_t.store(tb_buf);
    fprintf(fp, "\nunsigned char lr_buf[] = {\n");
    for (int i = 0; i < sz; i++)
    {
        if (i % 16 == 0)
            fprintf(fp, "    ");
        fprintf(fp, "0x%02x", ((unsigned char *)tb_buf)[i]);
        if (i < sz - 1)
            fprintf(fp, (i + 1) % 16 ? ", " : ",\n");
    }
    fprintf(fp, "};\n");
    free(tb_buf);
    fclose(fp);
    printf("Generate successfully!\n");
    return 0;
}
int int_len(int x)
{
    if (x == 0)
        return 1;
    int l = 0;
    while (x)
    {
        x /= 10;
        l++;
    }
    return l;
}
void print(expr e, FILE *fp)
{
    if (fp == NULL)
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
    else
    {
        fprintf(fp, "%u = ", e.lhs);
        for (int i = 0; i < e.rhs.size(); i++)
            switch (e.rhs[i])
            {
            case OP_ALTER:
                fprintf(fp, "|");
                break;
            case OP_CMPLM:
                fprintf(fp, "~");
                break;
            case OP_CNCAT:
                fprintf(fp, ".");
                break;
            case OP_KLCLS:
                fprintf(fp, "*");
                break;
            case OP_LPRTH:
                fprintf(fp, "(");
                break;
            case OP_MINUS:
                fprintf(fp, "-");
                break;
            case OP_OPTNL:
                fprintf(fp, "?");
                break;
            case OP_PTCLS:
                fprintf(fp, "+");
                break;
            case OP_RPRTH:
                fprintf(fp, ")");
                break;
            case EPSILON:
                fprintf(fp, "#");
                break;
            case ENDMARK:
                fprintf(fp, "$");
                break;
            default:
                fprintf(fp, "%u", e.rhs[i]);
            }
        fprintf(fp, "\n");
    }
}
void print(list<expr> l, FILE *fp)
{
    for (int i = 0; i < l.size(); i++)
        print(l[i], fp);
}
void print(list<wchar_t> name, FILE *fp)
{
    for (int j = 0; j < name.size(); j++)
    {
        wchar_t wstr[2] = {(unsigned short)name[j], 0};
        char str[8] = "\0\0\0\0\0\0\0";
        wcstombs(str, wstr, 8);
        if (fp == NULL)
            printf("%s", str);
        else
            fprintf(fp, "%s", str);
    }
}
void print(list<list<wchar_t>> names, list<unsigned int> types, FILE *fp,
           const char *pre, const char *suf, int len, int star_size)
{
    for (int i = 0; i < names.size(); i++)
    {
        for (int j = 1; j < names[i].size(); j++)
            if (names[i][j - 1] == '*' && names[i][j] == '/')
                names[i][j] = '|';
        if (fp != NULL && pre != NULL)
            fprintf(fp, "%s", pre);
        if (fp == NULL)
            printf("%4d", i);
        else
            fprintf(fp, "%4d", i);
        for (int j = 0; j < len + 2 - names[i].size(); j++)
            if (fp == NULL)
                printf(" ");
            else
                fprintf(fp, " ");
        print(names[i], fp);
        if (types[i] == TERMINAL)
            if (fp == NULL)
                printf("   T");
            else
                fprintf(fp, "   T");
        else if (types[i] == VARIABLE)
            if (fp == NULL)
                printf("   V");
            else
                fprintf(fp, "   V");
        else if (fp == NULL)
            printf("   I");
        else
            fprintf(fp, "   I");
        if (fp != NULL && suf != NULL)
        {
            for (int j = 0; j < star_size - len - 12; j++)
                fprintf(fp, " ");
            fprintf(fp, "%s", suf);
        }
        if (fp != NULL)
            fprintf(fp, "\n");
        else
            printf("\n");
    }
}
void print(grammar g, list<list<wchar_t>> names, FILE *fp,
           const char *pre, const char *suf, int len)
{
    for (int i = 0; i < g.productions.size(); i++)
    {
        int l = 2;
        if (g.productions[i].variable < (unsigned int)names.size())
            l += names[g.productions[i].variable].size() + 1;
        else if (g.productions[i].variable == AUGMNTED)
            l += 4;
        else
            l += 5 + int_len(g.productions[i].variable);
        if (fp != NULL && pre != NULL)
            fprintf(fp, pre);
        if (g.productions[i].variable < (unsigned int)names.size())
            print(names[g.productions[i].variable], fp);
        else if (fp == NULL)
            printf("Temp%u", g.productions[i].variable);
        else if (g.productions[i].variable == AUGMNTED)
            fprintf(fp, "Aug");
        else
            fprintf(fp, "Temp%u", g.productions[i].variable);
        if (fp == NULL)
            printf(" ::= ");
        else
            fprintf(fp, " ::= ");
        for (int j = 0; j < g.productions[i].expression.size(); j++)
        {
            if (fp == NULL)
                printf(j == 0 ? "" : "| ");
            else
                fprintf(fp, j == 0 ? "" : "| ");
            l += 2;
            if (g.productions[i].expression[j].size() == 0)
                if (fp == NULL)
                    printf("#E ");
                else
                    fprintf(fp, "#E ");
            if (g.productions[i].expression[j].empty())
                l += 3;
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
            {
                if (g.productions[i].expression[j][k] < (unsigned int)names.size())
                    l += names[g.productions[i].expression[j][k]].size() + 1;
                else if (g.productions[i].expression[j][k] == AUGMNTED)
                    l += 4;
                else
                    l += 5 + int_len(g.productions[i].expression[j][k]);
                if (g.productions[i].expression[j][k] < (unsigned int)names.size())
                    print(names[g.productions[i].expression[j][k]], fp);
                else if (fp == NULL)
                    printf("Temp%u", g.productions[i].expression[j][k]);
                else
                    fprintf(fp, "Temp%u", g.productions[i].expression[j][k]);
                if (fp == NULL)
                    printf(" ");
                else
                    fprintf(fp, " ");
            }
        }
        if (fp != NULL && suf != NULL)
        {
            for (int j = 0; j < len - l; j++)
                fprintf(fp, " ");
            fprintf(fp, "%s", suf);
        }
        if (fp == NULL)
            printf("\n");
        else
            fprintf(fp, "\n");
    }
}
void print(list<unsigned int> l, bool ch, FILE *fp)
{
    for (int i = 0; i < l.size(); i++)
        if (l[i] == EPSILON)
            if (fp == NULL)
                printf("# ");
            else
                fprintf(fp, "# ");
        else if (l[i] == TERMINAL)
            if (fp == NULL)
                printf("$ ");
            else
                fprintf(fp, "$ ");
        else if (ch)
        {
            wchar_t wstr[2] = {(unsigned short)l[i], 0};
            char str[8] = "\0\0\0\0\0\0\0";
            wcstombs(str, wstr, 8);
            if (fp == NULL)
                printf("%s", str);
            else
                fprintf(fp, "%s", str);
        }
        else if (fp == NULL)
            printf("%u ", l[i]);
        else
            fprintf(fp, "%u ", l[i]);
    if (!ch)
        if (fp == NULL)
            printf("\n");
        else
            fprintf(fp, "\n");
}
void print(list<int> l, FILE *fp)
{
    for (int i = 0; i < l.size(); i++)
        if (fp == NULL)
            printf("%d ", l[i]);
        else
            fprintf(fp, "%d ", l[i]);
    if (fp == NULL)
        printf("\n");
    else
        fprintf(fp, "\n");
}
