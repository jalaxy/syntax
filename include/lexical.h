/****************************************************************
 * @file lexical.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structure and function of lexical analysis
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "FA.h"

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

class transition_table
{
private:
    // [0, 0x110000) -> [0, sep[0]), [sep[0], sep[1]), ..., [sep[col - 2], sep[col - 1])
    // sep[col - 1] = 0x110000
    unsigned int *sep, *token;
    int *table;
    int row, col, s;
    bool *f, *ignore;
    void copy(const transition_table &b);
    int next(int state, unsigned int ch);

public:
    transition_table(const transition_table &b);
    transition_table(fa dfa, list<unsigned int> lsep = list<unsigned int>(),
                     list<unsigned int> types = list<unsigned int>());
    transition_table(const void *p);
    ~transition_table();
    transition_table &operator=(const transition_table &b);
    bool token_stream(const wchar_t *str, list<token_info> &tokens);
    int store(void *p = NULL);
};
