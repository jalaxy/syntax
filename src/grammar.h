/****************************************************************
 * @file grammar.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structures and functions to process grammar
 * @version 0.1
 * @date 2021-10-24
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "list.h"
#include "FA.h"

#define HASH_SZ ((unsigned int)1024)
#define LL1_PARSING_ERROR ((unsigned int)0xffffffff)

struct prod
{
    unsigned int variable;               // id of the variable
    list<list<unsigned int>> expression; // concatenate of the elements
};

struct prod_single
{
    unsigned int variable;
    list<unsigned int> expression;
};

struct parsing_tree
{
    int id;
    unsigned int symbol;
    token_info token;
    list<parsing_tree> subtree;
};

struct hash_symbol_info
{
    bool regular;
    int idx;
    unsigned int symbol;
};

struct hash_string_info
{
    int idx;
    list<wchar_t> str;
};

struct lr1_item
{
    prod_single pd;
    int dot;
    unsigned int sym;
};

struct hash_lr1_items_info
{
    int idx;
    lr1_item item;
};

struct action_info
{
    enum move_enum
    {
        SHIFT,
        REDUCE,
        ACCEPT,
        ERROR
    } mov;
    prod pd;
    int s;
};

class grammar
{
public:
    list<prod> productions;
    prod *s;
    void augment();
};

class ll1_parsing_table
{
private:
    list<unsigned int> *table;
    list<hash_symbol_info> *aidx;
    int row, col;
    unsigned int s;
    void copy(const ll1_parsing_table &b);
    void calc_first(grammar &g, list<unsigned int> *first, unsigned int variable);

public:
    ll1_parsing_table(const ll1_parsing_table &b);
    ll1_parsing_table(grammar g);
    ~ll1_parsing_table();
    const ll1_parsing_table &operator=(const ll1_parsing_table &b);
    int get_row();
    int get_col();
    unsigned int get_start();
    const list<hash_symbol_info> *get_index();
    list<unsigned int> *operator[](int idx);
};

class lr1_parsing_table
{
private:
    int row, col;
    int *table;
    unsigned int *token;
    int reduction_size;
    prod_single *reductions;
    int terminal_head;
    list<hash_symbol_info> *aidx;
    int s;
    int err_code;
    void copy(const lr1_parsing_table &b);
    list<unsigned int> calc_first(grammar &g, unsigned int variable, bool init = true);

public:
    lr1_parsing_table(const lr1_parsing_table &b);
    lr1_parsing_table(grammar g);
    ~lr1_parsing_table();
    const lr1_parsing_table &operator=(const lr1_parsing_table &b);
    const int get_start();
    const list<hash_symbol_info> *get_index();
    const int get_terminal_head();
    const prod_single &get_reduction(int idx);
    unsigned int get_token(int idx);
    int get_error_code();
    int get_row();
    int get_col();
    int *operator[](int idx);
};

bool EBNFToGrammar(list<expr> ebnflist, list<expr> &relist, unsigned int s, grammar &g,
                   bool detect_regular = true);
// EBNF to RE -> EBNF to FA will require less time
int ReadEBNFFromString(wchar_t *wbuf, list<expr> &ebnflist, unsigned int &start, list<expr> &relist,
                       list<unsigned int> &sep, list<list<wchar_t>> &names);
bool LL1Parsing(ll1_parsing_table t_ll1, dfa_table t_fa, parsing_tree &tr, const wchar_t *str);
bool LR1Parsing(lr1_parsing_table t_lr1, dfa_table t_fa, parsing_tree &tr, const wchar_t *str);

// Extended Backus-Naur Form (EBNF) notation, from XML specification
// Reference link: https://www.w3.org/TR/xml/

// Each rule in the grammar defines
// one symbol, in the form
//     symbol ::= expression
// Symbols are written with an initial capital letter if they are the start
// symbol of a regular language, otherwise with an initial lowercase letter.
// Literal strings are quoted.

// Within the expression on the right-hand side of a rule, the following expressions
// are used to match strings of one or more characters:
//     #xN
// where N is a hexadecimal integer, the expression matches the character whose
// number (code point) in ISO/IEC 10646 is N. The number of leading zeros in the
// #xN form is insignificant.
//     [a-zA-Z], [#xN-#xN]
// matches any Char with a value in the range(s) indicated (inclusive).
//     [abc], [#xN#xN#xN]
// matches any Char with a value among the characters enumerated. Enumerations and
// ranges can be mixed in one set of brackets.
//     [^a-z], [^#xN-#xN]
// matches any Char with a value outside the range indicated.
//     [^abc], [^#xN#xN#xN]
// matches any Char with a value not among the characters given. Enumerations and
// ranges of forbidden values can be mixed in one set of brackets.
//     "string"
// matches a literal string matching that given inside the double quotes.
//     'string'
// matches a literal string matching that given inside the single quotes.

// These symbols may be combined to match more complex patterns as follows, where
// A and B represent simple expressions:
//     (expression)
// expression is treated as a unit and may be combined as described in this list.
//     A?
// matches A or nothing; optional A.
//     A B
// matches A followed by B. This operator has higher precedence than alternation;
// thus A B | C D is identical to (A B) | (C D).
//     A | B
// matches A or B.
//     A - B
// matches any string that matches A but does not match B.
//     A+
// matches one or more occurrences of A. Concatenation has higher precedence than
// alternation; thus A+ | B+ is identical to (A+) | (B+).
//     A*
// matches zero or more occurrences of A. Concatenation has higher precedence than
// alternation; thus A* | B* is identical to (A*) | (B*).

// Other notations used in the productions are:
//     /* ... */
// comment.
