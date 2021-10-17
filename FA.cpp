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
    f += b.f;
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
fa &fa::operator<<=(fa_t &b)
{
    g.merge(b.g);
    for (int i = 0; i < f.size(); i++)
        g.add_edge(*f[i], *b.s, {EPSILON});
    f = b.f;
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
 * @brief Enclosure operation
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
 * @brief Atomic automata which can accept single letters
 * 
 * @param sigma 
 * @return fa_t 
 */
fa_t AtomicFA(unsigned int sigma)
{
    fa_t a;
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
bool REToNFA(list<re_t> &relist, fa_t &nfa)
{
    nfa = fa_t();
    for (int i = 0; i < relist.size(); i++)
    {
        list<fa_t> fa_stack;
        list<unsigned int> op_stack;
        op_stack.push_back(OP_LPRTH);
        int type = 0; // the next character type (operand: 0; operator: 1)
        for (unsigned int *p = relist[i].expression; !op_stack.empty(); p++)
        {
            if ((*p == OP_RPRTH || *p == OP_ENCLS || *p == OP_CNCAT || *p == OP_ALTER ? 1 : 0) != type)
                return false;
            type = *p == OP_LPRTH || *p == OP_CNCAT || *p == OP_ALTER ? 0 : 1;
            switch (*p)
            {
            case OP_LPRTH:
                op_stack.push_back(OP_LPRTH);
                break;
            case OP_RPRTH:
            case OP_ENCLS:
            case OP_CNCAT:
            case OP_ALTER:
                while (*p <= op_stack.top())
                {
                    switch (op_stack.top())
                    {
                    case OP_ENCLS:
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
    return true;
}

/**
 * @brief Convert an NFA to equivalent DFA
 * 
 * @param fa the finate automata to convert
 */
void NFAToDFA(fa_t &fa)
{
}

/**
 * @brief Minimize a DFA
 * 
 * @param dfa the DFA to minimize
 */
void MinimizeDFA(fa_t &dfa)
{
}
