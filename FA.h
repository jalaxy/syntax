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
#define OP_ENCLS ((unsigned int)0xffffffff) // '*'
#define OP_CNCAT ((unsigned int)0xfffffffe) // ''
#define OP_ALTER ((unsigned int)0xfffffffd) // '|'
#define OP_RPRTH ((unsigned int)0xfffffffc) // ')'
#define OP_LPRTH ((unsigned int)0xfffffffb) // '('
typedef struct re re_t;
typedef struct fa fa_t;

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

struct re
{
    unsigned int *expression;
    unsigned int token;
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

extern unsigned int id;

bool operator<(const vertex_info &a, const vertex_info &b);
bool operator<(const edge_info &a, const edge_info &b);

fa_t AtomicFA(unsigned int sigma);
bool REToNFA(list<re_t> &relist, fa_t &nfa);
void NFAToDFA(fa_t &fa);
void MinimizeDFA(fa_t &dfa);
