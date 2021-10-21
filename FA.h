/****************************************************************
 * @file FA.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Functions to process Finite Automata
 * @version 0.1
 * @date 2021-10-15
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "graph.h"
#include "list.h"

#define NON_TERMINAL ((unsigned int)0xffffffff)
#define EPSILON ((unsigned int)0xffffffff)
#define OP_KLCLS ((unsigned int)0xffffffff) // '*'
#define OP_CNCAT ((unsigned int)0xfffffffe) // ''
#define OP_ALTER ((unsigned int)0xfffffffd) // '|'
#define OP_RPRTH ((unsigned int)0xfffffffc) // ')'
#define OP_LPRTH ((unsigned int)0xfffffffb) // '('

struct edge_info
{
    unsigned int value; // edge value, representing characters in alphabet
};

struct vertex_info
{
    unsigned int value; // state id
    unsigned int token; // related token, the ids for acceptable states
};

typedef vertex<vertex_info, edge_info> vertex_t;
typedef edge<vertex_info, edge_info> edge_t;

class re
{
public:
    unsigned int *expression; // must be followed by a extra right parenthesis
    unsigned int token;
    re();
    re(const re &b);
    re(const unsigned int *EXPR, unsigned int TOKEN = NON_TERMINAL);
    ~re();
};

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
    const fa &operator=(const fa &b);
    fa &operator|=(fa &b);
    fa &operator<<=(fa &b);
    fa &operator~();
    fa &operator*();
};

struct hash_info
{
    int idx;
    list<vertex_t *> subset;
};

extern unsigned int id;

bool operator<(const vertex_info &a, const vertex_info &b);
bool operator<(const edge_info &a, const edge_info &b);

void NormalizeID(fa &nfa);
fa AtomicFA(unsigned int sigma);
bool REToNFA(list<re> relist, fa &nfa);
void NFAToDFA(fa nfa, fa &dfa);
void MinimizeDFA(fa &dfa);
