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
#include <cstring>

#define NON_TERMINAL ((unsigned int)0xffffffff) // non-terminal state token
#define UNDEFINED ((unsigned int)0xffffffff)    // undefined transition in DFA table
#define EPSILON ((unsigned int)0xffffffff)      // value representing epsilon
#define OP_TRMNL ((unsigned int)0xfffffffe)     // RE string terminal
#define OP_KLCLS ((unsigned int)0xfffffffd)     // '*'
#define OP_CMPLM ((unsigned int)0xfffffffc)     // '~'
#define OP_CNCAT ((unsigned int)0xfffffffb)     // '.'
#define OP_ALTER ((unsigned int)0xfffffffa)     // '|'
#define OP_RPRTH ((unsigned int)0xfffffff9)     // ')'
#define OP_LPRTH ((unsigned int)0xfffffff8)     // '('

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
    // an extra right parenthesis and a terminal are needed
    // which is RE -> RE)#
    unsigned int *expression;
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

class dfa_table
{
private:
    unsigned int *table, row, col;
    bool *f;
    unsigned int s;
    void copy(const dfa_table &b);

public:
    dfa_table(const dfa_table &b);
    dfa_table(fa dfa);
    ~dfa_table();
    unsigned int *operator[](int idx);
    unsigned int get_row();
    unsigned int get_col();
    unsigned int get_start();
    bool is_acceptable(int idx);
    dfa_table &operator=(const dfa_table &b);
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
fa AtomicFA(unsigned int sigma, unsigned sigma_range = 0);
bool REToNFA(list<re> relist, fa &nfa, unsigned int symbol_range = 0);
void NFAToDFA(fa nfa, fa &dfa);
void MinimizeDFA(fa &dfa);
