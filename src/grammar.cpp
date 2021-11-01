/****************************************************************
 * @file grammar.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Function definitions of grammar.h
 * @version 0.1
 * @date 2021-10-24
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#include "grammar.h"

/**
 * @brief Copy from another object
 * 
 * @param b another object
 */
void ll1_parsing_table::copy(const ll1_parsing_table &b)
{
    row = b.row;
    col = b.col;
    s = b.s;
    if (row * col == 0)
    {
        table = NULL;
        aidx = NULL;
    }
    else
    {
        table = new (std::nothrow) list<unsigned int>[row * col];
        aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
        if (table == NULL || aidx == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                table[i * col + j] = b.table[i * col + j];
        for (int i = 0; i < HASH_SZ; i++)
            aidx[i] = b.aidx[i];
    }
}

/**
 * @brief Construct a new LL(1) parsing table object from a CFG
 * 
 */
ll1_parsing_table::ll1_parsing_table(grammar g)
{
    row = g.productions.size();
    col = 0;
    aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    if (aidx == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    for (int i = 0; i < row; i++)
        aidx[g.productions[i].variable % HASH_SZ].append({false, i, g.productions[i].variable});
    list<unsigned int> re_sym;
    for (int i = 0; i < row; i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
                if (query_symbol(g.productions[i].expression[j][k]) == NULL)
                    re_sym.append(g.productions[i].expression[j][k]);
    re_sym.set_reduce();
    for (int i = 0; i < re_sym.size(); i++)
        aidx[re_sym[i] % HASH_SZ].append({true, col++, re_sym[i]});
    table = new (std::nothrow) list<unsigned int>[row * col];
    if (table == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
            table[i * col + j].append(LL1_PARSING_ERROR);
    list<unsigned int> *first = new (std::nothrow) list<unsigned int>[row],
                       *follow = new (std::nothrow) list<unsigned int>[row];
    if (first == NULL || follow == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    for (int i = 0; i < row; i++)
        if (first[i].empty())
            calc_first(g, first, g.productions[i].variable);
    // B -> beta A alpha    =>  FOLLOW(A) |= FIRST(alpha) | (epsilon in alpha ? FOLLOW(B))
    // B -> beta A          =>  FOLLOW(A) |= FOLLOW(B)
    bool finished = false;
    while (!finished)
    {
        finished = true;
        for (int i = 0; i < g.productions.size(); i++)
            for (int j = 0; j < g.productions[i].expression.size(); j++)
            {
                list<unsigned int> cur_first;
                cur_first.append(EPSILON);
                for (int k = g.productions[i].expression[j].size() - 1; k >= 0; k--)
                {
                    const hash_symbol_info *p_info = query_symbol(g.productions[i].expression[j][k]);
                    if (p_info->regular)
                    {
                        cur_first = list<unsigned int>();
                        cur_first.append(p_info->symbol);
                    }
                    else
                    {
                        int sz = follow[p_info->idx].size();
                        follow[p_info->idx] += cur_first;
                        if (cur_first.top() == EPSILON)
                        {
                            follow[p_info->idx].pop_back();
                            follow[p_info->idx] += follow[query_symbol(g.productions[i].variable)->idx];
                        }
                        if (first[p_info->idx].top() == EPSILON)
                        {
                            list<unsigned int> tmp = first[p_info->idx];
                            tmp.pop_back();
                            cur_first += tmp;
                            cur_first.set_reduce();
                        }
                        else
                            cur_first = first[p_info->idx];
                        follow[p_info->idx].set_reduce();
                        if (follow[p_info->idx].size() > sz)
                            finished = false;
                    }
                }
            }
    }
    for (int i = 0; i < g.productions.size(); i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
        {
            list<unsigned int> first_of_this;
            int k;
            for (k = 0; k < g.productions[i].expression[j].size(); k++)
            {
                const hash_symbol_info *p_info = query_symbol(g.productions[i].expression[j][k]);
                if (p_info->regular)
                    first_of_this.append(p_info->symbol);
                else
                    first_of_this += first[p_info->idx];
                if (first_of_this.top() == EPSILON)
                    first_of_this.pop_back();
                else
                    break;
            }
            if (k == g.productions[i].expression[j].size())
                first_of_this += follow[i];
            first_of_this.set_reduce();
            for (k = 0; k < first_of_this.size(); k++)
            {
                const hash_symbol_info *p_info = query_symbol(first_of_this[k]);
                if (table[i * col + p_info->idx].size() == 1 &&
                    table[i * col + p_info->idx].top() == LL1_PARSING_ERROR)
                    table[i * col + p_info->idx] = g.productions[i].expression[j];
                else
                {
                    delete[] first; // not an LL1 grammar
                    delete[] follow;
                    delete[] aidx;
                    delete[] table;
                    table = NULL;
                    aidx = NULL;
                    col = 0;
                    row = 0;
                    return;
                }
            }
        }
    delete[] first;
    delete[] follow;
    s = query_symbol(g.s->variable)->idx;
}

/**
 * @brief Calculate FIRST(variable) recursively
 * 
 * @param g the grammar
 * @param first array of FIRST
 * @param variable current variable
 */
void ll1_parsing_table::calc_first(
    grammar &g, list<unsigned int> *first, unsigned int variable)
{
    // FIRST(terminal) = {terminal}, FIRST(epsilon) = {epsilon}
    // FIRST(alpha_1 alpha_2 ... alpha_n) |=
    //     FIRST(alpha_1) |
    //         (epsilon in FIRST(alpha_1) ? (FIRST(alpha_2) |
    //             (epsilon in FIRST(alpha_2) ? (FIRST(alpha_3) |
    //                 ... |
    //                     epsilon in FIRST(alpha_n) ? epsilon))));
    const hash_symbol_info *p_info = query_symbol(variable);
    for (int i = 0; i < g.productions[p_info->idx].expression.size(); i++)
    {
        int j;
        for (j = 0; j < g.productions[p_info->idx].expression[i].size(); j++)
        {
            const hash_symbol_info *p_info_j =
                query_symbol(g.productions[p_info->idx].expression[i][j]);
            if (p_info_j->regular)
                first[p_info->idx].append(p_info_j->symbol);
            else
            {
                if (first[p_info_j->idx].empty())
                    calc_first(g, first, p_info_j->symbol);
                first[p_info->idx] += first[p_info_j->idx];
            }
            if (first[p_info->idx].top() == EPSILON)
                first[p_info->idx].pop_back();
            else
                break;
        }
        if (j == g.productions[p_info->idx].expression[i].size())
            first[p_info->idx].append(EPSILON);
    }
    first[p_info->idx].set_reduce();
}

/**
 * @brief Get infomation of a symbol
 * 
 * @param symbol the symbol to query
 * @return infomation including symbol type and index, NULL if not exist
 */
const hash_symbol_info *ll1_parsing_table::query_symbol(unsigned int symbol)
{
    for (int i = 0; i < aidx[symbol % HASH_SZ].size(); i++)
        if (aidx[symbol % HASH_SZ][i].symbol == symbol)
            return &aidx[symbol % HASH_SZ][i];
    return NULL;
}

/**
 * @brief Construct a new LL(1) parsing table object from another object
 * 
 * @param b another object
 */
ll1_parsing_table::ll1_parsing_table(const ll1_parsing_table &b) { copy(b); }

/**
 * @brief Destroy the LL(1) parsing table object
 * 
 */
ll1_parsing_table::~ll1_parsing_table()
{
    if (table != NULL)
        delete[] table;
    if (aidx != NULL)
        delete[] aidx;
}

/**
 * @brief Get the row of parsing table
 * 
 * @return int 
 */
int ll1_parsing_table::get_row() { return row; }

/**
 * @brief Get the column of parsing table
 * 
 * @return int 
 */
int ll1_parsing_table::get_col() { return col; }

/**
 * @brief Get the index of start variable
 * 
 * @return int 
 */
int ll1_parsing_table::get_start() { return s; }

/**
 * @brief Get the element of parsing table
 * 
 * @param idx the row index
 * @return pointer to first element of idx-th row
 */
list<unsigned int> *ll1_parsing_table::operator[](int idx) { return &table[idx * col]; }

/**
 * @brief Assignment function
 * 
 * @param b another object
 * @return referenece of this
 */
const ll1_parsing_table &ll1_parsing_table::operator=(const ll1_parsing_table &b)
{
    if (table != NULL)
        delete[] table;
    if (aidx != NULL)
        delete[] aidx;
    copy(b);
    return *this;
}

/**
 * @brief Recursion part of conversion from EBNF to grammar
 * 
 * @param ebnflist the list of ebnf expressions
 * @param relist the list of regular language, also to be appended with derived expressions
 * @param g the grammar to convert
 * @param pr current grammar production
 * @param id new id
 * @return whether successful converting to CFG
 */
bool EBNFToGrammarRecursion(
    list<expr> &ebnflist, list<expr> &relist, grammar &g, unsigned int idx,
    unsigned int &id, list<hash_symbol_info> *aidx, bool detect_regular)
{
    // the index of each production should be in productions.aux
    if (g.productions[idx].expression.empty() ||
        g.productions[idx].expression.top().empty() ||
        g.productions[idx].expression.top().top() != (unsigned int)(-2))
        return true;
    g.productions[idx].expression.pop_back();
    list<unsigned int> idx_stack;
    int len = 0;
    while (ebnflist[idx].rhs[len] != OP_TRMNL)
        len++;
    ebnflist[idx].rhs[len] = OP_RPRTH; // before restored, this ENBF cannot be copied!
    list<unsigned int> type_stack;
    list<unsigned int> op_stack;
    op_stack.push_back(OP_LPRTH);
    int op_type = 0; // the next character type (operand: 0; operator: 1)
    for (unsigned int *p = ebnflist[idx].rhs; !op_stack.empty(); p++)
    {
        if ((*p == OP_RPRTH || *p == OP_KLCLS || *p == OP_CNCAT || *p == OP_ALTER ||
                     *p == OP_PTCLS || *p == OP_OPTNL || *p == OP_MINUS
                 ? 1
                 : 0) != op_type)
            return false;
        op_type = *p == OP_LPRTH || *p == OP_CNCAT || *p == OP_ALTER ||
                          *p == OP_CMPLM || *p == OP_MINUS
                      ? 0
                      : 1;
        int i;
        switch (*p)
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
            while (*p < op_stack.top() ||
                   (*p == op_stack.top() && *p != OP_CMPLM))
            {
                switch (op_stack.top())
                {
                case OP_KLCLS:
                    if (type_stack.top() == 3 && detect_regular)
                    {
                        relist.append(*relist[idx_stack.top()]);
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.top() = relist.size() - 1;
                    }
                    else if (!detect_regular)
                    {
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // B -> AB
                        g.productions.top().expression.top().append(
                            type_stack.top() == 2 ? g.productions[idx_stack.top()].variable
                                                  : relist[idx_stack.top()].lhs);
                        g.productions.top().expression.top().append(
                            g.productions.top().variable);
                        g.productions.top().expression.append(list<unsigned int>()); // B -> epsilon
                        aidx[g.productions.top().variable % HASH_SZ].append(
                            {false, g.productions.size() - 1, g.productions.top().variable});
                        idx_stack.top() = g.productions.size() - 1;
                    }
                    break;
                case OP_PTCLS:
                    if (type_stack.top() == 3 && detect_regular)
                    {
                        relist.append((relist[idx_stack.top()]) << (*relist[idx_stack.top()]));
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.top() = relist.size() - 1;
                    }
                    else if (!detect_regular)
                    {
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // B -> AB
                        g.productions.top().expression.top().append(
                            type_stack.top() == 2 ? g.productions[idx_stack.top()].variable
                                                  : relist[idx_stack.top()].lhs);
                        g.productions.top().expression.top().append(
                            g.productions.top().variable);
                        g.productions.top().expression.append(list<unsigned int>()); // B -> A
                        g.productions.top().expression.top().append(
                            type_stack.top() == 2 ? g.productions[idx_stack.top()].variable
                                                  : relist[idx_stack.top()].lhs);
                        aidx[g.productions.top().variable % HASH_SZ].append(
                            {false, g.productions.size() - 1, g.productions.top().variable});
                        idx_stack.top() = g.productions.size() - 1;
                    }
                    break;
                case OP_OPTNL:
                    if (type_stack.top() == 3 && detect_regular)
                    {
                        unsigned int ep[] = {EPSILON, OP_TRMNL};
                        relist.append(relist[idx_stack.top()] | expr(NON_TERMINAL, ep));
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.top() = relist.size() - 1;
                    }
                    else if (!detect_regular)
                    {
                        g.productions.append({id++, list<list<unsigned int>>()});
                        g.productions.top().expression.append(list<unsigned int>()); // B -> A
                        g.productions.top().expression.top().append(
                            type_stack.top() == 2 ? g.productions[idx_stack.top()].variable
                                                  : relist[idx_stack.top()].lhs);
                        g.productions.top().expression.append(list<unsigned int>()); // B -> epsilon
                        aidx[g.productions.top().variable % HASH_SZ].append(
                            {false, g.productions.size() - 1, g.productions.top().variable});
                        idx_stack.top() = g.productions.size() - 1;
                    }
                    break;
                case OP_CMPLM:
                    if (type_stack.top() == 3 && type_stack.undertop() == 3 && detect_regular)
                    {
                        relist.append(~relist[idx_stack.top()]);
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.top() = relist.size() - 1;
                    }
                    else
                        return false;
                    break;
                case OP_CNCAT:
                    if (type_stack.top() == 3 && type_stack.undertop() == 3 && detect_regular)
                    {
                        relist.append(relist[idx_stack.undertop()] << relist[idx_stack.top()]);
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.pop_back();
                        type_stack.pop_back();
                        idx_stack.top() = relist.size() - 1;
                    }
                    else
                    {
                        if (!detect_regular)
                        {
                            g.productions.append({id++, list<list<unsigned int>>()});
                            g.productions.top().expression.append(list<unsigned int>()); // C -> AB
                            g.productions.top().expression.top().append(
                                type_stack.undertop() == 2 ? g.productions[idx_stack.undertop()].variable
                                                           : relist[idx_stack.undertop()].lhs);
                            g.productions.top().expression.top().append(
                                type_stack.top() == 2 ? g.productions[idx_stack.top()].variable
                                                      : relist[idx_stack.top()].lhs);
                            aidx[g.productions.top().variable % HASH_SZ].append(
                                {false, g.productions.size() - 1, g.productions.top().variable});
                        }
                        type_stack.pop_back();
                        idx_stack.pop_back();
                        type_stack.top() = 2;
                        idx_stack.top() = g.productions.size() - 1;
                    }
                    break;
                case OP_ALTER:
                    if (type_stack.top() == 3 && type_stack.undertop() == 3 && detect_regular)
                    {
                        relist.append(relist[idx_stack.undertop()] | relist[idx_stack.top()]);
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.pop_back();
                        type_stack.pop_back();
                        idx_stack.top() = relist.size() - 1;
                    }
                    else
                    {
                        if (!detect_regular)
                        {
                            g.productions.append({id++, list<list<unsigned int>>()});
                            g.productions.top().expression.append(list<unsigned int>()); // C -> A
                            g.productions.top().expression.top().append(
                                type_stack.undertop() == 2 ? g.productions[idx_stack.undertop()].variable
                                                           : relist[idx_stack.undertop()].lhs);
                            g.productions.top().expression.append(list<unsigned int>()); // C -> B
                            g.productions.top().expression.top().append(
                                type_stack.top() == 2 ? g.productions[idx_stack.top()].variable
                                                      : relist[idx_stack.top()].lhs);
                            aidx[g.productions.top().variable % HASH_SZ].append(
                                {false, g.productions.size() - 1, g.productions.top().variable});
                        }
                        type_stack.pop_back();
                        idx_stack.pop_back();
                        type_stack.top() = 2;
                        idx_stack.top() = g.productions.size() - 1;
                    }
                    break;
                case OP_MINUS:
                    if (type_stack.top() == 3 && type_stack.undertop() == 3 && detect_regular)
                    {
                        relist.append(~((~relist[idx_stack.undertop()]) | relist[idx_stack.top()]));
                        relist.top().lhs = id++;
                        aidx[relist.top().lhs % HASH_SZ].append(
                            {true, relist.size() - 1, relist.top().lhs});
                        idx_stack.pop_back();
                        type_stack.pop_back();
                        idx_stack.top() = relist.size() - 1;
                    }
                    else
                        return false;
                    break;
                }
                op_stack.pop_back();
            }
            if (*p == OP_RPRTH)
                op_stack.pop_back();
            else
                op_stack.push_back(*p);
            break;
        default:
            for (i = 0; i < aidx[*p % HASH_SZ].size(); i++)
                if (aidx[*p % HASH_SZ][i].symbol == *p)
                    break;
            if (i == aidx[*p % HASH_SZ].size())
                return false; // not found
            if (!aidx[*p % HASH_SZ][i].regular)
                if (!EBNFToGrammarRecursion(
                        ebnflist, relist, g, aidx[*p % HASH_SZ][i].idx, id, aidx, detect_regular))
                    return false;
            if (aidx[*p % HASH_SZ][i].regular) // production found
            {
                idx_stack.push_back(aidx[*p % HASH_SZ][i].idx); // index of relist
                type_stack.push_back(3);                        // regular
            }
            else
            {
                idx_stack.push_back(aidx[*p % HASH_SZ][i].idx); // index of production
                type_stack.push_back(2);                        // context-free
            }
            break;
        }
    }
    if (!detect_regular || g.productions[idx].variable == g.s->variable)
    {
        if (type_stack.top() == 3)
        {
            g.productions[idx].expression.append(list<unsigned int>());
            g.productions[idx].expression.top().append(relist[idx_stack.top()].lhs);
        }
        else if (!detect_regular)
            g.productions[idx].expression = g.productions[idx_stack.top()].expression;
    }
    else if (type_stack.top() == 3)
    {
        unsigned int &symbol = g.productions[idx].variable;
        for (int i = 0; i < aidx[symbol % HASH_SZ].size(); i++)
            if (aidx[symbol % HASH_SZ][i].symbol == symbol)
            {
                aidx[symbol % HASH_SZ][i].idx = relist.size();
                aidx[symbol % HASH_SZ][i].regular = true;
                break;
            }
        relist.append(expr(symbol, relist[idx_stack.top()].rhs));
        symbol = EPSILON;
    }
    ebnflist[idx].rhs[len] = OP_TRMNL;
    if (detect_regular)
    {
        g.productions[idx].expression.append(list<unsigned int>());
        g.productions[idx].expression.top().append((unsigned int)-2);
    }
    return true;
}

/**
 * @brief Substitute the single production recursively
 * 
 * @param g the grammar to simplify
 * @param aidx hash table
 * @param idx current index
 * @param v visiting array
 * @param id_start the additional symbol start id
 */
void SingleSubstitution(
    grammar &g, list<hash_symbol_info> *aidx, int idx, bool *v, unsigned int id_start)
{
    if (v[idx])
        return;
    v[idx] = true;
    for (int j = 0; j < g.productions[idx].expression.size(); j++)
    {
        for (int k = 0; k < g.productions[idx].expression[j].size(); k++)
            for (int l = 0; l < aidx[g.productions[idx].expression[j][k] % HASH_SZ].size(); l++)
                if (aidx[g.productions[idx].expression[j][k] % HASH_SZ][l].symbol ==
                        g.productions[idx].expression[j][k] &&
                    !aidx[g.productions[idx].expression[j][k] % HASH_SZ][l].regular &&
                    g.productions[idx].expression[j][k] >= id_start)
                {
                    int idx_k = aidx[g.productions[idx].expression[j][k] % HASH_SZ][l].idx;
                    SingleSubstitution(g, aidx, idx_k, v, id_start);
                    if (g.productions[idx_k].expression.size() == 1 && idx_k != idx)
                    {
                        g.productions[idx].expression[j].remove(k--);
                        for (int m = 0; m < g.productions[idx_k].expression.top().size(); m++)
                            g.productions[idx].expression[j].insert(
                                k++, g.productions[idx_k].expression.top()[m]);
                    }
                    break;
                }
        if (g.productions[idx].expression[j].size() == 1 &&
            g.productions[idx].expression[j].top() >= id_start &&
            g.productions[idx].expression[j].top() != g.productions[idx].variable)
        {
            unsigned int symbol = g.productions[idx].expression[j].top();
            for (int k = 0; k < aidx[symbol % HASH_SZ].size(); k++)
            {
                hash_symbol_info info = aidx[symbol % HASH_SZ][k];
                if (info.symbol == symbol && !info.regular)
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

void Traverse(grammar &g, list<expr> &relist, list<hash_symbol_info> *aidx,
              unsigned int symbol, bool *v_g, bool *v_re)
{
    int i, j;
    for (j = 0; j < aidx[symbol % HASH_SZ].size(); j++)
        if (aidx[symbol % HASH_SZ][j].symbol == symbol)
        {
            i = aidx[symbol % HASH_SZ][j].idx;
            break;
        }
    if ((aidx[symbol % HASH_SZ][j].regular ? v_re : v_g)[i])
        return;
    (aidx[symbol % HASH_SZ][j].regular ? v_re : v_g)[i] = true;
    if (aidx[symbol % HASH_SZ][j].regular)
        return;
    for (int j = 0; j < g.productions[i].expression.size(); j++)
        for (int k = 0; k < g.productions[i].expression[j].size(); k++)
            Traverse(g, relist, aidx, g.productions[i].expression[j][k], v_g, v_re);
}

/**
 * @brief Convert EBNF to grammar
 * 
 * @param ebnflist the list of ebnf expressions
 * @param relist the list of regular language, also to be appended with derived expressions
 * @param s start variable
 * @param g the grammar to convert
 */
bool EBNFToGrammar(list<expr> ebnflist, list<expr> &relist, unsigned int s, grammar &g,
                   bool detect_regular)
{
    unsigned int ep[] = {EPSILON, OP_TRMNL};
    relist.append(expr(EPSILON, ep));
    g = grammar();
    ebnflist.sort(0, ebnflist.size() - 1);
    unsigned int id_start = 0, id;
    for (int i = 0; i < ebnflist.size(); i++)
    {
        if (g.productions.size() == 0 || ebnflist[i].lhs != g.productions.top().variable)
        {
            g.productions.append({ebnflist[i].lhs, list<list<unsigned int>>()});
            g.productions.top().expression.append(list<unsigned int>());
            g.productions.top().expression.top().append((unsigned int)(-2)); // not visited
            if (s == ebnflist[i].lhs)
                g.s = &g.productions.top();
        }
        else
        {
            ebnflist[i - 1] = ebnflist[i - 1] | ebnflist[i];
            ebnflist[i - 1].lhs = ebnflist[i].lhs;
            ebnflist.remove(i--);
        }
        if (ebnflist[i].lhs + 1 > id_start)
            id_start = ebnflist[i].lhs + 1;
        for (unsigned int *p = ebnflist[i].rhs; *p != OP_TRMNL; p++)
            if (*p < OP_LPRTH && (*p) + 1 > id_start)
                id_start = (*p) + 1;
    }
    for (int i = 0; i < relist.size(); i++)
        if (relist[i].lhs + 1 > id_start)
            id_start = relist[i].lhs + 1;
    id = id_start;
    bool suc = true;
    list<hash_symbol_info> *aidx;
    aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    if (aidx == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    for (int i = 0; i < g.productions.size(); i++)
        aidx[g.productions[i].variable % HASH_SZ].append({false, i, g.productions[i].variable});
    for (int i = 0; i < relist.size(); i++)
        aidx[relist[i].lhs % HASH_SZ].append({true, i, relist[i].lhs});
    if (detect_regular)
        for (int i = 0; i < ebnflist.size() && suc; i++)
            suc &= EBNFToGrammarRecursion(ebnflist, relist, g, i, id, aidx, true);
    for (int i = 0; i < ebnflist.size() && suc; i++)
        suc &= EBNFToGrammarRecursion(ebnflist, relist, g, i, id, aidx, false);
    for (int i = 0; i < g.productions.size(); i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
                if (g.productions[i].expression[j][k] == EPSILON)
                    g.productions[i].expression[j].remove(k--);
    bool *vstd_g = (bool *)malloc(sizeof(bool) * g.productions.size());
    if (vstd_g == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    memset(vstd_g, 0, sizeof(bool) * g.productions.size());
    for (int i = 0; i < g.productions.size(); i++)
        if (!vstd_g[i])
            SingleSubstitution(g, aidx, i, vstd_g, id_start);
    bool *vstd_re = (bool *)malloc(sizeof(bool) * relist.size());
    if (vstd_re == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    memset(vstd_g, 0, sizeof(bool) * g.productions.size());
    memset(vstd_re, 0, sizeof(bool) * relist.size());
    Traverse(g, relist, aidx, g.s->variable, vstd_g, vstd_re);
    list<int> rm_idx;
    for (int i = 0; i < g.productions.size(); i++)
        if (!vstd_g[i])
            rm_idx.append(i);
    for (int i = 0; i < rm_idx.size(); i++)
        g.productions.remove(rm_idx[i] - i);
    rm_idx = list<int>();
    for (int i = 0; i < relist.size(); i++)
        if (!vstd_re[i])
            rm_idx.append(i);
    for (int i = 0; i < rm_idx.size(); i++)
        relist.remove(rm_idx[i] - i);
    relist.sort(0, relist.size() - 1);
    free(vstd_g);
    free(vstd_re);
    delete[] aidx;
    return suc;
}

/**
 * @brief Get the next unsigned integer
 * 
 * @param str the string
 * @param i start index
 * @param num integer output
 * @return index of position after this number
 */
int GetInt(list<unsigned int> &str, int i, unsigned short &num)
{
    int j = i;
    num = 0;
    while (j < str.size() &&
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
 * @param fp the file pointer
 * @param ebnflist result EBNF list
 * @param relist result regular language list
 * @param sep separations of UNICODE characters
 * @param names the corresponding name string of each index
 * 
 * @return the line number
 */
int ReadEBNFFromFile(FILE *fp, UINT code_page, list<expr> &ebnflist, list<expr> &relist,
                     list<unsigned int> &sep, list<list<unsigned int>> &names)
{
    fseek(fp, 0, SEEK_END);
    int sz = ftell(fp);
    char *buffer = (char *)malloc(sz);
    if (buffer == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    rewind(fp);
    fread(buffer, 1, sz, fp);
    int wsz = MultiByteToWideChar(code_page, 0, buffer, sz, NULL, 0);
    wchar_t *wbuffer = (wchar_t *)malloc(wsz * sizeof(wchar_t));
    if (wbuffer == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    MultiByteToWideChar(code_page, 0, buffer, sz, wbuffer, wsz);
    free(buffer);
    list<unsigned int> lstr; // list implementation of the wide string
    for (int i = 0; i < wsz; i++)
        lstr.append(wbuffer[i]); // to insert more conveniently
    free(wbuffer);
    int i = 0, j = 0, line = 1;
    while (i < wsz)
        if (lstr[i] == L'[')
        {
            i++;
            if (i < wsz && lstr[i] == L'^')
                i++;
            int j = i;
            while (j < wsz && lstr[j] != L'\n' && lstr[j] != L']')
                j++;
            if (j == wsz || lstr[j] == L'\n')
                return line;
            int k = i;
            while (k < j)
            {
                unsigned short l, r;
                l = lstr[k];
                if (k + 1 < j && lstr[k] == L'#' && lstr[k + 1] == L'x')
                    k = GetInt(lstr, k + 2, l);
                else
                    k++;
                if (lstr[k] == L'-')
                {
                    k++;
                    r = lstr[k];
                    if (k + 1 < j && lstr[k] == L'#' && lstr[k + 1] == L'x')
                        k = GetInt(lstr, k + 2, r);
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
        else if (lstr[i] == L'#')
            if (i + 1 < wsz && lstr[i + 1] == L'x')
            {
                unsigned short num;
                i = GetInt(lstr, i + 2, num);
                sep.append(num);
                sep.append(num + 1);
            }
            else
                return line;
        else if (lstr[i] == '\'' || lstr[i] == '"')
        {
            i++;
            int j = i;
            while (j < wsz && lstr[j] != L'\n' && lstr[j] != lstr[i - 1])
                j++;
            if (j < wsz && lstr[j] != L'\n')
                for (int k = i; k < j; k++)
                {
                    sep.append(lstr[k]);
                    sep.append(lstr[k] + 1);
                }
            else
                return line;
            i = j + 1;
        }
        else if (lstr[i++] == '\n')
            line++;
    sep.set_reduce();
    i = j = line = 0;
    int id = 0;
    names = list<list<unsigned int>>();
    while (i < wsz && (lstr[i] == L'\n' || lstr[i] == L' ' ||
                       lstr[i] == L'\t' || lstr[i] == L'\r'))
        i++;
    while (i < wsz)
    {
        line++;
        j = i;
        while (j < wsz && lstr[j] != L'\n')
            j++;
        // [i, j) is one EBNF expression
        int k = i;
        while (k + 2 < wsz && !(lstr[k] == ':' && lstr[k + 1] == ':' && lstr[k + 2] == '='))
            k++;
        if (k + 2 == wsz)
            return line;
        names.append(list<unsigned int>());
        for (int l = i; l < k; l++)
            if (lstr[l] != L' ' && lstr[l] != L'\t' && lstr[l] != L'\r')
                names.top().append(lstr[l]);
        i = j + 1;
        while (i < wsz && (lstr[i] == L'\n' || lstr[i] == L' ' ||
                           lstr[i] == L'\t' || lstr[i] == L'\r'))
            i++;
    }
    relist = list<expr>();
    for (int i = 0; i <= sep.size(); i++)
    {
        unsigned int RHS[] = {(unsigned int)i, OP_TRMNL};
        relist.append(expr(names.size() + i, RHS));
    }
    i = j = line = 0;
    while (i < wsz && (lstr[i] == L'\n' || lstr[i] == L' ' ||
                       lstr[i] == L'\t' || lstr[i] == L'\r'))
        i++;
    while (i < wsz)
    {
        line++;
        j = i;
        while (j < wsz && lstr[j] != L'\n')
            j++;
        // [i, j) is one EBNF expression
        int k = i;
        while (k + 2 < j && !(lstr[k] == ':' && lstr[k + 1] == ':' && lstr[k + 2] == '='))
            k++;
        if (k + 2 == j)
            return line;
        k += 3;
        list<unsigned int> rhs_list;
        list<unsigned int> name;
        bool reverse;
        list<unsigned int> terminals; // terminal symbol range
        int l, m;                     // loop variables
        while (k < j)
        {
            if (lstr[k] == L' ' || lstr[k] == L'\t' || lstr[k] == L'#' || lstr[k] == L'[' ||
                lstr[k] == L'\'' || lstr[k] == L'"' || lstr[k] == L'(' || lstr[k] == L')' ||
                lstr[k] == L'?' || lstr[k] == L'|' || lstr[k] == L'-' || lstr[k] == L'+' ||
                lstr[k] == L'*')
            {
                if ((!name.empty() || lstr[k] == L'(' || lstr[k] == L'[' ||
                     lstr[k] == L'#' || lstr[k] == L'\'' || lstr[k] == L'"') &&
                    (!rhs_list.empty() &&
                     (rhs_list.top() < OP_LPRTH || rhs_list.top() == OP_RPRTH ||
                      rhs_list.top() == OP_KLCLS || rhs_list.top() == OP_OPTNL ||
                      rhs_list.top() == OP_PTCLS))) // is not a operator
                    rhs_list.append(OP_CNCAT);
                if (!name.empty())
                {
                    int l;
                    // need hash improvement
                    for (l = 0; l < names.size(); l++)
                        if (names[l] == name)
                            break;
                    if (l == names.size())
                        return line;
                    // need hash improvement
                    rhs_list.append((unsigned int)l);
                    name = list<unsigned int>();
                }
            }
            switch (lstr[k])
            {
            case L'\r': // carriage return
                k++;
                break;
            case L' ':
            case L'\t': // space characters
                k++;
                break;
            case L'#': // unicode characters
                if (k + 1 < j && lstr[k + 1] == L'x')
                {
                    unsigned short num;
                    k = GetInt(lstr, k + 2, num);
                    if (!rhs_list.empty() && rhs_list.top() < OP_LPRTH)
                        rhs_list.append(OP_CNCAT);
                    rhs_list.append(DetermineTerminal(sep, num) + names.size());
                }
                else
                    name.append(lstr[k++]);
                break;
            case L'[': // strings
                k++;
                if ((reverse = k < j && lstr[k] == L'^'))
                    k++;
                terminals = list<unsigned int>();
                l = k;
                while (l < j && lstr[l] != L']')
                    l++;
                m = k; // m in [k, l)
                while (m < l)
                {
                    unsigned short left, right;
                    left = lstr[m];
                    if (m + 1 < l && lstr[m] == L'#' && lstr[m + 1] == L'x')
                        m = GetInt(lstr, m + 2, left);
                    else
                        m++;
                    if (lstr[m] == L'-')
                    {
                        m++;
                        right = lstr[m];
                        if (m + 1 < l && lstr[m] == L'#' && lstr[m + 1] == L'x')
                            m = GetInt(lstr, m + 2, left);
                        else
                            m++;
                    }
                    else
                        right = left;
                    for (int n = DetermineTerminal(sep, left); n <= DetermineTerminal(sep, right); n++)
                        terminals.append(n + names.size());
                }
                if (reverse)
                {
                    list<unsigned int> terminals_r;
                    int n = 0;
                    for (m = 0; m <= sep.size(); m++)
                        if (n < terminals.size() && (unsigned int)(m + names.size()) == terminals[n])
                            n++;
                        else
                            terminals_r.append((unsigned int)(m + names.size()));
                    terminals = terminals_r;
                }
                if (!rhs_list.empty() && rhs_list.top() < OP_LPRTH)
                    rhs_list.append(OP_CNCAT);
                if (terminals.empty())
                    rhs_list.append(EPSILON);
                else
                {
                    if (terminals.size() > 1)
                        rhs_list.append(OP_LPRTH);
                    rhs_list.append(terminals[0]);
                    for (m = 1; m < terminals.size(); m++)
                    {
                        rhs_list.append(OP_ALTER);
                        rhs_list.append(terminals[m]);
                    }
                    if (terminals.size() > 1)
                        rhs_list.append(OP_RPRTH);
                }
                k = l + 1;
                break;
            case L'\'':
            case L'"': // quoted strings
                k++;
                l = k;
                while (l < j && lstr[l] != lstr[k - 1])
                    l++;
                if (l < j)
                {
                    if (!rhs_list.empty() && rhs_list.top() < OP_LPRTH)
                        rhs_list.append(OP_CNCAT);
                    if (l == k)
                        rhs_list.append(EPSILON);
                    else
                    {
                        if (l > k + 1)
                            rhs_list.append(OP_LPRTH);
                        rhs_list.append(DetermineTerminal(sep, lstr[k]) + names.size());
                        for (m = k + 1; m < l; m++)
                        {
                            rhs_list.append(OP_CNCAT);
                            rhs_list.append(DetermineTerminal(sep, lstr[m]) + names.size());
                        }
                        if (l > k + 1)
                            rhs_list.append(OP_RPRTH);
                    }
                    k = l + 1;
                }
                else
                    name.append(lstr[k - 1]);
                break;
            case L'(': // left parentheses
                rhs_list.append(OP_LPRTH);
                k++;
                break;
            case L')': // right parentheses
                rhs_list.append(OP_RPRTH);
                k++;
                break;
            case L'?': // optional operator
                rhs_list.append(OP_OPTNL);
                k++;
                break;
            case L'|': // alternation operator
                rhs_list.append(OP_ALTER);
                k++;
                break;
            case L'-': // difference operator
                rhs_list.append(OP_MINUS);
                k++;
                break;
            case L'+': // positive closure operator
                rhs_list.append(OP_PTCLS);
                k++;
                break;
            case L'*': // Kleene closure operator
                rhs_list.append(OP_KLCLS);
                k++;
                break;
            default:
                name.append(lstr[k++]);
                break;
            }
        }
        if (!name.empty() &&
            (!rhs_list.empty() &&
             (rhs_list.top() < OP_LPRTH || rhs_list.top() == OP_RPRTH ||
              rhs_list.top() == OP_KLCLS || rhs_list.top() == OP_OPTNL ||
              rhs_list.top() == OP_PTCLS))) // is not a operator
            rhs_list.append(OP_CNCAT);
        if (!name.empty())
        {
            int l;
            // need hash improvement
            for (l = 0; l < names.size(); l++)
                if (names[l] == name)
                    break;
            if (l == names.size())
                return line;
            // need hash improvement
            rhs_list.append((unsigned int)l);
            name = list<unsigned int>();
        }
        unsigned int *rhs = (unsigned int *)malloc(sizeof(unsigned int) * (rhs_list.size() + 1));
        if (rhs == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
        for (int i = 0; i < rhs_list.size(); i++)
            rhs[i] = rhs_list[i];
        rhs[rhs_list.size()] = OP_TRMNL;
        ebnflist.append(expr(id++, rhs));
        free(rhs);
        i = j + 1;
        while (i < wsz && (lstr[i] == L'\n' || lstr[i] == L' ' ||
                           lstr[i] == L'\t' || lstr[i] == L'\r'))
            i++;
    }
    return 0;
}
