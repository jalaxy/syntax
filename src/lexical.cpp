/****************************************************************
 * @file lexical.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Function definition of lexical.h
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#include "lexical.h"
#include <cstring>

/**
 * @brief Construct a new token info object from another object
 * 
 * @param b another object 
 */
token_info::token_info(const token_info &b)
{
    token = b.token;
    int sz = 0;
    if ((lexeme = b.lexeme) == NULL)
        return;
    while (b.lexeme[sz++] != L'\0')
        ;
    lexeme = (wchar_t *)malloc(sizeof(wchar_t) * sz);
    if (lexeme == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return;
    }
    memcpy(lexeme, b.lexeme, sz * sizeof(wchar_t));
}

/**
 * @brief Construct a new token info object from assignment
 * 
 * @param TOKEN the token to copy
 * @param LEXEME the lexeme string to copy
 * @param len the length of lexeme
 */
token_info::token_info(unsigned int TOKEN, const wchar_t *LEXEME, int len)
{
    token = TOKEN;
    if (LEXEME == NULL)
    {
        lexeme = NULL;
        return;
    }
    if (len < 0)
    {
        len = 0;
        while (LEXEME[len] != L'\0')
            len++;
    }
    lexeme = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
    if (lexeme == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return;
    }
    memcpy(lexeme, LEXEME, len * sizeof(wchar_t));
    lexeme[len] = L'\0';
}

/**
 * @brief Assignment function
 * 
 * @param b another object
 * @return reference to this object
 */
token_info &token_info::operator=(const token_info &b)
{
    if (&b != this)
    {
        if (lexeme != NULL)
            free(lexeme);
        token = b.token;
        int sz = 0;
        if ((lexeme = b.lexeme) == NULL)
            return *this;
        while (b.lexeme[sz++] != L'\0')
            ;
        lexeme = (wchar_t *)malloc(sizeof(wchar_t) * sz);
        if (lexeme == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
            return *this;
        }
        memcpy(lexeme, b.lexeme, sz * sizeof(wchar_t));
    }
    return *this;
}

/**
 * @brief Destroy the token info object
 * 
 */
token_info::~token_info()
{
    if (lexeme != NULL)
        free(lexeme);
}

/**
 * @brief Copy from another object
 * 
 * @param b the other object
 */
void transition_table::copy(const transition_table &b)
{
    row = b.row;
    col = b.col;
    s = b.s;
    if (row * col == 0)
    {
        table = NULL;
        sep = token = NULL;
        f = NULL;
        ignore = NULL;
    }
    else
    {
        table = (int *)malloc(sizeof(int) * row * col);
        sep = (unsigned int *)malloc(sizeof(unsigned int) * col);
        token = (unsigned int *)malloc(sizeof(unsigned int) * row);
        f = (bool *)malloc(sizeof(bool) * row);
        ignore = (bool *)malloc(sizeof(bool) * row);
        if (table == NULL || sep == NULL || token == NULL || f == NULL || ignore == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
            return;
        }
        memcpy(table, b.table, sizeof(int) * row * col);
        memcpy(sep, b.sep, sizeof(unsigned int) * col);
        memcpy(token, b.token, sizeof(unsigned int) * row);
        memcpy(f, b.f, sizeof(bool) * row);
        memcpy(ignore, b.ignore, sizeof(bool) * row);
    }
}

/**
 * @brief Construct a new DFA table
 * 
 * @param b the other object
 */
transition_table::transition_table(const transition_table &b) { copy(b); }

/**
 * @brief Construct a new DFA table from a DFA
 * 
 * @param dfa the DFA
 * @param lsep the list of separation values
 */
transition_table::transition_table(fa dfa, list<unsigned int> lsep, list<unsigned int> types)
{
    row = dfa.g.size();
    col = dfa.sigma_range;
    if (lsep.size() == 0)
        for (int i = 1; i < col; i++)
            lsep.append(i);
    if (lsep.size() != col - 1)
    {
        table = NULL;
        sep = token = NULL;
        f = NULL;
        ignore = NULL;
        row = 1;
        col = 0;
        return;
    }
    table = (int *)malloc(sizeof(int) * row * col);
    sep = (unsigned int *)malloc(sizeof(unsigned int) * col);
    token = (unsigned int *)malloc(sizeof(unsigned int) * row);
    f = (bool *)malloc(sizeof(bool) * row);
    ignore = (bool *)malloc(sizeof(bool) * row);
    if (table == NULL || sep == NULL || token == NULL || f == NULL || ignore == NULL)
    {
#ifdef HANDLE_MEMORY_EXCEPTION
        HANDLE_MEMORY_EXCEPTION;
#endif
        return;
    }
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
            table[i * col + j] = UNDEFINED;
        dfa.g[i].aux = (vertex_t *)(long long)i;
    }
    s = (unsigned int)(long long)dfa.s->aux;
    for (int i = 0; i < row; i++)
        for (int j = 0; j < dfa.g[i].size(); j++)
        {
            unsigned int symbol = dfa.g[i][j].data.value;
            if (symbol < dfa.sigma_range && table[i * col + symbol] == UNDEFINED)
                table[i * col + symbol] = (int)(long long)dfa.g[i][j].to->aux;
            else
            {
                free(table);
                free(sep);
                free(token);
                free(f);
                free(ignore);
                table = NULL;
                sep = token = NULL;
                f = NULL;
                ignore = NULL;
                row = 1;
                col = 0;
                return;
            }
        }
    for (int i = 0; i < dfa.g.size(); i++)
        dfa.g[i].aux = (vertex_t *)0;
    for (int i = 0; i < dfa.f.size(); i++)
        dfa.f[i]->aux = (vertex_t *)1;
    for (int i = 0; i < row; i++)
        f[i] = dfa.g[i].aux == (vertex_t *)1;
    for (int i = 0; i < dfa.g.size(); i++)
        ignore[i] = dfa.g[i].data.output.front() < (unsigned int)types.size() &&
                    types[dfa.g[i].data.output.front()] == IGNORSYM;
    if (col > 0)
    {
        for (int i = 0; i < col - 1; i++)
            sep[i] = lsep[i];
        sep[col - 1] = 0x110000;
    }
    for (int i = 0; i < dfa.g.size(); i++)
        token[i] = dfa.g[i].data.output.empty() ? NON_ACC : dfa.g[i].data.output.front();
}

/**
 * @brief Construct a new object from binary buffer
 * 
 * @param p buffer pointer
 */
transition_table::transition_table(const void *p)
{
    if (p == NULL)
    {
        table = NULL;
        sep = token = NULL;
        f = NULL;
        ignore = NULL;
        return;
    }
    unsigned char *pc = (unsigned char *)p;
    row = *(int *)pc;
    pc += sizeof(int);
    col = *(int *)pc;
    pc += sizeof(int);
    s = *(int *)pc;
    pc += sizeof(int);
    if (row * col == 0)
    {
        table = NULL;
        sep = token = NULL;
        f = NULL;
        ignore = NULL;
        return;
    }
    else
    {
        table = (int *)malloc(sizeof(int) * row * col);
        sep = (unsigned int *)malloc(sizeof(unsigned int) * col);
        token = (unsigned int *)malloc(sizeof(unsigned int) * row);
        f = (bool *)malloc(sizeof(bool) * row);
        ignore = (bool *)malloc(sizeof(bool) * row);
        if (table == NULL || sep == NULL || token == NULL || f == NULL || ignore == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
            return;
        }
        for (int i = 0; i < col; i++)
        {
            sep[i] = *(unsigned int *)pc;
            pc += sizeof(unsigned int);
        }
        for (int i = 0; i < row; i++)
        {
            token[i] = *(unsigned int *)pc;
            pc += sizeof(unsigned int);
        }
        for (int i = 0; i < row * col; i++)
        {
            table[i] = *(int *)pc;
            pc += sizeof(int);
        }
        for (int i = 0; i < row; i++)
        {
            f[i] = *(bool *)pc;
            pc += sizeof(bool);
        }
        for (int i = 0; i < row; i++)
        {
            ignore[i] = *(bool *)pc;
            pc += sizeof(bool);
        }
    }
}

/**
 * @brief Destroy the DFA table object
 * 
 */
transition_table::~transition_table()
{
    if (table != NULL)
        free(table);
    if (f != NULL)
        free(f);
    if (ignore != NULL)
        free(ignore);
    if (sep != NULL)
        free(sep);
    if (token != NULL)
        free(token);
}

/**
 * @brief Assignment function
 * 
 * @param b 
 * @return transition_table& 
 */
transition_table &transition_table::operator=(const transition_table &b)
{
    if (&b != this)
    {
        if (table != NULL)
            free(table);
        if (f != NULL)
            free(f);
        if (ignore != NULL)
            free(ignore);
        if (sep != NULL)
            free(sep);
        if (token != NULL)
            free(token);
        copy(b);
    }
    return *this;
}

/**
 * @brief Get next state according to separation
 * 
 * @param state current state
 * @param ch unicode character, from 0 to 0x10FFFF
 * @return next state
 */
int transition_table::next(int state, unsigned int ch)
{
    if (state > row || ch > 0x110000)
        return UNDEFINED;
    unsigned int symbol = 0;
    if (ch >= sep[0])
    {
        int l = 0, r = col;
        while (l + 1 < r)
        {
            int mid = (l + r) / 2;
            if (sep[mid] <= ch)
                l = mid;
            else
                r = mid;
        }
        symbol = l + 1;
    }
    return table[state * col + symbol];
}

/**
 * @brief Convert a string into token stream and lexemes
 * 
 * @param str a string
 * @param tokens the output tokens
 * @return whether it is successful
 */
bool transition_table::token_stream(const wchar_t *str, list<token_info> &tokens)
{
    tokens = list<token_info>();
    const wchar_t *p = str;
    while (*p != L'\0')
    {
        const wchar_t *q = p, *base = p;
        int state = s;
        int acceptable_state = UNDEFINED;
        while (*q != L'\0')
        {
            int next_state = next(state, *q++);
            if (next_state == UNDEFINED)
                break;
            else if (f[state = next_state])
            {
                acceptable_state = state;
                p = q;
            }
        }
        if (acceptable_state == UNDEFINED)
            return false;
        if (!ignore[acceptable_state])
            tokens.append(token_info(token[acceptable_state], base, p - base));
    }
    return true;
}

/**
 * @brief Store the table into binary-byte buffer
 * 
 * @param p the binary-byte buffer
 * @return the size of buffer
 */
int transition_table::store(void *p)
{
    if (p == NULL)
        return sizeof(int) * (3 + row * col) +
               sizeof(unsigned int) * (col + row) +
               sizeof(bool) * (row + row);
    unsigned char *pc = (unsigned char *)p;
    *(int *)pc = row;
    pc += sizeof(int);
    *(int *)pc = col;
    pc += sizeof(int);
    *(int *)pc = s;
    pc += sizeof(int);
    for (int i = 0; i < col; i++)
    {
        *(unsigned int *)pc = sep[i];
        pc += sizeof(unsigned int);
    }
    for (int i = 0; i < row; i++)
    {
        *(unsigned int *)pc = token[i];
        pc += sizeof(unsigned int);
    }
    for (int i = 0; i < row * col; i++)
    {
        *(int *)pc = table[i];
        pc += sizeof(int);
    }
    for (int i = 0; i < row; i++)
    {
        *(bool *)pc = f[i];
        pc += sizeof(bool);
    }
    for (int i = 0; i < row; i++)
    {
        *(bool *)pc = ignore[i];
        pc += sizeof(bool);
    }
    return pc - (unsigned char *)p;
}
