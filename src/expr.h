/****************************************************************
 * @file expr.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structure and function declaration of expression
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "list.h"

#define UNDEFINED ((int)0x7fffffff)         // undefined transition in DFA table
#define NON_ACC ((unsigned int)0xffffffff)  // non-acceptance state token
#define EPSILON ((unsigned int)0xffffffff)  // epsilon symbol
#define ENDMARK ((unsigned int)0xfffffffe)  // terminal symbol
#define AUGMNTED ((unsigned int)0xfffffffd) // augmented grammar start symbol
#define REDCTION ((unsigned int)0xfffffffc) // reduction placeholder
#define OP_KLCLS ((unsigned int)0xfffffffb) // '*'
#define OP_PTCLS ((unsigned int)0xfffffffa) // '+'
#define OP_OPTNL ((unsigned int)0xfffffff9) // '?'
#define OP_CMPLM ((unsigned int)0xfffffff8) // '~'
#define OP_CNCAT ((unsigned int)0xfffffff7) // '.'
#define OP_ALTER ((unsigned int)0xfffffff6) // '|'
#define OP_MINUS ((unsigned int)0xfffffff5) // '-'
#define OP_RPRTH ((unsigned int)0xfffffff4) // ')'
#define OP_LPRTH ((unsigned int)0xfffffff3) // '('
#define VARIABLE ((unsigned int)0xfffffff2) // variable type
#define TERMINAL ((unsigned int)0xfffffff1) // terminal type
#define IGNORSYM ((unsigned int)0xfffffff0) // symbols ignored

#ifndef HASH_SZ
#define HASH_SZ 32768
#endif

struct hash_symbol_info
{
    bool regular;
    int idx;
    unsigned int symbol;
};

class expr
{
public:
    unsigned int lhs;
    list<unsigned int> rhs;
    unsigned int prior() const;
    bool operator<(const expr &b) const;
    expr operator|(const expr &b) const;
    expr operator<<(const expr &b) const;
    expr operator*() const;
    expr operator~() const;
};

const hash_symbol_info *QuerySymbol(const list<hash_symbol_info> *aidx, unsigned int symbol);
