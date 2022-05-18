/****************************************************************
 * @file grammar.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structure and function declaration of grammar
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "expr.h"
#include "list.h"

struct prod
{
    unsigned int variable;               // id of the variable
    list<list<unsigned int>> expression; // concatenate of the elements
};

class grammar
{
public:
    list<prod> productions;
    prod *s;
    void augment();
};

int ReadEBNFFromString(wchar_t *wbuf, list<expr> &ebnflist, unsigned int &start, list<expr> &relist,
                       list<unsigned int> &sep, list<list<wchar_t>> &names, list<unsigned int> &types);
int EBNFToGrammar(list<expr> ebnflist, grammar &g, unsigned int s, unsigned int id = 0);
int EBNFToRE(list<expr> ebnflist, list<expr> &relist);
void EliminateUnreachableRE(list<expr> &re, grammar g, list<unsigned int> types);
// EBNF to RE -> EBNF to FA may require less time
