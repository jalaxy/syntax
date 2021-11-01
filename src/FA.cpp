/****************************************************************
 * @file FA.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Functions definition of FA.h
 * @version 0.1
 * @date 2021-10-15
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#include "FA.h"

unsigned int id = 0;

/**
 * @brief Construct a new expr object from another object
 * 
 * @param b the other object
 */
expr::expr(const expr &b)
{
    int len = 0;
    while (b.rhs[len] != OP_TRMNL)
        len++;
    rhs = (unsigned int *)malloc(sizeof(unsigned int) * (len + 1));
    if (rhs == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    memcpy(rhs, b.rhs, sizeof(unsigned int) * (len + 1));
    lhs = b.lhs;
}

/**
 * @brief Construct a new expr object with left-hand side value
 * 
 * @param LHS left-hand side value
 */
expr::expr(unsigned int LHS)
{
    rhs = (unsigned int *)malloc(sizeof(unsigned int));
    if (rhs == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    rhs[0] = OP_TRMNL;
}

/**
 * @brief Construct a new expr object by elements
 * 
 * @param LHS left-hand-side value
 * @param EXPR right-side expression
 */
expr::expr(unsigned int LHS, const unsigned int *RHS)
{
    int len = 0;
    while (RHS[len] != OP_TRMNL)
        len++;
    rhs = (unsigned int *)malloc(sizeof(unsigned int) * (len + 1));
    if (rhs == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    memcpy(rhs, RHS, sizeof(unsigned int) * (len + 1));
    lhs = LHS;
}

/**
 * @brief Destroy the re::re object
 * 
 */
expr::~expr()
{
    if (rhs != NULL)
        free(rhs);
}

/**
 * @brief Assignment function
 * 
 * @param b another object
 * @return reference to this object
 */
expr &expr::operator=(const expr &b)
{
    if (rhs != NULL)
        free(rhs);
    int len = 0;
    while (b.rhs[len] != OP_TRMNL)
        len++;
    rhs = (unsigned int *)malloc(sizeof(unsigned int) * (len + 1));
    if (rhs == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    memcpy(rhs, b.rhs, sizeof(unsigned int) * (len + 1));
    lhs = b.lhs;
    return *this;
}

/**
 * @brief Comparison between two expression object
 * 
 * @param b another object
 * @return whether lhs is less than b.lhs
 */
bool expr::operator<(const expr &b) const { return lhs < b.lhs; }

/**
 * @brief Alternation operator
 * 
 * @param b another RE
 * @return A | B 
 */
expr expr::operator|(const expr &b) const
{
    int len = 0, lenb = 0;
    for (unsigned int *p = rhs; *p != OP_TRMNL; p++)
        len++;
    for (unsigned int *p = b.rhs; *p != OP_TRMNL; p++)
        lenb++;
    unsigned int *RHS = (unsigned int *)malloc(sizeof(unsigned int) * (len + lenb + 6));
    if (RHS == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    RHS[0] = OP_LPRTH;
    memcpy(RHS + 1, rhs, len * sizeof(unsigned int));
    RHS[len + 1] = OP_RPRTH;
    RHS[len + 2] = OP_ALTER;
    RHS[len + 3] = OP_LPRTH;
    memcpy(RHS + len + 4, b.rhs, lenb * sizeof(unsigned int));
    RHS[len + lenb + 4] = OP_RPRTH;
    RHS[len + lenb + 5] = OP_TRMNL;
    return expr(NON_TERMINAL, RHS);
}

/**
 * @brief Concatenation operator
 * 
 * @param b another RE
 * @return A B
 */
expr expr::operator<<(const expr &b) const
{
    int len = 0, lenb = 0;
    for (unsigned int *p = rhs; *p != OP_TRMNL; p++)
        len++;
    for (unsigned int *p = b.rhs; *p != OP_TRMNL; p++)
        lenb++;
    unsigned int *RHS = (unsigned int *)malloc(sizeof(unsigned int) * (len + lenb + 6));
    if (RHS == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    RHS[0] = OP_LPRTH;
    memcpy(RHS + 1, rhs, len * sizeof(unsigned int));
    RHS[len + 1] = OP_RPRTH;
    RHS[len + 2] = OP_CNCAT;
    RHS[len + 3] = OP_LPRTH;
    memcpy(RHS + len + 4, b.rhs, lenb * sizeof(unsigned int));
    RHS[len + lenb + 4] = OP_RPRTH;
    RHS[len + lenb + 5] = OP_TRMNL;
    return expr(NON_TERMINAL, RHS);
}

/**
 * @brief Kleene Closure operation
 * 
 * @return A*
 */
expr expr::operator*() const
{
    int len = 0;
    for (unsigned int *p = rhs; *p != OP_TRMNL; p++)
        len++;
    unsigned int *RHS = (unsigned int *)malloc(sizeof(unsigned int) * (len + 4));
    if (RHS == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    RHS[0] = OP_LPRTH;
    memcpy(RHS + 1, rhs, len * sizeof(unsigned int));
    RHS[len + 1] = OP_RPRTH;
    RHS[len + 2] = OP_KLCLS;
    RHS[len + 3] = OP_TRMNL;
    return expr(NON_TERMINAL, RHS);
}

/**
 * @brief Complementation
 * 
 * @return ~A
 */
expr expr::operator~() const
{
    int len = 0;
    for (unsigned int *p = rhs; *p != OP_TRMNL; p++)
        len++;
    unsigned int *RHS = (unsigned int *)malloc(sizeof(unsigned int) * (len + 4));
    if (RHS == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    RHS[0] = OP_CMPLM;
    RHS[1] = OP_LPRTH;
    memcpy(RHS + 2, rhs, len * sizeof(unsigned int));
    RHS[len + 2] = OP_RPRTH;
    RHS[len + 3] = OP_TRMNL;
    return expr(NON_TERMINAL, RHS);
}

/**
 * @brief Copy an fa object from another object
 * 
 * @param b 
 */
void fa::copy(const fa &b)
{
    g = b.g;
    b.g.mapping(g, 0);
    s = b.s->aux;
    for (int i = 0; i < b.f.size(); i++)
        f.append(((fa &)b).f[i]->aux);
    sigma_range = b.sigma_range;
}

/**
 * @brief Construct a new empty fa object
 * 
 */
fa::fa()
{
    g.add_vertex({id++, NON_TERMINAL});
    s = &g[0];
    sigma_range = 0;
}

/**
 * @brief Construct a new fa object
 * 
 * @param b another object
 */
fa::fa(const fa &b) { copy(b); }

/**
 * @brief Assignment function
 * 
 * @param b 
 * @return const fa& 
 */
const fa &fa::operator=(const fa &b)
{
    copy(b);
    return *this;
}

/**
 * @brief Copy from another object
 * 
 * @param b the other object
 */
void dfa_table::copy(const dfa_table &b)
{
    row = b.row;
    col = b.col;
    s = b.s;
    if (row * col == 0)
    {
        table = NULL;
        f = NULL;
    }
    else
    {
        table = (unsigned int *)malloc(sizeof(unsigned int) * row * col);
        sep = (unsigned int *)malloc(sizeof(unsigned int) * col);
        f = (bool *)malloc(sizeof(bool) * row);
        if (table == NULL || sep == NULL || f == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
        memcpy(table, b.table, sizeof(unsigned int) * row * col);
        memcpy(sep, b.sep, sizeof(unsigned int) * col);
        memcpy(f, b.f, sizeof(bool) * row);
    }
}

/**
 * @brief Construct a new DFA table
 * 
 * @param b the other object
 */
dfa_table::dfa_table(const dfa_table &b) { copy(b); }

/**
 * @brief Construct a new DFA table from a DFA
 * 
 * @param dfa the DFA
 * @param lsep the list of separation values
 */
dfa_table::dfa_table(fa dfa, list<unsigned int> lsep)
{
    row = dfa.g.size();
    col = dfa.sigma_range;
    if (lsep.size() != col - 1)
    {
        table = sep = NULL;
        f = NULL;
        row = 1;
        col = 0;
        return;
    }
    table = (unsigned int *)malloc(sizeof(unsigned int) * row * col);
    sep = (unsigned int *)malloc(sizeof(unsigned int) * col);
    f = (bool *)malloc(sizeof(bool) * row);
    if (table == NULL || sep == NULL || f == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
            table[i * col + j] = UNDEFINED;
        dfa.g[i].aux = (vertex_t *)(long long)i;
    }
    s = (unsigned int)(long long)dfa.s->aux;
    for (int i = 0; i < row; i++)
        for (int j = 0; j < dfa.g[i].size(); j++)
        {
            unsigned int symbol = dfa.g[i][j].data.value;
            if (symbol < dfa.sigma_range && table[i * col + symbol] == UNDEFINED)
                table[i * col + symbol] = (unsigned int)(long long)dfa.g[i][j].to->aux;
            else
            {
                free(table);
                free(sep);
                free(f);
                table = sep = NULL;
                f = NULL;
                row = 1;
                col = 0;
                return;
            }
        }
    for (int i = 0; i < dfa.g.size(); i++)
        dfa.g[i].aux = (vertex_t *)0;
    for (int i = 0; i < dfa.f.size(); i++)
        dfa.f[i]->aux = (vertex_t *)1;
    for (int i = 0; i < row; i++)
        f[i] = dfa.g[i].aux == (vertex_t *)1;
    if (col > 0)
    {
        for (int i = 0; i < col - 1; i++)
            sep[i] = lsep[i];
        sep[col - 1] = 0x110000;
    }
}

/**
 * @brief Destroy the DFA table object
 * 
 */
dfa_table::~dfa_table()
{
    if (table != NULL)
        free(table);
    if (f != NULL)
        free(f);
    if (sep != NULL)
        free(sep);
}

/**
 * @brief Assignment function
 * 
 * @param b 
 * @return dfa_table& 
 */
dfa_table &dfa_table::operator=(const dfa_table &b)
{
    if (table != NULL)
        free(table);
    if (f != NULL)
        free(f);
    copy(b);
    return *this;
}

/**
 * @brief Operator []
 * 
 * @param idx the index
 * @return the first address of idx-th row
 */
unsigned int *dfa_table::operator[](int idx) { return &table[idx * col]; }

/**
 * @brief Get the number of rows
 * 
 * @return the number of rows
 */
int dfa_table::get_row() { return row; }

/**
 * @brief Get the number of columns
 * 
 * @return the number of columns
 */
int dfa_table::get_col() { return col; }

/**
 * @brief Get the initial state
 * 
 * @return the initial state
 */
int dfa_table::get_start() { return s; }

/**
 * @brief Get the final state
 * 
 * @param idx the index to query
 * @return whether i-th state is acceptable
 */
bool dfa_table::is_acceptable(int idx) { return f[idx]; }

/**
 * @brief Get next state according to separation
 * 
 * @param state current state
 * @param ch unicode character, from 0 to 0x10FFFF
 * @return next state
 */
int dfa_table::next(int state, unsigned int ch)
{
    if (state > row || ch > 0x110000)
        return UNDEFINED;
    unsigned int symbol = 0;
    if (ch >= sep[0])
    {
        int l = 0, r = col;
        while (l + 1 < r)
        {
            int mid = (l + r) / 2;
            if (sep[mid] <= ch)
                l = mid;
            else
                r = mid;
        }
        symbol = l + 1;
    }
    return table[state * col + symbol];
}

/**
 * @brief Comparison between two vertices
 * 
 * @param a one vertex
 * @param b another vertex
 * @return whether value in a is less than in b
 */
bool operator<(const vertex_info &a, const vertex_info &b) { return a.value < b.value; }

/**
 * @brief Comparison between two edges
 * 
 * @param a one edge
 * @param b another edge
 * @return whether two values in edges are equal
 */
bool operator==(const edge_info &a, const edge_info &b) { return a.value == b.value; }

/**
 * @brief Comparison between two edges
 * 
 * @param a one edge
 * @param b another edge
 * @return whether value in a is less than in b
 */
bool operator<(const edge_info &a, const edge_info &b) { return a.value < b.value; }

/**
 * @brief Alternation operation, noted that vertices in b will be
 *        moved into a and thus empty
 * 
 * @param b second automata
 * @return reference to this object after operation
 */
fa &fa::operator|=(fa &b)
{
    g.merge(b.g);
    g.add_vertex({id++, NON_TERMINAL});
    g.add_edge(g.top(), *s, {EPSILON});
    g.add_edge(g.top(), *b.s, {EPSILON});
    s = &g.top();
    f.merge(&b.f);
    sigma_range = sigma_range > b.sigma_range ? sigma_range : b.sigma_range;
    return *this;
}

/**
 * @brief Concatenation operation, noted that vertices in b will be
 *        moved into a and thus empty
 * 
 * @param b second automata
 * @return reference to this object after operation
 */
fa &fa::operator<<=(fa &b)
{
    g.merge(b.g);
    for (int i = 0; i < f.size(); i++)
        g.add_edge(*f[i], *b.s, {EPSILON});
    f = list<vertex_t *>();
    f.merge(&b.f);
    sigma_range = sigma_range > b.sigma_range ? sigma_range : b.sigma_range;
    return *this;
}

/**
 * @brief Complemention operation (only DFA)
 * 
 * @return reference to this object after operation
 */
fa &fa::operator~()
{
    g.add_vertex({id++, NON_TERMINAL});
    for (unsigned int i = 0; i < sigma_range; i++)
        g.add_edge(g.top(), g.top(), {(unsigned int)i});
    for (int i = 0; i < g.size() - 1; i++)
    {
        vertex_t &v = g[i];
        v.sort();
        int j = v.size() - 1;
        for (unsigned int k = sigma_range - 1; (int)k >= 0; k--)
            if (j >= 0 && v[j].data.value == k)
                j--;
            else
                g.add_edge(i, g.size() - 1, {(unsigned int)k});
    }
    for (int i = 0; i < g.size(); i++)
        g[i].aux = (vertex_t *)1;
    for (int i = 0; i < f.size(); i++)
        f[i]->aux = (vertex_t *)0;
    // b.g[i].aux stores whether b.g[i] is terminal states here
    f = list<vertex_t *>();
    for (int i = 0; i < g.size(); i++)
        if (g[i].aux)
            f.append(&g[i]);
    return *this;
}

/**
 * @brief Kleene Closure operation
 * 
 * @return reference to this object after operation
 */
fa &fa::operator*()
{
    g.add_vertex({id++, NON_TERMINAL});
    g.add_edge(g.top(), *s, {EPSILON});
    for (int i = 0; i < f.size(); i++)
        g.add_edge(*f[i], g.top(), {EPSILON});
    f = list<vertex_t *>();
    f.append(s = &g.top());
    return *this;
}

/**
 * @brief Make vertex ID from 0 to size() - 1
 * 
 * @param nfa the NFA to normalize
 */
void NormalizeID(fa &nfa)
{
    for (int i = 0; i < nfa.g.size(); i++)
        nfa.g[i].data.value = i;
}

/**
 * @brief Atomic automata which can accept single letters
 * 
 * @param sigma 
 * @return fa_t 
 */
fa AtomicFA(unsigned int sigma, unsigned int sigma_range)
{
    fa a;
    a.g.add_vertex({id++, NON_TERMINAL});
    a.g.add_edge(0, 1, {sigma});
    a.s = &a.g[0];
    a.f.append(&a.g[1]);
    a.sigma_range = sigma + 1 > sigma_range ? sigma + 1 : sigma_range;
    return a;
}

/**
 * @brief Convert a regular expression to NFA
 * 
 * @param relist the regular expression group
 * @param nfa the result nfa
 * @return whether succeeded
 */
bool REToNFA(list<expr> relist, fa &nfa, unsigned int symbol_range)
{
    nfa = fa();
    for (int i = 0; i < relist.size(); i++)
    {
        unsigned int sigma_range = symbol_range;
        for (unsigned int *p = relist[i].rhs; *p != OP_TRMNL; p++)
            if (*p < OP_LPRTH && (*p) + 1 > sigma_range)
                sigma_range = (*p) + 1; // decide symbol range
        int len = 0;
        while (relist[i].rhs[len] != OP_TRMNL)
            len++;
        relist[i].rhs[len] = OP_RPRTH;
        list<fa> fa_stack;
        list<unsigned int> op_stack;
        op_stack.push_back(OP_LPRTH);
        int type = 0; // the next character type (operand: 0; operator: 1)
        for (unsigned int *p = relist[i].rhs; !op_stack.empty(); p++)
        {
            if ((*p == OP_RPRTH || *p == OP_KLCLS || *p == OP_CNCAT || *p == OP_ALTER ? 1 : 0) != type)
                return false;
            type = *p == OP_LPRTH || *p == OP_CNCAT || *p == OP_ALTER || *p == OP_CMPLM ? 0 : 1;
            switch (*p)
            {
            case OP_LPRTH:
                op_stack.push_back(OP_LPRTH);
                break;
            case OP_RPRTH:
            case OP_KLCLS:
            case OP_CMPLM:
            case OP_CNCAT:
            case OP_ALTER:
                while (*p < op_stack.top() ||
                       (*p == op_stack.top() && *p != OP_CMPLM))
                {
                    switch (op_stack.top())
                    {
                    case OP_KLCLS:
                        *fa_stack.top();
                        break;
                    case OP_CMPLM:
                        NFAToDFA(fa_stack.top(), fa_stack.top());
                        MinimizeDFA(fa_stack.top());
                        ~fa_stack.top();
                        break;
                    case OP_CNCAT:
                        fa_stack.undertop() <<= fa_stack.top();
                        fa_stack.pop_back();
                        break;
                    case OP_ALTER:
                        fa_stack.undertop() |= fa_stack.top();
                        fa_stack.pop_back();
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
                fa_stack.push_back(AtomicFA(*p, sigma_range));
                break;
            }
        }
        for (int j = 0; j < fa_stack.top().f.size(); j++)
            fa_stack.top().f[j]->data.token = relist[i].lhs;
        if (nfa.f.empty())
            nfa = fa_stack.top();
        else
            nfa |= fa_stack.top();
        relist[i].rhs[len] = OP_TRMNL;
    }
    NormalizeID(nfa);
    return true;
}

/**
 * @brief Recursion part of rings elimination
 *
 * @param p the current vertex
 * @param rings the list to record rings vertices
 * @param st a stack to record path
 * @return whether exists a ring from a particular vertex
 */
bool EliminateRingsRecursion(vertex_t *p, list<vertex_t *> &st)
{
    if (!st.empty() && p == st.bottom())
        return true;
    if (p->aux == (vertex_t *)1) // aux = 1 represents this vertex is visited
        return false;
    st.push_back(p);
    p->aux = (vertex_t *)1;
    for (int i = 0; i < p->size(); i++)
        if ((*p)[i].data.value == EPSILON && (*p)[i].to != p)
            if (EliminateRingsRecursion((*p)[i].to, st))
                return true;
    st.pop_back();
    return false;
}

/**
 * @brief Eliminate the rings in nfa, to make it ordered
 * 
 * @param nfa the NFA to process
 */
void EliminateRings(fa &nfa)
{
    bool found;
    unsigned int id = nfa.g.size();
    do
    {
        found = false;
        list<vertex_t *> st;
        for (int i = 0; i < nfa.g.size() && !found; i++)
        {
            for (int i = 0; i < nfa.g.size(); i++)
                nfa.g[i].aux = (vertex_t *)0;
            found |= EliminateRingsRecursion(&nfa.g[i], st);
        }
        if (found)
        {
            nfa.g.add_vertex({id++, NON_TERMINAL});
            for (int i = 0; i < st.size(); i++)
                nfa.g.top().merge(*st[i]);
            for (int i = 0; i < nfa.g.size(); i++)
                nfa.g[i].aux = (vertex_t *)0;
            for (int i = 0; i < st.size(); i++)
                st[i]->aux = (vertex_t *)1; // when it is in this ring
            int sz = nfa.g.top().size();
            for (int i = 0; i < sz; i++)
                if (nfa.g.top()[i].to->aux == (vertex_t *)1 &&
                    nfa.g.top()[i].data.value != EPSILON)
                    nfa.g.add_edge(nfa.g.size() - 1, nfa.g.size() - 1, {nfa.g.top()[i].data});
            for (int i = 0; i < nfa.g.size() - 1; i++)
                for (int j = 0; j < nfa.g[i].size(); j++)
                    if (nfa.g[i][j].to->aux == (vertex_t *)1)
                        nfa.g.add_edge(i, nfa.g.size() - 1, {nfa.g[i][j].data});
            nfa.g.top().set_reduce();
            for (int i = 0; i < nfa.g.size(); i++)
                nfa.g[i].aux = (vertex_t *)-1;
            for (int i = 0; i < nfa.f.size(); i++)
                nfa.f[i]->aux = (vertex_t *)(long long)i; // i is the index of nfa.f
            nfa.g.top().data.token = NON_TERMINAL;
            bool tmnl = false;
            for (int i = 0; i < st.size(); i++)
            {
                if (st[i]->aux != (vertex_t *)-1) // it is a terminal
                {
                    if (st[i]->data.token < nfa.g.top().data.token)
                        nfa.g.top().data.token = st[i]->data.token;
                    nfa.f.remove((long long)st[i]->aux);
                    tmnl = true;
                }
                if (st[i] == nfa.s)
                    nfa.s = &nfa.g.top();
            }
            if (tmnl)
                nfa.f.append(&nfa.g.top());
            list<int> rm_idx;
            for (int i = 0; i < nfa.g.size(); i++)
                nfa.g[i].aux = (vertex_t *)(long long)i;
            for (int i = 0; i < st.size(); i++)
                rm_idx.append((long long)st[i]->aux);
            rm_idx.sort(0, rm_idx.size() - 1);
            for (int i = 0; i < rm_idx.size(); i++)
                nfa.g.remove_vertex(rm_idx[i] - i);
        }
    } while (found);
    NormalizeID(nfa);
}

/**
 * @brief Recursion part of epsilon-closure calculation
 * 
 * @param p current vertex to calculate
 */
void EpsilonClosureRecursion(vertex_t *p)
{
    list<vertex_t *> &epsilon_closure = *(list<vertex_t *> *)(p->aux->aux);
    if (epsilon_closure.top() != NULL)
        return;
    epsilon_closure.pop_back(); // pop the not-visited sign
    epsilon_closure.push_back(p);
    for (int i = 0; i < p->size(); i++)
        if ((*p)[i].data.value == EPSILON && (*p)[i].to != p)
        {
            EpsilonClosureRecursion((*p)[i].to);
            epsilon_closure += *(list<vertex_t *> *)((*p)[i].to->aux->aux);
        }
    epsilon_closure.set_reduce();
}

/**
 * @brief Calculate Epsilon-closure of an NFA.
 *        The result will be stored in nfa.g[i].aux. So make sure that
 *        dynamic list<vertex_t *> object is pre-allocated in aux 
 * 
 * @param nfa the NFA
 */
void EpsilonClosure(fa &nfa)
{
    // initialize with NULL, representing not visited
    for (int i = 0; i < nfa.g.size(); i++)
        ((list<vertex_t *> *)nfa.g[i].aux->aux)->push_back(NULL);
    for (int i = 0; i < nfa.g.size(); i++)
        EpsilonClosureRecursion(&nfa.g[i]);
}

/**
 * @brief Traverse from the start state and mark reachable vertices in aux
 * 
 * @param p the current vertex
 */
void Traverse(vertex_t *p)
{
    if (p->aux == (vertex_t *)1) // visited
        return;
    p->aux = (vertex_t *)1;
    for (int i = 0; i < p->size(); i++)
        Traverse((*p)[i].to);
}

/**
 * @brief Hash function
 * 
 * @param l the vertex list
 * @param m modulus
 */
unsigned int h(list<vertex_t *> &l, unsigned int m)
{
    unsigned int res = 0;
    for (int i = 0; i < l.size(); i++)
        res = (res + l[i]->data.value % m) % m;
    return res;
}

/**
 * @brief Convert an NFA to equivalent DFA
 * 
 * @param nfa the NFA to convert
 * @param dfa the result DFA
 */
void NFAToDFA(fa nfa, fa &dfa)
{
    EliminateRings(nfa);
    dfa.g = graph<vertex_info, edge_info>();
    dfa.f = list<vertex_t *>();
    for (int i = 0; i < nfa.g.size(); i++)
    {
        dfa.g.add_vertex(nfa.g[i].data);
        dfa.g[i].aux = (vertex_t *)new (std::nothrow) list<vertex_t *>;
        if (dfa.g[i].aux == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
    }
    nfa.g.mapping(dfa.g, 0);
    EpsilonClosure(nfa); // dfa.aux will store the subsets
    unsigned int size = nfa.g.size();
    const int hash_size = 1024;
    list<hash_subset_info> *aidx;
    aidx = new (std::nothrow) list<hash_subset_info>[hash_size];
    if (aidx == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    for (int i = 0; i < dfa.g.size(); i++)
    {
        list<vertex_t *> &subset = *(list<vertex_t *> *)dfa.g[i].aux;
        unsigned int hash = h(subset, hash_size);
        aidx[hash].append({i, subset});
    }
    for (int i = 0; i < dfa.g.size(); i++) // i-th dfa-state
    {
        // elements in subset is the pointer to vertices of nfa
        list<vertex_t *> &subset = *(list<vertex_t *> *)dfa.g[i].aux;
        list<unsigned int> symbols;
        list<list<vertex_t *>> dest;            // current_subset --symbol[i]--> dest[i]
        for (int j = 0; j < subset.size(); j++) // j-th nfa-state in dfa-state subset
        {
            for (int k = 0; k < subset[j]->size(); k++) // k-th edge in nfa[j]
            {
                unsigned int symbol = (*subset[j])[k].data.value;
                int l;
                if (symbol == EPSILON)
                    continue;
                for (l = 0; l < symbols.size(); l++)
                    if (symbols[l] == symbol)
                    {
                        dest[l] += *(list<vertex_t *> *)(*subset[j])[k].to->aux->aux;
                        break;
                    }
                if (l == symbols.size())
                {
                    symbols.append(symbol);
                    dest.append(list<vertex_t *>());
                    dest.top() += *(list<vertex_t *> *)(*subset[j])[k].to->aux->aux;
                }
            }
        }
        for (int j = 0; j < dest.size(); j++)
        {
            dest[j].set_reduce();
            unsigned int hash = h(dest[j], hash_size);
            int k;
            for (k = 0; k < aidx[hash].size(); k++)
                if (dest[j] == aidx[hash][k].subset)
                {
                    dfa.g.add_edge(i, aidx[hash][k].idx, {symbols[j]});
                    break;
                }
            if (k == aidx[hash].size())
            {
                dfa.g.add_vertex({size++, NON_TERMINAL});
                dfa.g.top().aux = (vertex_t *)new (std::nothrow) list<vertex_t *>;
                if (dfa.g.top().aux == NULL)
                {
#ifdef HANDLE_MEMORY_EXCEPTION
                    HANDLE_MEMORY_EXCEPTION;
#endif
                }
                *(list<vertex_t *> *)dfa.g.top().aux = dest[j];
                dfa.g.add_edge(i, dfa.g.size() - 1, {symbols[j]});
                aidx[hash].append({dfa.g.size() - 1, dest[j]});
            }
        }
    }
    delete[] aidx;
    for (int i = 0; i < nfa.g.size(); i++)
        nfa.g[i].aux = (vertex_t *)0;
    for (int i = 0; i < nfa.f.size(); i++)
        nfa.f[i]->aux = (vertex_t *)1; // aux represents whether is a terminal
    for (int i = 0; i < dfa.g.size(); i++)
    {
        vertex_t &v = dfa.g[i];
        list<vertex_t *> &subset = *(list<vertex_t *> *)dfa.g[i].aux;
        v.data.token = NON_TERMINAL;
        for (int j = 0; j < subset.size(); j++)
        {
            if (subset[j]->data.token < v.data.token)
                v.data.token = subset[j]->data.token;
            if (subset[j]->aux == (vertex_t *)1 && dfa.f.top() != &dfa.g[i])
                dfa.f.append(&dfa.g[i]);
        }
    }
    nfa.g.mapping(dfa.g, 0);
    dfa.s = nfa.s->aux;
    dfa.sigma_range = nfa.sigma_range;
    for (int i = 0; i < dfa.g.size(); i++)
        delete (list<vertex_t *> *)(dfa.g[i].aux);
    for (int i = 0; i < dfa.g.size(); i++)
        dfa.g[i].aux = (vertex_t *)0;
    if (dfa.g.size() == 0)
        dfa = fa();
    else
    {
        Traverse(dfa.s);
        for (int i = 0; i < dfa.f.size(); i++)
            if (dfa.f[i]->aux == (vertex_t *)0)
                dfa.f.remove(i--);
        for (int i = 0; i < dfa.g.size(); i++)
            if (dfa.g[i].aux == (vertex_t *)0)
                dfa.g.remove_vertex(i--);
    }
    NormalizeID(dfa);
}

/**
 * @brief Minimize a DFA
 * 
 * @param dfa the DFA to minimize
 */
void MinimizeDFA(fa &dfa)
{
    dfa.g.add_vertex({UNDEFINED, NON_TERMINAL});
    list<int> group; // the group number of each dfa state
    for (int i = 0; i < dfa.g.size(); i++)
        group.append(0);
    for (int i = 0; i < dfa.g.size(); i++)
        dfa.g[i].aux = (vertex_t *)(long long)i;
    for (int i = 0; i < dfa.f.size(); i++)
        group[(int)(long long)dfa.f[i]->aux] = 1;
    list<unsigned int> symbols;
    for (int i = 0; i < dfa.g.size(); i++)
        for (int j = 0; j < dfa.g[i].size(); j++)
            symbols.append(dfa.g[i][j].data.value);
    symbols.set_reduce();
    bool changed;
    int group_id;
    do
    {
        list<list<int>> next;
        for (int i = 0; i < dfa.g.size(); i++)
        {
            next.append(list<int>());
            next[i].append(group[i]);
            next[i].append(dfa.g[i].data.token);
        }
        for (int i = 0; i < symbols.size(); i++) // the i-th symbol
        {
            unsigned int symbol = symbols[i];
            for (int j = 0; j < dfa.g.size(); j++) // the j-th state
            {
                int k = 0;
                for (k = 0; k < dfa.g[j].size(); k++)
                    if (dfa.g[j][k].data.value == symbol)
                    {
                        next[j].append(group[(int)(long long)dfa.g[j][k].to->aux]);
                        break;
                    }
                if (k == dfa.g[j].size())
                    next[j].append(group[(int)(long long)dfa.g[dfa.g.size() - 1].aux]); // dead state
            }
        }
        changed = false;
        for (int i = 0; i < next.size(); i++)
            next[i].append(i); // next[i]: original-group, next_1, ..., next_n, graph-index
        next.sort(0, next.size() - 1);
        group[0] = group_id = 0;
        for (int i = 1; i < dfa.g.size(); i++)
        {
            unsigned int idx = next[i].top(), idx_pre = next[i - 1].top();
            next[i].pop_back();
            next[i - 1].pop_back();
            if (!(next[i] == next[i - 1]))
            {
                group_id++;
                changed |= (next[i][0] == next[i - 1][0]);
            }
            group[idx] = group_id;
            next[i].push_back(idx);
            next[i - 1].push_back(idx_pre);
        }
    } while (changed);
    list<list<unsigned int>> idx; // list of states' indices by group
    for (int i = 0; i <= group_id; i++)
        idx.append(list<unsigned int>());
    for (int i = 0; i < dfa.g.size(); i++)
        idx[group[i]].append(i);
    for (int i = 0; i < dfa.g.size(); i++)
        dfa.g[i].aux = &dfa.g[idx[group[i]][0]]; // the representitive of each group
    vertex_t *dead_state = dfa.g.top().aux;
    for (int i = 0; i < dfa.g.size(); i++)
        for (int j = 0; j < dfa.g[i].size(); j++)
            dfa.g[i][j].to = dfa.g[i][j].to->aux;
    dfa.s = dfa.s->aux;
    list<unsigned int> rm_idx;
    for (int i = 0; i < dfa.g.size(); i++)
        dfa.g[i].aux = (vertex_t *)0;
    for (int i = 0; i <= group_id; i++)
    {
        for (int j = 1; j < idx[i].size(); j++)
        {
            dfa.g[idx[i][0]].merge(dfa.g[idx[i][j]]);
            dfa.g[idx[i][j]].aux = (vertex_t *)1; // will be removed
            rm_idx.append(idx[i][j]);
        }
        if (&dfa.g[idx[i][0]] == dead_state)
            rm_idx.append(idx[i][0]);
        dfa.g[idx[i][0]].set_reduce();
    }
    for (int i = 0; i < dfa.f.size(); i++)
        if (dfa.f[i]->aux == (vertex_t *)1)
            dfa.f.remove(i--);
    rm_idx.sort(0, rm_idx.size() - 1);
    for (int i = 0; i < rm_idx.size(); i++)
        dfa.g.remove_vertex(rm_idx[i] - i);
    if (dfa.g.size() == 0)
    {
        dfa.g.add_vertex({0, NON_TERMINAL});
        dfa.s = &dfa.g.top();
        dfa.f = list<vertex_t *>();
    }
    NormalizeID(dfa);
}
