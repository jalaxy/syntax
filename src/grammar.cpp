/****************************************************************
 * @file grammar.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Function definition of grammar.h
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#include "grammar.h"

struct hash_string_info
{
    int idx;
    list<wchar_t> str;
};

/**
 * @brief Hash function
 * 
 * @param l the string
 * @return hash value
 */
int h(list<wchar_t> &l)
{
    int ans = 0;
    for (int i = 0; i < l.size(); i++)
        ans = (ans * 0x80 + l[i]) % HASH_SZ;
    return ans;
}

/**
 * @brief produce augmented grammar
 * 
 */
void grammar::augment()
{
    productions.push_front({AUGMNTED, list<list<unsigned int>>()});
    productions.front().expression.push_back(list<unsigned int>());
    productions.front().expression.top().push_back(s->variable);
    s = &productions.front();
}

/**
 * @brief Get the next unsigned integer
 * 
 * @param str the string
 * @param wsz the size of the string
 * @param i start index
 * @param num integer output
 * @return index of position after this number
 */
int GetInt(wchar_t *str, int wsz, int i, unsigned int &num)
{
    int j = i;
    num = 0;
    while (j < wsz &&
           ((str[j] >= L'0' && str[j] <= L'9') ||
            (str[j] >= L'a' && str[j] <= L'f') ||
            (str[j] >= L'A' && str[j] <= L'F')))
    {
        num *= 16;
        if (str[j] >= L'a')
            num += str[j] - L'a' + 10;
        else if (str[j] >= L'A')
            num += str[j] - L'A' + 10;
        else if (str[j] >= L'0')
            num += str[j] - L'0';
        j++;
    }
    return j;
}

/**
 * @brief Determine set of the character according to the separation
 * 
 * @param sep the separation
 * @param ch the character to determine
 * @return the index of sets
 */
int DetermineTerminal(list<unsigned int> &sep, unsigned int ch)
{
    int idx = 0;
    sep.append(0x110000);
    if ((unsigned int)ch >= sep[0])
    {
        int l = 0, r = sep.size();
        while (l + 1 < r)
        {
            int mid = (l + r) / 2;
            if (sep[mid] <= (unsigned int)ch)
                l = mid;
            else
                r = mid;
        }
        idx = l + 1;
    }
    sep.pop_back();
    return idx;
}

/**
 * @brief Read an EBNF from a file
 * 
 * @param wbuf the wide character string
 * @param ebnflist result EBNF list
 * @param start result start variable
 * @param relist result regular language list
 * @param sep separations of UNICODE characters
 * @param names the corresponding name string of each index
 * @param types the types of each symbol
 * 
 * @return the line number
 */
int ReadEBNFFromString(wchar_t *wbuf, list<expr> &ebnflist, unsigned int &start, list<expr> &relist,
                       list<unsigned int> &sep, list<list<wchar_t>> &names, list<unsigned int> &types)
{
    int wsz = 0;
    while (wbuf[wsz] != L'\0')
        wsz++;
    // some extra space is reserved for inline-RE ordering (0 - sz_rsvd)
    int i = 0, line = 1, sz_rsvd = 0;
    names = list<list<wchar_t>>();
    while (i < wsz)
        if (wbuf[i] == L'[')
        {
            i++;
            if (i < wsz && wbuf[i] == L'^')
                i++;
            int j = i;
            while (j < wsz && wbuf[j] != L'\n' && wbuf[j] != L']')
                j++;
            if (j == wsz || wbuf[j] == L'\n')
                return line;
            types.append(TERMINAL);
            names.append(list<wchar_t>());
            for (int k = i - 1; k <= j; k++)
                names[sz_rsvd].append(wbuf[k]);
            sz_rsvd++;
            int k = i;
            while (k < j)
            {
                unsigned int l, r;
                l = wbuf[k];
                if (k + 1 < j && wbuf[k] == L'#' && wbuf[k + 1] == L'x')
                    k = GetInt(wbuf, wsz, k + 2, l);
                else
                    k++;
                if (wbuf[k] == L'-')
                {
                    k++;
                    r = wbuf[k];
                    if (k + 1 < j && wbuf[k] == L'#' && wbuf[k + 1] == L'x')
                        k = GetInt(wbuf, wsz, k + 2, r);
                    else
                        k++;
                }
                else
                    r = l;
                sep.append(l);
                sep.append(r + 1);
            }
            i = j + 1;
        }
        else if (wbuf[i] == L'#')
            if (i + 1 < wsz && wbuf[i + 1] == L'x')
            {
                unsigned int num;
                int j = GetInt(wbuf, wsz, i + 2, num);
                sep.append(num);
                sep.append(num + 1);
                types.append(TERMINAL);
                names.append(list<wchar_t>());
                for (int k = i; k < j; k++)
                    names[sz_rsvd].append(wbuf[k]);
                sz_rsvd++;
                i = j;
            }
            else
                return line;
        else if (wbuf[i] == '\'' || wbuf[i] == '"')
        {
            i++;
            int j = i;
            while (j < wsz && wbuf[j] != L'\n' && wbuf[j] != wbuf[i - 1])
                j++;
            if (j < wsz && wbuf[j] != L'\n')
                for (int k = i; k < j; k++)
                {
                    sep.append(wbuf[k]);
                    sep.append(wbuf[k] + 1);
                }
            else
                return line;
            types.append(TERMINAL);
            names.append(list<wchar_t>());
            for (int k = i - 1; k <= j; k++)
                names[sz_rsvd].append(wbuf[k]);
            sz_rsvd++;
            i = j + 1;
        }
        else if (wbuf[i++] == '\n')
            line++;
    sep.set_reduce();
    start = sz_rsvd;
    int j = i = line = 0;
    while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                       wbuf[i] == L'\t' || wbuf[i] == L'\r'))
        i++;
    unsigned int type = VARIABLE;
    while (i < wsz)
    {
        line++;
        j = i;
        while (j < wsz && wbuf[j] != L'\n')
            j++;
        // [i, j) is one EBNF expression
        if (j > i + 8 && wbuf[i + 8] == L':')
        {
            if (wbuf[i + 0] == L'v' && wbuf[i + 1] == L'a' && wbuf[i + 2] == L'r' &&
                wbuf[i + 3] == L'i' && wbuf[i + 4] == L'a' && wbuf[i + 5] == L'b' &&
                wbuf[i + 6] == L'l' && wbuf[i + 7] == L'e')
            {
                type = VARIABLE;
                i = j + 1;
                while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                                   wbuf[i] == L'\t' || wbuf[i] == L'\r'))
                    i++;
                continue;
            }
            else if (wbuf[i + 0] == L't' && wbuf[i + 1] == L'e' && wbuf[i + 2] == L'r' &&
                     wbuf[i + 3] == L'm' && wbuf[i + 4] == L'i' && wbuf[i + 5] == L'n' &&
                     wbuf[i + 6] == L'a' && wbuf[i + 7] == L'l')
            {
                type = TERMINAL;
                i = j + 1;
                while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                                   wbuf[i] == L'\t' || wbuf[i] == L'\r'))
                    i++;
                continue;
            }
        }
        if (j > i + 6 && wbuf[i + 6] == L':' &&
            wbuf[i + 0] == L'i' && wbuf[i + 1] == L'g' && wbuf[i + 2] == L'n' &&
            wbuf[i + 3] == L'o' && wbuf[i + 4] == L'r' && wbuf[i + 5] == L'e')
        {
            type = IGNORSYM;
            i = j + 1;
            while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                               wbuf[i] == L'\t' || wbuf[i] == L'\r'))
                i++;
            continue;
        }
        int k = i;
        while (k + 2 < wsz && !(wbuf[k] == ':' && wbuf[k + 1] == ':' && wbuf[k + 2] == '='))
            k++;
        if (k + 2 == wsz)
            return line;
        types.append(type);
        names.append(list<wchar_t>());
        for (int l = i; l < k; l++)
            if (wbuf[l] != L' ' && wbuf[l] != L'\t' && wbuf[l] != L'\r')
                names.top().append(wbuf[l]);
        i = j + 1;
        while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                           wbuf[i] == L'\t' || wbuf[i] == L'\r'))
            i++;
    }
    list<hash_string_info> *aidx = new (std::nothrow) list<hash_string_info>[HASH_SZ];
    if (aidx == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return -2;
    }
    for (int i = 0; i < names.size(); i++)
    {
        int hv = h(names[i]), j;
        for (j = 0; j < aidx[hv].size(); j++)
            if (aidx[hv][j].str == names[i])
                break;
        if (j == aidx[hv].size())
            aidx[hv].append({i, names[i]});
    }
    relist = list<expr>();
    for (int i = 0; i <= sep.size(); i++)
    {
        list<unsigned int> RHS;
        RHS.append((unsigned int)i);
        relist.append({(unsigned int)(names.size() + i), RHS});
    }
    unsigned int id_rsvd = 0;
    for (i = 0; i < sz_rsvd; i++)
        ebnflist.append({id_rsvd++, list<unsigned int>()});
    i = j = line = 0;
    id_rsvd = 0;
    while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                       wbuf[i] == L'\t' || wbuf[i] == L'\r'))
        i++;
    while (i < wsz)
    {
        list<wchar_t> lhs_name;
        line++;
        j = i;
        while (j < wsz && wbuf[j] != L'\n')
            j++;
        // [i, j) is one EBNF expression
        if ((j > i + 8 && wbuf[i + 8] == L':' &&
             ((wbuf[i + 0] == L'v' && wbuf[i + 1] == L'a' && wbuf[i + 2] == L'r' &&
               wbuf[i + 3] == L'i' && wbuf[i + 4] == L'a' && wbuf[i + 5] == L'b' &&
               wbuf[i + 6] == L'l' && wbuf[i + 7] == L'e') ||
              (wbuf[i + 0] == L't' && wbuf[i + 1] == L'e' && wbuf[i + 2] == L'r' &&
               wbuf[i + 3] == L'm' && wbuf[i + 4] == L'i' && wbuf[i + 5] == L'n' &&
               wbuf[i + 6] == L'a' && wbuf[i + 7] == L'l'))) ||
            (j > i + 6 && wbuf[i + 6] == L':' &&
             wbuf[i + 0] == L'i' && wbuf[i + 1] == L'g' && wbuf[i + 2] == L'n' &&
             wbuf[i + 3] == L'o' && wbuf[i + 4] == L'r' && wbuf[i + 5] == L'e'))
        {
            i = j + 1;
            while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                               wbuf[i] == L'\t' || wbuf[i] == L'\r'))
                i++;
            continue;
        }
        int k = i;
        while (k + 2 < j && !(wbuf[k] == ':' && wbuf[k + 1] == ':' && wbuf[k + 2] == '='))
            k++;
        if (k + 2 == j)
            return line;
        for (int l = i; l < k; l++)
            if (wbuf[l] != L' ' && wbuf[l] != L'\t' && wbuf[l] != L'\r')
                lhs_name.append(wbuf[l]);
        int hv = h(lhs_name), lhs;
        for (lhs = 0; lhs < aidx[hv].size(); lhs++)
            if (aidx[hv][lhs].str == lhs_name)
                break;
        lhs = aidx[hv][lhs].idx;
        k += 3;
        list<unsigned int> rhs;
        list<wchar_t> name;
        bool reverse;
        list<unsigned int> terminals; // terminal symbol range
        int l, m;                     // loop variables
        while (k < j)
        {
            if (wbuf[k] == L' ' || wbuf[k] == L'\t' || wbuf[k] == L'#' || wbuf[k] == L'[' ||
                wbuf[k] == L'\'' || wbuf[k] == L'"' || wbuf[k] == L'(' || wbuf[k] == L')' ||
                wbuf[k] == L'?' || wbuf[k] == L'|' || wbuf[k] == L'-' || wbuf[k] == L'+' ||
                wbuf[k] == L'*')
            {
                if ((!name.empty() || wbuf[k] == L'(' || wbuf[k] == L'[' ||
                     wbuf[k] == L'#' || wbuf[k] == L'\'' || wbuf[k] == L'"') &&
                    (!rhs.empty() &&
                     (rhs.top() < OP_LPRTH || rhs.top() == OP_RPRTH ||
                      rhs.top() == OP_KLCLS || rhs.top() == OP_OPTNL ||
                      rhs.top() == OP_PTCLS))) // is not a operator
                    rhs.append(OP_CNCAT);
                if (!name.empty())
                {
                    int l;
                    hv = h(name);
                    for (l = 0; l < aidx[hv].size(); l++)
                        if (aidx[hv][l].str == name)
                            break;
                    if (l == aidx[hv].size())
                        return line;
                    rhs.append((unsigned int)aidx[hv][l].idx);
                    name = list<wchar_t>();
                }
            }
            switch (wbuf[k])
            {
            case L'\r': // carriage return
                k++;
                break;
            case L' ':
            case L'\t': // space characters
                k++;
                break;
            case L'#': // unicode characters
                if (k + 1 < j && wbuf[k + 1] == L'x')
                {
                    unsigned int num;
                    k = GetInt(wbuf, wsz, k + 2, num);
                    ebnflist[id_rsvd].rhs.append(DetermineTerminal(sep, num) + names.size());
                    hv = h(names[id_rsvd]);
                    for (l = 0; l < aidx[hv].size(); l++)
                        if (aidx[hv][l].str == names[id_rsvd])
                            break;
                    rhs.append(aidx[hv][l].idx);
                    id_rsvd++;
                }
                else
                    name.append(wbuf[k++]);
                break;
            case L'[': // optional charcters
                k++;
                if ((reverse = k < j && wbuf[k] == L'^'))
                    k++;
                terminals = list<unsigned int>();
                l = k;
                while (l < j && wbuf[l] != L']')
                    l++;
                m = k; // m in [k, l)
                while (m < l)
                {
                    unsigned int left, right;
                    left = wbuf[m];
                    if (m + 1 < l && wbuf[m] == L'#' && wbuf[m + 1] == L'x')
                        m = GetInt(wbuf, wsz, m + 2, left);
                    else
                        m++;
                    if (wbuf[m] == L'-')
                    {
                        m++;
                        right = wbuf[m];
                        if (m + 1 < l && wbuf[m] == L'#' && wbuf[m + 1] == L'x')
                            m = GetInt(wbuf, wsz, m + 2, right);
                        else
                            m++;
                    }
                    else
                        right = left;
                    for (int n = DetermineTerminal(sep, left); n <= DetermineTerminal(sep, right); n++)
                        terminals.append(n + names.size());
                }
                terminals.set_reduce();
                if (reverse)
                {
                    int p = 0;
                    list<unsigned int> terminals_r;
                    for (unsigned int q = 0; q < (unsigned int)relist.size(); q++)
                        if (p < terminals.size() && q + names.size() == terminals[p])
                            p++;
                        else
                            terminals_r.append(q + names.size());
                    terminals = terminals_r;
                }
                if (terminals.empty())
                    ebnflist[id_rsvd].rhs.append(EPSILON);
                else
                {
                    ebnflist[id_rsvd].rhs.append(terminals[0]);
                    for (m = 1; m < terminals.size(); m++)
                    {
                        ebnflist[id_rsvd].rhs.append(OP_ALTER);
                        ebnflist[id_rsvd].rhs.append(terminals[m]);
                    }
                }
                hv = h(names[id_rsvd]);
                for (m = 0; m < aidx[hv].size(); m++)
                    if (aidx[hv][m].str == names[id_rsvd])
                        break;
                rhs.append(aidx[hv][m].idx);
                id_rsvd++;
                k = l + 1;
                break;
            case L'\'':
            case L'"': // quoted strings
                k++;
                l = k;
                while (l < j && wbuf[l] != wbuf[k - 1])
                    l++;
                if (l < j)
                {
                    if (l == k)
                        ebnflist[id_rsvd].rhs.append(EPSILON);
                    else
                    {
                        if (l > k + 1)
                            ebnflist[id_rsvd].rhs.append(OP_LPRTH);
                        ebnflist[id_rsvd].rhs.append(
                            DetermineTerminal(sep, wbuf[k]) + names.size());
                        for (m = k + 1; m < l; m++)
                        {
                            ebnflist[id_rsvd].rhs.append(OP_CNCAT);
                            ebnflist[id_rsvd].rhs.append(
                                DetermineTerminal(sep, wbuf[m]) + names.size());
                        }
                        if (l > k + 1)
                            ebnflist[id_rsvd].rhs.append(OP_RPRTH);
                    }
                    hv = h(names[id_rsvd]);
                    for (m = 0; m < aidx[hv].size(); m++)
                        if (aidx[hv][m].str == names[id_rsvd])
                            break;
                    rhs.append(aidx[hv][m].idx);
                    id_rsvd++;
                    k = l + 1;
                }
                else
                    name.append(wbuf[k - 1]);
                break;
            case L'(': // left parentheses
                rhs.append(OP_LPRTH);
                k++;
                break;
            case L')': // right parentheses
                rhs.append(OP_RPRTH);
                k++;
                break;
            case L'?': // optional operator
                rhs.append(OP_OPTNL);
                k++;
                break;
            case L'|': // alternation operator
                rhs.append(OP_ALTER);
                k++;
                break;
            case L'-': // difference operator
                rhs.append(OP_MINUS);
                k++;
                break;
            case L'+': // positive closure operator
                rhs.append(OP_PTCLS);
                k++;
                break;
            case L'*': // Kleene closure operator
                rhs.append(OP_KLCLS);
                k++;
                break;
            default:
                name.append(wbuf[k++]);
                break;
            }
        }
        if (!name.empty() &&
            (!rhs.empty() &&
             (rhs.top() < OP_LPRTH || rhs.top() == OP_RPRTH ||
              rhs.top() == OP_KLCLS || rhs.top() == OP_OPTNL ||
              rhs.top() == OP_PTCLS))) // is not a operator
            rhs.append(OP_CNCAT);
        if (!name.empty())
        {
            int l;
            int hv = h(name);
            for (l = 0; l < aidx[hv].size(); l++)
                if (aidx[hv][l].str == name)
                    break;
            if (l == aidx[hv].size())
                return line;
            rhs.append((unsigned int)aidx[hv][l].idx);
            name = list<wchar_t>();
        }
        ebnflist.append({(unsigned int)lhs, rhs});
        i = j + 1;
        while (i < wsz && (wbuf[i] == L'\n' || wbuf[i] == L' ' ||
                           wbuf[i] == L'\t' || wbuf[i] == L'\r'))
            i++;
    }
    delete[] aidx;
    return 0;
}

/**
 * @brief Substitute the single production recursively
 * 
 * @param g the grammar to simplify
 * @param aidx hash table
 * @param idx current index
 * @param v visited array
 * @param id_s the additional symbol start id
 */
void SingleSubstitution(grammar &g, list<hash_symbol_info> *aidx, int idx, bool *v, unsigned int id_s)
{
    if (v[idx])
        return;
    v[idx] = true;
    for (int j = 0; j < g.productions[idx].expression.size(); j++)
    {
        for (int k = 0; k < g.productions[idx].expression[j].size(); k++)
        {
            unsigned int symbol = g.productions[idx].expression[j][k];
            if (symbol >= id_s)
                for (int l = 0; l < aidx[symbol % HASH_SZ].size(); l++)
                    if (aidx[symbol % HASH_SZ][l].symbol == symbol)
                    {
                        int idx_k = aidx[symbol % HASH_SZ][l].idx;
                        SingleSubstitution(g, aidx, idx_k, v, id_s);
                        if (g.productions[idx_k].expression.size() == 1 && idx_k != idx)
                        {
                            g.productions[idx].expression[j].remove(k--);
                            for (int m = 0; m < g.productions[idx_k].expression.top().size(); m++)
                                g.productions[idx].expression[j].insert(
                                    k++, g.productions[idx_k].expression.top()[m]);
                        }
                        break;
                    }
        }
        if (g.productions[idx].expression[j].size() == 1 &&
            g.productions[idx].expression[j].top() >= id_s &&
            g.productions[idx].expression[j].top() != g.productions[idx].variable)
        {
            unsigned int symbol = g.productions[idx].expression[j].top();
            for (int k = 0; k < aidx[symbol % HASH_SZ].size(); k++)
            {
                hash_symbol_info info = aidx[symbol % HASH_SZ][k];
                if (info.symbol == symbol)
                {
                    g.productions[idx].expression.remove(j--);
                    for (int l = 0; l < g.productions[info.idx].expression.size(); l++)
                        g.productions[idx].expression.insert(
                            j++, g.productions[info.idx].expression[l]);
                }
            }
        }
    }
}

/**
 * @brief Traverse the grammar to eliminate unreachable variables
 * 
 * @param g the grammar
 * @param aidx hash table
 * @param symbol current symbol
 * @param v visited array
 */
void TraverseGrammar(grammar &g, list<hash_symbol_info> *aidx, unsigned int symbol, bool *v)
{
    for (int i = 0; i < aidx[symbol % HASH_SZ].size(); i++)
        if (aidx[symbol % HASH_SZ][i].symbol == symbol)
        {
            int idx = aidx[symbol % HASH_SZ][i].idx;
            if (v[idx])
                return;
            v[idx] = true;
            for (int j = 0; j < g.productions[idx].expression.size(); j++)
                for (int k = 0; k < g.productions[idx].expression[j].size(); k++)
                    TraverseGrammar(g, aidx, g.productions[idx].expression[j][k], v);
            break;
        }
}

/**
 * @brief Convert EBNF to grammar
 * 
 * @param ebnflist the EBNF list
 * @param g the output grammar
 * @return error index
 */
int EBNFToGrammar(list<expr> ebnflist, grammar &g, unsigned int s, unsigned int id)
{
    for (int i = 0; i < ebnflist.size(); i++)
    {
        g.productions.append({ebnflist[i].lhs, list<list<unsigned int>>()});
        if (ebnflist[i].lhs == s)
            g.s = &g.productions[i];
        if (ebnflist[i].lhs + 1 > id)
            id = ebnflist[i].lhs + 1;
    }
    int id_start = id;
    for (int idx = 0; idx < ebnflist.size(); idx++)
    {
        list<unsigned int> sym_stack;
        ebnflist[idx].rhs.append(OP_RPRTH);
        list<unsigned int> op_stack;
        op_stack.push_back(OP_LPRTH);
        int op_type = 0; // the next character type (operand: 0; operator: 1)
        for (int i = 0; i < ebnflist[idx].rhs.size(); i++)
        {
            unsigned int ch = ebnflist[idx].rhs[i];
            if ((ch == OP_RPRTH || ch == OP_KLCLS || ch == OP_CNCAT || ch == OP_ALTER ||
                         ch == OP_PTCLS || ch == OP_OPTNL || ch == OP_MINUS
                     ? 1
                     : 0) != op_type)
                return idx;
            op_type = ch == OP_LPRTH || ch == OP_CNCAT || ch == OP_ALTER ||
                              ch == OP_CMPLM || ch == OP_MINUS
                          ? 0
                          : 1;
            switch (ch)
            {
            case OP_LPRTH:
                op_stack.push_back(OP_LPRTH);
                break;
            case OP_RPRTH:
            case OP_KLCLS:
            case OP_PTCLS:
            case OP_OPTNL:
            case OP_CMPLM:
            case OP_CNCAT:
            case OP_ALTER:
            case OP_MINUS:
                while (ch < op_stack.top() ||
                       (ch == op_stack.top() && ch != OP_CMPLM))
                {
                    switch (op_stack.top())
                    {
                    case OP_KLCLS:
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // B -> AB
                        g.productions.top().expression.top().append(sym_stack.top());
                        g.productions.top().expression.top().append(g.productions.top().variable);
                        g.productions.top().expression.append(list<unsigned int>()); // B -> epsilon
                        sym_stack.top() = g.productions.top().variable;
                        break;
                    case OP_PTCLS:
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // B -> AB
                        g.productions.top().expression.top().append(sym_stack.top());
                        g.productions.top().expression.top().append(g.productions.top().variable);
                        g.productions.top().expression.append(list<unsigned int>()); // B -> A
                        g.productions.top().expression.top().append(sym_stack.top());
                        sym_stack.top() = g.productions.top().variable;
                        break;
                    case OP_OPTNL:
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // B -> A
                        g.productions.top().expression.top().append(sym_stack.top());
                        g.productions.top().expression.append(list<unsigned int>()); // B -> epsilon
                        sym_stack.top() = g.productions.top().variable;
                        break;
                    case OP_CNCAT:
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // C -> AB
                        g.productions.top().expression.top().append(sym_stack.undertop());
                        g.productions.top().expression.top().append(sym_stack.top());
                        sym_stack.pop_back();
                        sym_stack.top() = g.productions.top().variable;
                        break;
                    case OP_ALTER:
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // C -> A
                        g.productions.top().expression.top().append(sym_stack.undertop());
                        g.productions.top().expression.append(list<unsigned int>()); // C -> B
                        g.productions.top().expression.top().append(sym_stack.top());
                        sym_stack.pop_back();
                        sym_stack.top() = g.productions.top().variable;
                        break;
                    case OP_CMPLM:
                    case OP_MINUS:
                        return idx;
                    }
                    op_stack.pop_back();
                }
                if (ch == OP_RPRTH)
                    if (op_stack.empty() || op_stack.top() != OP_LPRTH)
                        return idx;
                    else
                        op_stack.pop_back();
                else
                    op_stack.push_back(ch);
                break;
            default:
                sym_stack.push_back(ch);
                break;
            }
        }
        if (!op_stack.empty())
            return idx;
        g.productions[idx].expression.append(list<unsigned int>());
        g.productions[idx].expression.top().append(sym_stack.top());
        ebnflist[idx].rhs.pop_back();
    }
    list<hash_symbol_info> *aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ],
                           *aidx_opt = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    bool *v = (bool *)malloc(sizeof(bool) * g.productions.size());
    if (aidx == NULL || aidx_opt == NULL || v == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return -2;
    }
    for (int i = 0; i < g.productions.size(); i++)
    {
        aidx[g.productions[i].variable % HASH_SZ].append({false, i, g.productions[i].variable});
        v[i] = false;
    }
    for (int i = 0; i < g.productions.size(); i++)
        if (!v[i])
            SingleSubstitution(g, aidx, i, v, id_start);
    for (int i = 0; i < g.productions.size(); i++)
        v[i] = false;
    for (int i = 0; i < g.productions.size(); i++)
        if (g.productions[i].expression.size() == 2 &&
            g.productions[i].expression[0].size() == 1 &&
            g.productions[i].expression[1].size() == 0)
        {
            int j;
            list<hash_symbol_info> &info = aidx_opt[g.productions[i].expression[0].top() % HASH_SZ];
            for (j = 0; j < info.size(); j++)
                if (info[j].symbol == g.productions[i].expression[0].top())
                    break;
            if (j == info.size())
                info.append({false, i, g.productions[i].expression[0].top()});
            else
                for (int k = 0; k < g.productions.size(); k++)
                    for (int l = 0; l < g.productions[k].expression.size(); l++)
                        for (int m = 0; m < g.productions[k].expression[l].size(); m++)
                            if (g.productions[k].expression[l][m] == g.productions[i].variable)
                                g.productions[k].expression[l][m] =
                                    g.productions[info[j].idx].variable;
        }
    TraverseGrammar(g, aidx, s, v);
    list<int> rm_idx;
    for (int i = 0; i < g.productions.size(); i++)
        if (!v[i])
            rm_idx.append(i);
    for (int i = 0; i < rm_idx.size(); i++)
        g.productions.remove(rm_idx[i] - i);
    delete[] aidx;
    delete[] aidx_opt;
    free(v);
    return -1; // no error
}

/**
 * @brief Recursion part of conversion from EBNF to grammar
 * 
 * @param ebnflist the EBNF list
 * @param symbol current symbol
 * @param aidx hash table of symbol
 * @param relist old RE list
 * @param v visiting array
 * @return error index
 */
int EBNFToRERecursion(list<expr> &ebnflist, unsigned int symbol, list<hash_symbol_info> *aidx,
                      list<expr> &relist)
{
    int idx;
    hash_symbol_info *p_info = NULL;
    for (int i = 0; i < aidx[symbol % HASH_SZ].size(); i++)
        if (aidx[symbol % HASH_SZ][i].symbol == symbol)
        {
            p_info = &aidx[symbol % HASH_SZ][i];
            if (p_info->regular)
                return -1;
            idx = p_info->idx;
            break;
        }
    list<expr> expr_stack;
    ebnflist[idx].rhs.append(OP_RPRTH);
    list<unsigned int> op_stack;
    op_stack.push_back(OP_LPRTH);
    int op_type = 0; // the next character type (operand: 0; operator: 1)
    for (int i = 0; i < ebnflist[idx].rhs.size(); i++)
    {
        unsigned int ch = ebnflist[idx].rhs[i];
        int j;
        if ((ch == OP_RPRTH || ch == OP_KLCLS || ch == OP_CNCAT || ch == OP_ALTER ||
                     ch == OP_PTCLS || ch == OP_OPTNL || ch == OP_MINUS
                 ? 1
                 : 0) != op_type)
            return idx;
        op_type = ch == OP_LPRTH || ch == OP_CNCAT || ch == OP_ALTER ||
                          ch == OP_CMPLM || ch == OP_MINUS
                      ? 0
                      : 1;
        switch (ch)
        {
        case OP_LPRTH:
            op_stack.push_back(OP_LPRTH);
            break;
        case OP_RPRTH:
        case OP_KLCLS:
        case OP_PTCLS:
        case OP_OPTNL:
        case OP_CMPLM:
        case OP_CNCAT:
        case OP_ALTER:
        case OP_MINUS:
            while (ch < op_stack.top() ||
                   (ch == op_stack.top() && ch != OP_CMPLM))
            {
                list<unsigned int> ep;
                switch (op_stack.top())
                {
                case OP_KLCLS:
                    expr_stack.top() = *expr_stack.top();
                    break;
                case OP_PTCLS:
                    expr_stack.top() = expr_stack.top() << *expr_stack.top();
                    break;
                case OP_OPTNL:
                    ep.append(EPSILON);
                    expr_stack.top() = expr_stack.top() | expr({NON_ACC, ep});
                    break;
                case OP_CMPLM:
                    expr_stack.top() = ~expr_stack.top();
                    break;
                case OP_CNCAT:
                    expr_stack.undertop() = expr_stack.undertop() << expr_stack.top();
                    expr_stack.pop_back();
                    break;
                case OP_ALTER:
                    expr_stack.undertop() = expr_stack.undertop() | expr_stack.top();
                    expr_stack.pop_back();
                    break;
                case OP_MINUS:
                    expr_stack.undertop() = ~((~expr_stack.undertop()) | expr_stack.top());
                    expr_stack.pop_back();
                    break;
                }
                op_stack.pop_back();
            }
            if (ch == OP_RPRTH)
                if (op_stack.empty() || op_stack.top() != OP_LPRTH)
                    return idx;
                else
                    op_stack.pop_back();
            else
                op_stack.push_back(ch);
            break;
        default:
            for (j = 0; j < aidx[ch % HASH_SZ].size(); j++)
                if (aidx[ch % HASH_SZ][j].symbol == ch)
                {
                    hash_symbol_info &info = aidx[ch % HASH_SZ][j];
                    if (ch == symbol)
                        return idx;
                    int err = EBNFToRERecursion(ebnflist, ch, aidx, relist);
                    if (err != -1)
                        return err;
                    expr_stack.push_back(relist[info.idx]);
                    break;
                }
            if (j == aidx[ch % HASH_SZ].size())
                return idx;
            break;
        }
    }
    if (!op_stack.empty())
        return idx;
    ebnflist[idx].rhs.pop_back();
    p_info->regular = true;
    p_info->idx = relist.size();
    expr_stack.top().lhs = p_info->symbol;
    relist.append(expr_stack.top());
    return -1;
}

/**
 * @brief Expand EBNF to RE
 * 
 * @param ebnflist the EBNF list
 * @param relist original RE list
 * @return error index
 */
int EBNFToRE(list<expr> ebnflist, list<expr> &relist)
{
    list<hash_symbol_info> *aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    if (aidx == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return -2;
    }
    for (int i = 0; i < relist.size(); i++)
        aidx[relist[i].lhs % HASH_SZ].append({true, i, relist[i].lhs});
    for (int i = 0; i < ebnflist.size(); i++)
        aidx[ebnflist[i].lhs % HASH_SZ].append({false, i, ebnflist[i].lhs});
    for (int i = 0; i < ebnflist.size(); i++)
        if (EBNFToRERecursion(ebnflist, ebnflist[i].lhs, aidx, relist) != -1)
            return i;
    delete[] aidx;
    return -1;
}

/**
 * @brief Remove unreachable RE
 * 
 * @param relist RE list
 * @param g the grammar that refers to the RE list
 */
void EliminateUnreachableRE(list<expr> &relist, grammar g, list<unsigned int> types)
{
    list<hash_symbol_info> *aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    bool *v = (bool *)malloc(sizeof(bool) * relist.size());
    if (aidx == NULL || v == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return;
    }
    for (int i = 0; i < relist.size(); i++)
    {
        aidx[relist[i].lhs % HASH_SZ].append({true, i, relist[i].lhs});
        v[i] = false;
    }
    for (int i = 0; i < g.productions.size(); i++)
        aidx[g.productions[i].variable % HASH_SZ].append({false, i, g.productions[i].variable});
    for (int i = 0; i < g.productions.size(); i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
            {
                unsigned int symbol = g.productions[i].expression[j][k];
                int l;
                for (l = 0; l < aidx[symbol % HASH_SZ].size(); l++)
                    if (aidx[symbol % HASH_SZ][l].symbol == symbol)
                        break;
                if (l == aidx[symbol % HASH_SZ].size())
                    return;
                if (aidx[symbol % HASH_SZ][l].regular)
                    v[aidx[symbol % HASH_SZ][l].idx] = true;
            }
    list<int> rm_idx;
    for (int i = 0; i < relist.size(); i++)
        if (!(v[i] || (relist[i].lhs < (unsigned int)types.size() && types[relist[i].lhs] == IGNORSYM)))
            rm_idx.append(i);
    for (int i = 0; i < rm_idx.size(); i++)
        relist.remove(rm_idx[i] - i);
    delete[] aidx;
    free(v);
}
