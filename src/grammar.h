/****************************************************************
 * @file grammar.h
 * @author Name (username@domain.com)
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
using namespace std;

#define HASH_SZ 1024
#define LL1_PARSING_ERROR ((unsigned int)0xffffffff)
#define LL1_PARSING_TERMINAL ((unsigned int)0xffffffff)

struct prod
{
    unsigned int variable;               // id of the variable
    list<list<unsigned int>> expression; // concatenate of the elements
};

struct parsing_tree
{
    unsigned int symbol;
    list<parsing_tree> subtree;
};

class grammar
{
public:
    list<prod> productions;
    prod *s;
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
    list<unsigned int> str;
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
    const hash_symbol_info *query_symbol(unsigned int symbol);
    int get_row();
    int get_col();
    unsigned int get_start();
    list<unsigned int> *operator[](int idx);
};

bool EBNFToGrammar(list<expr> ebnflist, list<expr> &relist, unsigned int s, grammar &g,
                   bool detect_regular = true);
int ReadEBNFFromString(wchar_t *wbuf, list<expr> &ebnflist, unsigned int &start, list<expr> &relist,
                       list<unsigned int> &sep, list<list<unsigned int>> &names);
bool LL1Parsing(ll1_parsing_table t_ll1, dfa_table t_fa, parsing_tree &tr, const wchar_t *str);

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
