/****************************************************************
 * @file LALR.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structure and function declaration of LALR parser
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "expr.h"
#include "FA.h"
#include "grammar.h"
#include "lexical.h"

struct lr1_item
{
    unsigned int variable;
    list<unsigned int> expression;
    int dot;
    unsigned int sym;
};

struct parsing_tree
{
    int id;
    unsigned int symbol;
    token_info token;
    list<parsing_tree> subtree;
};

class lr1_parsing_table
{
private:
    int row, col;
    int *table;
    unsigned int *output; // output on each state, index of reduc_var and reduc_expr
    int reduc_size;
    unsigned int *reduc_var;        // reduction variable i.e. variable (LHS)
    list<unsigned int> *reduc_expr; // reduction expression i.e. rule string (RHS)
    list<hash_symbol_info> *aidx;   // hash table
    int s;
    void copy(const lr1_parsing_table &b);
    list<unsigned int> calc_first(grammar &g, unsigned int variable, bool init = true);

public:
    lr1_parsing_table(const lr1_parsing_table &b);
    lr1_parsing_table(grammar g);
    ~lr1_parsing_table();
    const lr1_parsing_table &operator=(const lr1_parsing_table &b);
    bool parse(list<token_info> tk, parsing_tree &tr);
};
