/****************************************************************
 * @file FA.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structures and functions to process finite automata
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
#include <malloc.h>

#define UNDEFINED ((int)0x7fffffff)             // undefined transition in DFA table
#define NON_TERMINAL ((unsigned int)0xffffffff) // non-terminal state token
#define EPSILON ((unsigned int)0xffffffff)      // value representing epsilon
#define OP_TRMNL ((unsigned int)0xfffffffe)     // RE string terminal
#define OP_KLCLS ((unsigned int)0xfffffffd)     // '*'
#define OP_PTCLS ((unsigned int)0xfffffffc)     // '+'
#define OP_OPTNL ((unsigned int)0xfffffffb)     // '?'
#define OP_CMPLM ((unsigned int)0xfffffffa)     // '~'
#define OP_CNCAT ((unsigned int)0xfffffff9)     // '.'
#define OP_ALTER ((unsigned int)0xfffffff8)     // '|'
#define OP_MINUS ((unsigned int)0xfffffff7)     // '-'
#define OP_RPRTH ((unsigned int)0xfffffff6)     // ')'
#define OP_LPRTH ((unsigned int)0xfffffff5)     // '('

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

class expr
{
public:
    // an extra terminal are needed
    // which is EXPR -> EXPR# (# -> OP_TRMNL)
    unsigned int lhs;
    unsigned int *rhs;
    expr(const expr &b);
    expr(unsigned int LHS);
    expr(unsigned int LHS, const unsigned int *RHS);
    ~expr();
    expr &operator=(const expr &b);
    bool operator<(const expr &b) const;
    expr operator|(const expr &b) const;
    expr operator<<(const expr &b) const;
    expr operator*() const;
    expr operator~() const;
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
    // [0, 0x110000) -> [0, sep[0]), [sep[0], sep[1]), ..., [sep[col - 2], sep[col - 1])
    // sep[col - 1] = 0x110000
    unsigned int *table, *sep;
    int row, col, s;
    bool *f;
    void copy(const dfa_table &b);

public:
    dfa_table(const dfa_table &b);
    dfa_table(fa dfa, list<unsigned int> lsep);
    ~dfa_table();
    dfa_table &operator=(const dfa_table &b);
    unsigned int *operator[](int idx);
    int get_row();
    int get_col();
    int get_start();
    bool is_acceptable(int idx);
    int next(int state, unsigned int ch);
};

struct hash_subset_info
{
    int idx;
    list<vertex_t *> subset;
};

extern unsigned int id;

bool operator<(const vertex_info &a, const vertex_info &b);
bool operator<(const edge_info &a, const edge_info &b);

void NormalizeID(fa &nfa);
fa AtomicFA(unsigned int sigma, unsigned sigma_range = 0);
bool REToNFA(list<expr> relist, fa &nfa, unsigned int symbol_range = 0);
void NFAToDFA(fa nfa, fa &dfa);
void MinimizeDFA(fa &dfa);
