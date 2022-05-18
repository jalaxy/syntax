/****************************************************************
 * @file LALR.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Function definition of LALR.h
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#include "LALR.h"
#include <cstring>
#include <cstdio>

struct hash_lr1_items_info
{
    int idx;
    lr1_item item;
};

struct hash_prod_info
{
    int idx;
    unsigned int variable;
    list<unsigned int> expr;
};

/**
 * @brief Comparison between two LR(1) items
 * 
 * @param a one item
 * @param b the other item
 * @return whether two LR(1) items are the same
 */
bool operator==(const lr1_item &a, const lr1_item &b)
{
    return a.variable == b.variable &&
           a.expression == b.expression &&
           a.dot == b.dot && a.sym == b.sym;
}

/**
 * @brief Hash function
 * 
 * @param i the LR(1) item
 * @return hash value
 */
int h(lr1_item &i)
{
    unsigned int ans = i.variable;
    for (int j = 0; j < i.expression.size(); j++)
        ans = (ans * 10 + i.expression[j]) % HASH_SZ;
    return (ans + i.dot + i.sym) % HASH_SZ;
}

/**
 * @brief Hash function
 * 
 * @param v production variable
 * @param e expression
 * @return hash value
 */
int h(unsigned int v, list<unsigned int> &e)
{
    unsigned int ans = v;
    for (int j = 0; j < e.size(); j++)
        ans = (ans * 10 + e[j]) % HASH_SZ;
    return ans;
}

/**
 * @brief The calculation of hash table
 * 
 * @param g the grammar
 * @param aidx the result hash table array
 * @param prod_sz production size, the index of first terminal
 * @return the number of regular language, which is not appearing in LHS
 */
int CalcHash(grammar &g, list<hash_symbol_info> *aidx)
{
    int col = 0;
    for (int i = 0; i < g.productions.size(); i++)
        aidx[g.productions[i].variable % HASH_SZ].append({false, i, g.productions[i].variable});
    list<unsigned int> re_sym;
    for (int i = 0; i < g.productions.size(); i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
            for (int k = 0; k < g.productions[i].expression[j].size(); k++)
                if (QuerySymbol(aidx, g.productions[i].expression[j][k]) == NULL)
                    re_sym.append(g.productions[i].expression[j][k]);
    re_sym.set_reduce();
    for (int i = 0; i < re_sym.size(); i++)
        aidx[re_sym[i] % HASH_SZ].append({true, (col++) + g.productions.size(), re_sym[i]});
    aidx[ENDMARK % HASH_SZ].append({true, (col++) + g.productions.size(), ENDMARK});
    return col;
}

/**
 * @brief Calculation of FIRST(variable)
 * 
 * @param g the grammar
 * @param variable the variable
 * @return the list of FIRST
 */
list<unsigned int> lr1_parsing_table::calc_first(grammar &g, unsigned int variable, bool init)
{
    static bool *v;
    if (init)
    {
        v = (bool *)malloc(sizeof(bool) * g.productions.size());
        if (v == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
            return list<unsigned int>();
        }
        memset(v, 0, sizeof(bool) * g.productions.size());
    }
    const hash_symbol_info *p_info = QuerySymbol(aidx, variable);
    v[p_info->idx] = true;
    list<unsigned int> l;
    for (int i = 0; i < g.productions[p_info->idx].expression.size(); i++)
    {
        int j;
        for (j = 0; j < g.productions[p_info->idx].expression[i].size(); j++)
        {
            const hash_symbol_info *q_info =
                QuerySymbol(aidx, g.productions[p_info->idx].expression[i][j]);
            if (q_info->regular)
                l.append(q_info->symbol);
            else if (!v[q_info->idx])
                l += calc_first(g, q_info->symbol, false);
            if (l.top() == EPSILON)
                l.pop_back();
            else
                break;
        }
        if (j == g.productions[p_info->idx].expression[i].size())
            l.append(EPSILON);
    }
    l.set_reduce();
    if (init && v != NULL)
        free(v);
    return l;
}

/**
 * @brief Copy from another object
 * 
 * @param b 
 */
void lr1_parsing_table::copy(const lr1_parsing_table &b)
{
    s = b.s;
    row = b.row;
    col = b.col;
    col_var = b.col_var;
    reduc_size = b.reduc_size;
    table = (int *)malloc(sizeof(int) * row * col);
    output = (unsigned int *)malloc(sizeof(unsigned int) * row);
    reduc_var = (unsigned int *)malloc(sizeof(unsigned int) * reduc_size);
    reduc_expr = new (std::nothrow) list<unsigned int>[reduc_size];
    reduc_idx = (int *)malloc(sizeof(int) * reduc_size);
    aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    if (table == NULL || reduc_var == NULL || reduc_idx == NULL ||
        reduc_expr == NULL || aidx == NULL || output == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        row = col = col_var = 0;
        return;
    }
    memcpy(table, b.table, row * col * sizeof(int));
    memcpy(output, b.output, row * sizeof(unsigned int));
    for (int i = 0; i < reduc_size; i++)
    {
        reduc_var[i] = b.reduc_var[i];
        reduc_idx[i] = b.reduc_idx[i];
        reduc_expr[i] = b.reduc_expr[i];
    }
    for (int i = 0; i < (int)HASH_SZ; i++)
        aidx[i] = b.aidx[i];
}

/**
 * @brief Construct a new LR(1) parsing table object
 * 
 * @param b another object
 */
lr1_parsing_table::lr1_parsing_table(const lr1_parsing_table &b) { copy(b); }

/**
 * @brief Construct a new LR(1) parsing table object
 * 
 * @param g a context-free grammar
 */
lr1_parsing_table::lr1_parsing_table(grammar g)
{
    table = NULL;
    reduc_var = output = NULL;
    reduc_expr = NULL;
    reduc_idx = NULL;
    aidx = NULL;
    g.augment();
    aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    list<hash_lr1_items_info> *aidx_item_1 = new (std::nothrow) list<hash_lr1_items_info>[HASH_SZ];
    list<unsigned int> *first = new (std::nothrow) list<unsigned int>[g.productions.size()];
    if (aidx == NULL || aidx_item_1 == NULL || first == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        row = col = col_var = 0;
        return;
    }
    // symbols: 0 ~ variables - 1, variables ~ variables + terminals - 1
    col = (col_var = g.productions.size()) + CalcHash(g, aidx);
    for (int i = 0; i < g.productions.size(); i++)
        first[i] = calc_first(g, g.productions[i].variable); // can be optimized
    unsigned int id = 0, vp = 0;                             // viable prefix
    list<lr1_item> items;
    list<unsigned int> reduc_var_list;
    list<list<unsigned int>> reduc_expr_list;
    fa nfa;
    nfa.g.remove_vertex(0);
    nfa.g.add_vertex({id++, list<unsigned int>()});
    items.append({g.s->variable, g.s->expression.top(), 0, ENDMARK}); // S' -> .S, $
    aidx_item_1[h(items.top()) % HASH_SZ].append({0, items.top()});
    nfa.s = &nfa.g[0];
    for (int i = 0; i < nfa.g.size(); i++)
        if (items[i].sym != REDCTION && items[i].dot < items[i].expression.size())
        {
            unsigned symbol_after_dot = items[i].expression[items[i].dot];
            const hash_symbol_info *p_info = QuerySymbol(aidx, symbol_after_dot);
            lr1_item item_shift = items[i];
            item_shift.dot++;
            int j;
            list<hash_lr1_items_info> &info = aidx_item_1[h(item_shift) % HASH_SZ];
            for (j = 0; j < info.size(); j++)
                if (info[j].item == item_shift)
                    break;
            if (j == info.size()) // not found
            {
                nfa.g.add_vertex({id++, list<unsigned int>()});
                nfa.g.add_edge(nfa.g[i], nfa.g.top(), {symbol_after_dot});
                items.append(item_shift);
                info.append({items.size() - 1, item_shift});
            }
            else
                nfa.g.add_edge(i, info[j].idx, {symbol_after_dot});
            if (!p_info->regular)
                for (int j = 0; j < g.productions[p_info->idx].expression.size(); j++)
                {
                    lr1_item item_closure =
                        {g.productions[p_info->idx].variable,
                         g.productions[p_info->idx].expression[j],
                         0};
                    list<unsigned int> first_bta; // FIRST(beta a)
                    first_bta.append(EPSILON);
                    for (int l = items[i].dot + 1; l < items[i].expression.size(); l++)
                    {
                        const hash_symbol_info *p_info =
                            QuerySymbol(aidx, items[i].expression[l]);
                        if (first_bta.top() == EPSILON)
                            first_bta.pop_back();
                        else
                            break;
                        if (p_info->regular)
                            first_bta.append(p_info->symbol);
                        else
                            first_bta += first[p_info->idx];
                    }
                    first_bta.set_reduce();
                    if (first_bta.top() == EPSILON)
                    {
                        first_bta.pop_back();
                        first_bta.append(items[i].sym);
                    }
                    for (int s = 0; s < first_bta.size(); s++)
                    {
                        item_closure.sym = first_bta[s];
                        int l = 0;
                        list<hash_lr1_items_info> &info = aidx_item_1[h(item_closure) % HASH_SZ];
                        for (l = 0; l < info.size(); l++)
                            if (info[l].item == item_closure)
                                break;
                        if (l == info.size())
                        {
                            nfa.g.add_vertex({id++, list<unsigned int>()});
                            nfa.g.add_edge(nfa.g[i], nfa.g.top(), {EPSILON});
                            items.append(item_closure);
                            info.append({items.size() - 1, item_closure});
                        }
                        else
                            nfa.g.add_edge(i, info[l].idx, {EPSILON});
                    }
                }
        }
        else if (items[i].sym != REDCTION)
        {
            lr1_item item_reduc = items[i];
            item_reduc.sym = REDCTION;
            list<hash_lr1_items_info> &info = aidx_item_1[h(item_reduc) % HASH_SZ];
            int j;
            for (j = 0; j < info.size(); j++)
                if (info[j].item == item_reduc)
                    break;
            if (j == info.size())
            {
                nfa.g.add_vertex({id++, list<unsigned int>()});
                nfa.g.top().data.output.append(vp++);
                nfa.f.append(&nfa.g.top());
                items.append(item_reduc);
                reduc_var_list.append(item_reduc.variable);
                reduc_expr_list.append(item_reduc.expression);
                info.append({items.size() - 1, item_reduc});
            }
            nfa.g.add_edge(i, info[j].idx, {items[i].sym});
        }
    printf("LR NFA size: %d\n", nfa.g.size());
    NFAToDFA(nfa, nfa);
    list<list<unsigned int>> reducsym;
    list<list<int>> reducidx;
    for (int i = 0; i < (int)vp; i++)
    {
        reducsym.append(list<unsigned int>());
        reducidx.append(list<int>());
    }
    for (int i = 0; i < nfa.g.size(); i++)
        for (int j = 0; j < nfa.g[i].size(); j++)
            if (!nfa.g[i][j].to->data.output.empty())
            {
                reducsym[nfa.g[i][j].to->data.output.front()].append(nfa.g[i][j].data.value);
                reducidx[nfa.g[i][j].to->data.output.front()].append(i);
            }
    for (int i = 0; i < nfa.g.size(); i++)
        if (nfa.g[i].data.output.front() != NON_ACC)
        {
            reducsym[nfa.g[i].data.output.front()].set_reduce();
            reducidx[nfa.g[i].data.output.front()].set_reduce();
        }
    for (int i = 0; i < nfa.g.size(); i++)
    {
        unsigned int rdc = nfa.g[i].data.output.front();
        if (rdc != NON_ACC)
            for (int j = 0; j < reducidx[rdc].size(); j++)
                for (int k = 0; k < reducsym[rdc].size(); k++)
                {
                    int l;
                    for (l = 0; l < nfa.g[reducidx[rdc][j]].size(); l++)
                        if (nfa.g[reducidx[rdc][j]][l].data.value == reducsym[rdc][k])
                            break;
                    if (l == nfa.g[reducidx[rdc][j]].size())
                        nfa.g.add_edge(reducidx[rdc][j], i, {reducsym[rdc][k]});
                }
    }
    printf("LR DFA size: %d\n", nfa.g.size());
    MinimizeDFA(nfa);
    printf("Reduced LR DFA size: %d\n", nfa.g.size());
    printf("Reduction state size: %d\n", nfa.f.size());
    int cnt = 0;
    for (int i = 0; i < nfa.g.size(); i++)
        if (nfa.g[i].data.output.size() > 1)
            cnt++;
    printf("R/R Conflict: %d\n", cnt);
    delete[] first;
    delete[] aidx_item_1;
    reduc_var = (unsigned int *)malloc(sizeof(unsigned int) * reduc_var_list.size());
    reduc_expr = new (std::nothrow) list<unsigned int>[reduc_expr_list.size()];
    reduc_idx = (int *)malloc(sizeof(int) * reduc_var_list.size());
    list<hash_prod_info> *aidx_prod = new (std::nothrow) list<hash_prod_info>[HASH_SZ];
    if (reduc_var == NULL || reduc_expr == NULL | reduc_idx == NULL || aidx_prod == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        row = col = col_var = 0;
        return;
    }
    reduc_size = reduc_var_list.size();
    int prod_idx = 0;
    for (int i = 0; i < g.productions.size(); i++)
        for (int j = 0; j < g.productions[i].expression.size(); j++)
            aidx_prod[h(g.productions[i].variable, g.productions[i].expression[j]) % HASH_SZ]
                .append({prod_idx++, g.productions[i].variable, g.productions[i].expression[j]});
    for (int i = 0; i < reduc_size; i++)
    {
        reduc_var[i] = reduc_var_list[i];
        reduc_expr[i] = reduc_expr_list[i];
        list<hash_prod_info> &info = aidx_prod[h(reduc_var[i], reduc_expr[i]) % HASH_SZ];
        for (int j = 0; j < info.size(); j++)
            if (info[j].variable == reduc_var[i] && info[j].expr == reduc_expr[i])
            {
                reduc_idx[i] = info[j].idx;
                break;
            }
    }
    delete[] aidx_prod;
    row = nfa.g.size();
    table = (int *)malloc(sizeof(int) * row * col);
    output = (unsigned int *)malloc(sizeof(unsigned int) * row);
    if (table == NULL || output == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        row = col = col_var = 0;
        return;
    }
    for (int i = 0; i < row; i++)
        output[i] = nfa.g[i].data.output.empty() ? NON_ACC : nfa.g[i].data.output.front();
    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
            table[i * col + j] = UNDEFINED;
    for (int i = 0; i < row; i++)
        nfa.g[i].aux = (vertex_t *)(long long)i;
    for (int i = 0; i < row; i++)
    {
        if (nfa.s == &nfa.g[i])
            s = i;
        for (int j = 0; j < nfa.g[i].size(); j++)
        {
            const hash_symbol_info *p_info = QuerySymbol(aidx, nfa.g[i][j].data.value);
            if (table[i * col + p_info->idx] == UNDEFINED)
                table[i * col + p_info->idx] = (int)(long long)nfa.g[i][j].to->aux;
        }
    }
}

/**
 * @brief Construct a new object from binary buffer
 * 
 * @param p buffer pointer
 */
lr1_parsing_table::lr1_parsing_table(const void *p)
{
    table = NULL;
    reduc_var = output = NULL;
    reduc_expr = NULL;
    reduc_idx = NULL;
    aidx = NULL;
    if (p == NULL)
    {
        row = col = col_var = 0;
        return;
    }
    unsigned char *pc = (unsigned char *)p;
    row = *(int *)pc;
    pc += sizeof(int);
    col = *(int *)pc;
    pc += sizeof(int);
    col_var = *(int *)pc;
    pc += sizeof(int);
    reduc_size = *(int *)pc;
    pc += sizeof(int);
    s = *(int *)pc;
    pc += sizeof(int);
    table = (int *)malloc(sizeof(int) * row * col);
    output = (unsigned int *)malloc(sizeof(unsigned int) * row);
    reduc_var = (unsigned int *)malloc(sizeof(unsigned int) * reduc_size);
    reduc_expr = new (std::nothrow) list<unsigned int>[reduc_size];
    reduc_idx = (int *)malloc(sizeof(int) * reduc_size);
    aidx = new (std::nothrow) list<hash_symbol_info>[HASH_SZ];
    if (table == NULL || reduc_var == NULL || reduc_idx == NULL ||
        reduc_expr == NULL || aidx == NULL || output == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        row = col = col_var = 0;
        return;
    }
    for (int i = 0; i < row * col; i++)
    {
        table[i] = *(int *)pc;
        pc += sizeof(int);
    }
    for (int i = 0; i < row; i++)
    {
        output[i] = *(unsigned int *)pc;
        pc += sizeof(unsigned int);
    }
    for (int i = 0; i < reduc_size; i++)
    {
        reduc_var[i] = *(unsigned int *)pc;
        pc += sizeof(unsigned int);
    }
    for (int i = 0; i < reduc_size; i++)
    {
        int sz = *((int *)pc);
        pc += sizeof(int);
        for (int j = 0; j < sz; j++)
        {
            reduc_expr[i].append(*(unsigned int *)pc);
            pc += sizeof(unsigned int);
        }
    }
    for (int i = 0; i < reduc_size; i++)
    {
        reduc_idx[i] = *(int *)pc;
        pc += sizeof(int);
    }
    int cnt = *((int *)pc);
    pc += sizeof(int);
    for (int i = 0; i < cnt; i++)
    {
        int idx = *(int *)pc;
        pc += sizeof(int);
        int sz = *((int *)pc);
        pc += sizeof(int);
        for (int j = 0; j < sz; j++)
        {
            aidx[idx].append(*((hash_symbol_info *)pc));
            pc += sizeof(hash_symbol_info);
        }
    }
}

/**
 * @brief Destroy the lr1 parsing table object
 * 
 */
lr1_parsing_table::~lr1_parsing_table()
{
    if (reduc_var != NULL)
        free(reduc_var);
    if (reduc_expr != NULL)
        delete[] reduc_expr;
    if (reduc_idx != NULL)
        free(reduc_idx);
    if (aidx != NULL)
        delete[] aidx;
    if (table != NULL)
        free(table);
    if (output != NULL)
        free(output);
}

/**
 * @brief Assignment function
 * 
 * @param b 
 * @return reference to this object
 */
const lr1_parsing_table &lr1_parsing_table::operator=(const lr1_parsing_table &b)
{
    if (reduc_var != NULL)
        free(reduc_var);
    if (reduc_expr != NULL)
        delete[] reduc_expr;
    if (reduc_idx != NULL)
        free(reduc_idx);
    if (aidx != NULL)
        delete[] aidx;
    if (table != NULL)
        free(table);
    if (output != NULL)
        free(output);
    copy(b);
    return *this;
}

/**
 * @brief Store the table into binary-byte buffer
 * 
 * @param p the binary-byte buffer
 * @return the size of buffer 
 */
int lr1_parsing_table::store(void *p)
{
    if (p == NULL)
    {
        int sz = 0;
        for (int i = 0; i < reduc_size; i++)
            sz += sizeof(unsigned int) * reduc_expr[i].size();
        for (int i = 0; i < HASH_SZ; i++)
            if (!aidx[i].empty())
                sz += sizeof(int) * 2 + sizeof(hash_symbol_info) * aidx[i].size();
        return sizeof(int) * (6 + row * col + reduc_size * 2) +
               sizeof(unsigned int) * (row + reduc_size) + sz;
    }
    unsigned char *pc = (unsigned char *)p;
    *(int *)pc = row;
    pc += sizeof(int);
    *(int *)pc = col;
    pc += sizeof(int);
    *(int *)pc = col_var;
    pc += sizeof(int);
    *(int *)pc = reduc_size;
    pc += sizeof(int);
    *(int *)pc = s;
    pc += sizeof(int);
    for (int i = 0; i < row * col; i++)
    {
        *(int *)pc = table[i];
        pc += sizeof(int);
    }
    for (int i = 0; i < row; i++)
    {
        *(unsigned int *)pc = output[i];
        pc += sizeof(unsigned int);
    }
    for (int i = 0; i < reduc_size; i++)
    {
        *(unsigned int *)pc = reduc_var[i];
        pc += sizeof(unsigned int);
    }
    for (int i = 0; i < reduc_size; i++)
    {
        *(int *)pc = reduc_expr[i].size();
        pc += sizeof(int);
        for (int j = 0; j < reduc_expr[i].size(); j++)
        {
            *(unsigned int *)pc = reduc_expr[i][j];
            pc += sizeof(unsigned int);
        }
    }
    for (int i = 0; i < reduc_size; i++)
    {
        *(int *)pc = reduc_idx[i];
        pc += sizeof(int);
    }
    int cnt = 0;
    for (int i = 0; i < HASH_SZ; i++)
        if (!aidx[i].empty())
            cnt++;
    *(int *)pc = cnt;
    pc += sizeof(int);
    for (int i = 0; i < HASH_SZ; i++)
        if (!aidx[i].empty())
        {
            *(int *)pc = i;
            pc += sizeof(int);
            *(int *)pc = aidx[i].size();
            pc += sizeof(int);
            for (int j = 0; j < aidx[i].size(); j++)
            {
                *(hash_symbol_info *)pc = aidx[i][j];
                pc += sizeof(hash_symbol_info);
            }
        }
    return pc - (unsigned char *)p;
}

/**
 * @brief Parsing function
 * 
 * @param tk token stream
 * @param f_list function list
 * @return whether successful
 */
bool lr1_parsing_table::postfixtrans(list<token_info> tk,
                                     const void *(*f_list[])(const void **),
                                     const void *(*ft_list[])(const token_info &))
{
    tk.append(token_info(ENDMARK));
    list<int> st_state;
    st_state.push_back(s);
    unsigned int start_symbol = AUGMNTED;
    list<unsigned int> st_buffer;
    list<unsigned int> st_input;
    list<const void *> st_buffer_attr;
    list<const void *> st_input_attr;
    int id_tr = 0;
    for (int i = 0; i < tk.size(); i++)
    {
        st_input.push_front(tk[i].token);
        const hash_symbol_info *p_info = QuerySymbol(aidx, tk[i].token);
        st_input_attr.push_front(
            tk[i].token == ENDMARK ? NULL : ft_list[p_info->idx - col_var](tk[i]));
    }
    while (st_input.top() != start_symbol)
    {
        int state = st_state.top();
        const hash_symbol_info *p_info = QuerySymbol(aidx, st_input.top());
        int c = p_info == NULL ? 0 : p_info->idx;
        if (p_info == NULL || table[state * col + c] == UNDEFINED)
            if (output[state] == NON_ACC)
                return false;
            else
            {
                const void **args;
                if (reduc_expr[output[state]].empty())
                    args = NULL;
                else
                {
                    args = (const void **)malloc(
                        sizeof(const void *) * reduc_expr[output[state]].size());
                    if (args == NULL)
                    {
#ifdef HANDLE_MEMORY_EXCEPTION
                        HANDLE_MEMORY_EXCEPTION;
#endif
                        return false;
                    }
                }
                st_input.push_back(st_buffer.top()); // reduce
                st_input_attr.push_back(st_buffer_attr.top());
                st_buffer.pop_back();
                st_buffer_attr.pop_back();
                st_state.pop_back();
                for (int i = reduc_expr[output[state]].size() - 1; i >= 0; i--)
                {
                    args[i] = st_buffer_attr.top();
                    st_buffer.pop_back();
                    st_buffer_attr.pop_back();
                    st_state.pop_back();
                }
                st_input.push_back(reduc_var[output[state]]);
                st_input_attr.push_back(f_list[reduc_idx[output[state]]](args));
                if (args != NULL)
                    free(args);
            }
        else
        {
            if (st_input.empty())
                return false;
            st_buffer.push_back(st_input.top()); // shift
            st_buffer_attr.push_back(st_input_attr.top());
            st_state.push_back(table[state * col + c]);
            st_input.pop_back();
            st_input_attr.pop_back();
        }
    }
    return true;
}
