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

#define UNDEFINED ((int)0x7fffffff)         // undefined transition in DFA table
#define NON_ACC ((unsigned int)0xffffffff)  // non-acceptance state token
#define EPSILON ((unsigned int)0xffffffff)  // epsilon symbol
#define TERMINAL ((unsigned int)0xfffffffe) // terminal symbol
#define AUGMNTED ((unsigned int)0xfffffffd) // augmented grammar start symbol
#define OP_KLCLS ((unsigned int)0xfffffffc) // '*'
#define OP_PTCLS ((unsigned int)0xfffffffb) // '+'
#define OP_OPTNL ((unsigned int)0xfffffffa) // '?'
#define OP_CMPLM ((unsigned int)0xfffffff9) // '~'
#define OP_CNCAT ((unsigned int)0xfffffff8) // '.'
#define OP_ALTER ((unsigned int)0xfffffff7) // '|'
#define OP_MINUS ((unsigned int)0xfffffff6) // '-'
#define OP_RPRTH ((unsigned int)0xfffffff5) // ')'
#define OP_LPRTH ((unsigned int)0xfffffff4) // '('

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

struct hash_subset_info
{
    int idx;
    list<vertex_t *> subset;
};

class expr
{
public:
    unsigned int lhs;
    list<unsigned int> rhs;
    bool operator<(const expr &b) const;
    expr operator|(const expr &b) const;
    expr operator<<(const expr &b) const;
    expr operator*() const;
    expr operator~() const;
};

class token_info
{
public:
    unsigned int token;
    wchar_t *lexeme;
    token_info(const token_info &b);
    token_info(unsigned int TOKEN = NON_ACC, const wchar_t *LEXEME = NULL, int len = -1);
    token_info &operator=(const token_info &b);
    ~token_info();
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
    unsigned int *sep, *token;
    int *table;
    int row, col, s;
    bool *f;
    void copy(const dfa_table &b);

public:
    dfa_table(const dfa_table &b);
    dfa_table(fa dfa, list<unsigned int> lsep = list<unsigned int>());
    ~dfa_table();
    dfa_table &operator=(const dfa_table &b);
    int *operator[](int idx);
    int get_row();
    int get_col();
    int get_start();
    unsigned int get_token(int idx);
    bool is_acceptable(int idx);
    int next(int state, unsigned int ch);
    bool token_stream(const wchar_t *str, list<token_info> &tokens);
};

bool operator<(const vertex_info &a, const vertex_info &b);
bool operator<(const edge_info &a, const edge_info &b);

void NormalizeID(fa &nfa);
fa AtomicFA(unsigned int sigma, unsigned sigma_range = 0);
bool REToNFA(list<expr> relist, fa &nfa, unsigned int symbol_range = 0);
void NFAToDFA(fa nfa, fa &dfa);
void MinimizeDFA(fa &dfa);
bool ContainEpsilon(expr re);
