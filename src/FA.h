/****************************************************************
 * @file FA.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structure and function to process finite automata
 * @version 0.1
 * @date 2021-10-15
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "expr.h"
#include "graph.h"

struct edge_info
{
    unsigned int value; // edge value, representing characters in alphabet
};

struct vertex_info
{
    unsigned int value;  // state id
    unsigned int output; // state output e.g., tokens
};

typedef vertex<vertex_info, edge_info> vertex_t;
typedef edge<vertex_info, edge_info> edge_t;

class fa
{
private:
    void copy(const fa &b);

public:
    graph<vertex_info, edge_info> g;
    vertex_t *s;
    list<vertex_t *> f;
    unsigned int sigma_range; // Sigma = [0, Sigma_range) & N
    fa();
    fa(const fa &b);
    fa &operator=(const fa &b);
    fa &operator|=(fa &b);
    fa &operator<<=(fa &b);
    fa &operator~();
    fa &operator*();
};

bool operator<(const vertex_info &a, const vertex_info &b);
bool operator<(const edge_info &a, const edge_info &b);

bool REToNFA(list<expr> relist, fa &nfa, unsigned int symbol_range = 0);
void NFAToDFA(fa nfa, fa &dfa);
void MinimizeDFA(fa &dfa);
