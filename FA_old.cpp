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
 * @brief Construct a default re object
 * 
 */
re::re()
{
    expression = (unsigned int *)malloc(sizeof(unsigned int) * 2);
    expression[0] = OP_RPRTH;
    expression[1] = 0;
    token = NON_TERMINAL;
}

/**
 * @brief Construct a new re object from another object
 * 
 * @param b the other object
 */
re::re(const re &b)
{
    int len = 0;
    while (b.expression[len] != 0)
        len++;
    expression = (unsigned int *)malloc(sizeof(unsigned int) * (len + 1));
    for (int i = 0; i <= len; i++)
        expression[i] = b.expression[i];
    token = b.token;
}

/**
 * @brief Construct a new re object by elements
 * 
 * @param EXPR expression
 * @param TOKEN token
 */
re::re(const unsigned int *EXPR, unsigned int TOKEN)
{
    int len = 0;
    while (EXPR[len] != 0)
        len++;
    expression = (unsigned int *)malloc(sizeof(unsigned int) * (len + 1));
    for (int i = 0; i <= len; i++)
        expression[i] = EXPR[i];
    token = TOKEN;
}

/**
 * @brief Destroy the re::re object
 * 
 */
re::~re()
{
    if (expression != NULL)
        free(expression);
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
 * @brief Comparison between two vertices
 * 
 * @param a one vertex
 * @param b another vertex
 * @return whether value in a is less than in b
 */
bool operator<(const vertex_info &a, const vertex_info &b) { return a.value < b.value; }

/**
 * @brief Comparison between two edge
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
    for (int i = 0; i < g.size() - 1; i++)
    {
        vertex_t &v = g[i];
        v.sort();
        int j = v.size() - 1;
        for (unsigned int k = sigma_range - 1; k >= 0; k--)
            if (j >= 0 && v[j].data.value == k)
                j--;
            else
                g.add_edge(i, g.size() - 1, {k});
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
 * @brief Kleen Closure operation
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
fa AtomicFA(unsigned int sigma)
{
    fa a;
    a.g.add_vertex({id++, NON_TERMINAL});
    a.g.add_edge(0, 1, {sigma});
    a.s = &a.g[0];
    a.f.append(&a.g[1]);
    a.sigma_range = sigma + 1;
    return a;
}

/**
 * @brief Convert a regular expression to NFA
 * 
 * @param relist the regular expression group
 * @param nfa the result nfa
 * @return whether succeeded
 */
bool REToNFA(list<re> relist, fa &nfa)
{
    nfa = fa();
    for (int i = 0; i < relist.size(); i++)
    {
        list<fa> fa_stack;
        list<unsigned int> op_stack;
        op_stack.push_back(OP_LPRTH);
        int type = 0; // the next character type (operand: 0; operator: 1)
        for (unsigned int *p = relist[i].expression; !op_stack.empty(); p++)
        {
            if ((*p == OP_RPRTH || *p == OP_KLCLS || *p == OP_CNCAT || *p == OP_ALTER ? 1 : 0) != type)
                return false;
            type = *p == OP_LPRTH || *p == OP_CNCAT || *p == OP_ALTER ? 0 : 1;
            switch (*p)
            {
            case OP_LPRTH:
                op_stack.push_back(OP_LPRTH);
                break;
            case OP_RPRTH:
            case OP_KLCLS:
            case OP_CNCAT:
            case OP_ALTER:
                while (*p <= op_stack.top())
                {
                    switch (op_stack.top())
                    {
                    case OP_KLCLS:
                        *fa_stack.top();
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
                fa_stack.push_back(AtomicFA(*p));
                break;
            }
        }
        for (int j = 0; j < fa_stack.top().f.size(); j++)
            fa_stack.top().f[j]->data.token = relist[i].token;
        if (nfa.sigma_range == 0)
            nfa = fa_stack.top();
        else
            nfa |= fa_stack.top();
    }
    NormalizeID(nfa);
    return true;
}

/**
 * @brief Epsilon-closure calculation by recursion
 * 
 * @param nfa the nfa
 * @param i the index
 * @return the epsilon closure
 */
list<vertex_t *> EpsilonClosure(fa &nfa, int i)
{
    list<vertex_t *> epsilon_closure;
    list<vertex_t *> st;
    for (int i = 0; i < nfa.g.size(); i++)
        nfa.g[i].aux = (vertex_t *)0; // not visited
    st.push_back(&nfa.g[i]);
    nfa.g[i].aux = (vertex_t *)1;
    while (!st.empty())
    {
        vertex_t *pv = st.top();
        st.pop_back();
        epsilon_closure += pv;
        for (int i = 0; i < pv->size(); i++)
        {
            if ((*pv)[i].data.value == EPSILON && (*pv)[i].get_to()->aux == (vertex_t *)0)
            {
                st.push_back((*pv)[i].get_to());
                (*pv)[i].get_to()->aux = (vertex_t *)1;
            }
        }
    }
    return epsilon_closure;
}

/**
 * @brief Convert an NFA to equivalent DFA
 * 
 * @param nfa the NFA to convert
 * @param dfa the result DFA
 */
void NFAToDFA(fa nfa, fa &dfa)
{
    dfa.g = graph<vertex_info, edge_info>();
    try
    {
        // the epsilon closure will be pointed to by aux in dfa
        for (int i = 0; i < nfa.g.size(); i++)
        {
            dfa.g.add_vertex(nfa.g[i].data);
            dfa.g[i].aux = (vertex_t *)new list<vertex_t *>;
            list<vertex_t *> &subset = *(list<vertex_t *> *)dfa.g[i].aux;
            subset = EpsilonClosure(nfa, i);
            subset.sort(0, subset.size() - 1);
        }
    }
    catch (std::bad_alloc)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
    }
    nfa.g.mapping(dfa.g, 0);
    unsigned int size = nfa.g.size();
    for (int i = 0; i < dfa.g.size(); i++) // i-th dfa-state
    {
        // elements in subset is the pointer to vertices of nfa
        list<vertex_t *> &subset = *(list<vertex_t *> *)dfa.g[i].aux;
        list<unsigned int> symbols;
        list<list<vertex_t *>> dest;
        for (int j = 0; j < subset.size(); j++) // j-th nfa-state in  dfa-state subset
        {
            for (int k = 0; k < subset[j]->size(); k++) // k-th edge in nfa[j]
            {
                int symbol = (*subset[j])[k].data.value, l;
                if (symbol == EPSILON)
                    continue;
                for (l = 0; l < symbols.size(); l++)
                    if (symbols[l] == symbol)
                    {
                        dest[l] += *(list<vertex_t *> *)(*subset[j])[k].get_to()->aux->aux;
                        break;
                    }
                if (l == symbols.size())
                {
                    symbols.append(symbol);
                    dest.append(list<vertex_t *>());
                    dest.top() += *(list<vertex_t *> *)(*subset[j])[k].get_to()->aux->aux;
                }
            }
        }
        for (int j = 0; j < dest.size(); j++)
        {
            dest[j].set_reduce();
            int k;
            for (k = 0; k < dfa.g.size(); k++)
                if (dest[j] == *(list<vertex_t *> *)dfa.g[k].aux)
                {
                    dfa.g.add_edge(i, k, {symbols[j]});
                    break;
                }
            if (k == dfa.g.size())
            {
                dfa.g.add_vertex({size++, NON_TERMINAL});
                try
                {
                    dfa.g.top().aux = (vertex_t *)new list<vertex_t *>;
                }
                catch (std::bad_alloc)
                {
#ifdef HANDLE_MEMORY_EXCEPTION
                    HANDLE_MEMORY_EXCEPTION;
#endif
                }
                *(list<vertex_t *> *)dfa.g.top().aux = dest[j];
                dfa.g.add_edge(i, dfa.g.size() - 1, {symbols[j]});
            }
        }
    }
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
    bool finished;
    do
    {
        finished = true;
        for (int i = 0; i < dfa.g.size(); i++)
            dfa.g[i].aux = (vertex_t *)0;
        for (int i = 0; i < dfa.g.size(); i++)
            for (int j = 0; j < dfa.g[i].size(); j++)
                dfa.g[i][j].get_to()->aux = (vertex_t *)1; // aux represents whether is pointed
        for (int i = 0; i < dfa.g.size(); i++)
            if (&dfa.g[i] != dfa.s && !dfa.g[i].aux)
            {
                for (int j = 0; j < dfa.f.size(); j++)
                    if (dfa.f[j] == &dfa.g[i])
                        dfa.f.remove(j--);
                dfa.g.remove_vertex(i--);
                finished = false;
            }
        for (int i = 0; i < dfa.g.size(); i++)
            dfa.g[i].aux = (vertex_t *)0;
        for (int i = 0; i < dfa.f.size(); i++)
            dfa.f[i]->aux = (vertex_t *)1; // aux represents whether is a terminal
        for (int i = 0; i < dfa.g.size(); i++)
            if (!dfa.g[i].aux && dfa.g[i].size() == 0)
            {
                for (int j = 0; j < dfa.f.size(); j++)
                    if (dfa.f[j] == &dfa.g[i])
                        dfa.f.remove(j--);
                dfa.g.remove_vertex(i--);
                finished = false;
            }
    } while (!finished);
    NormalizeID(dfa);
}

/**
 * @brief Minimize a DFA
 * 
 * @param dfa the DFA to minimize
 */
void MinimizeDFA(fa &dfa)
{
}
