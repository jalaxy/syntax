/*****************************************************
 * @file semantics.cpp                               *
 * @brief Define semantic rules for each production. *
 *        Parameters of functions are pointer type.  *
 *        New objects will be created in functions.  *
 *****************************************************/

#include "semantics.h"
#include <cstdio>

/********************************
 * The grammar is listed below: *
 * Aug ::= E                    *
 * E ::= E '+' T | E '-' T | T  *
 * T ::= T '*' F | T '/' F | F  *
 * F ::= '(' E ')' | 'a'        *
 ********************************/

// function for Aug ::= E
const void *f_0(const void **argv)
{
    v_0 *retval = new (std::nothrow) v_0;
    if (retval == NULL)
        return NULL;
    const v_1 *arg_0 = (const v_1 *)argv[0]; // E

    // add rules here
    printf("\n");

    return retval;
}

// function for E ::= E '+' T
const void *f_1(const void **argv)
{
    v_1 *retval = new (std::nothrow) v_1;
    if (retval == NULL)
        return NULL;
    const v_1 *arg_0 = (const v_1 *)argv[0]; // E
    const t_0 *arg_1 = (const t_0 *)argv[1]; // '+'
    const v_2 *arg_2 = (const v_2 *)argv[2]; // T

    // add rules here
    printf("+");

    return retval;
}

// function for E ::= E '-' T
const void *f_2(const void **argv)
{
    v_1 *retval = new (std::nothrow) v_1;
    if (retval == NULL)
        return NULL;
    const v_1 *arg_0 = (const v_1 *)argv[0]; // E
    const t_1 *arg_1 = (const t_1 *)argv[1]; // '-'
    const v_2 *arg_2 = (const v_2 *)argv[2]; // T

    // add rules here
    printf("-");

    return retval;
}

// function for E ::= T
const void *f_3(const void **argv)
{
    v_1 *retval = new (std::nothrow) v_1;
    if (retval == NULL)
        return NULL;
    const v_2 *arg_0 = (const v_2 *)argv[0]; // T

    // add rules here

    return retval;
}

// function for T ::= T '*' F
const void *f_4(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_2 *arg_0 = (const v_2 *)argv[0]; // T
    const t_2 *arg_1 = (const t_2 *)argv[1]; // '*'
    const v_3 *arg_2 = (const v_3 *)argv[2]; // F

    // add rules here
    printf("*");

    return retval;
}

// function for T ::= T '/' F
const void *f_5(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_2 *arg_0 = (const v_2 *)argv[0]; // T
    const t_3 *arg_1 = (const t_3 *)argv[1]; // '/'
    const v_3 *arg_2 = (const v_3 *)argv[2]; // F

    // add rules here
    printf("/");

    return retval;
}

// function for T ::= F
const void *f_6(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_3 *arg_0 = (const v_3 *)argv[0]; // F

    // add rules here

    return retval;
}

// function for F ::= '(' E ')'
const void *f_7(const void **argv)
{
    v_3 *retval = new (std::nothrow) v_3;
    if (retval == NULL)
        return NULL;
    const t_4 *arg_0 = (const t_4 *)argv[0]; // '('
    const v_1 *arg_1 = (const v_1 *)argv[1]; // E
    const t_5 *arg_2 = (const t_5 *)argv[2]; // ')'

    // add rules here

    return retval;
}

// function for F ::= 'a'
const void *f_8(const void **argv)
{
    v_3 *retval = new (std::nothrow) v_3;
    if (retval == NULL)
        return NULL;
    const t_6 *arg_0 = (const t_6 *)argv[0]; // 'a'

    // add rules here
    printf("a");

    return retval;
}

// function for '+'
const void *ft_0(const token_info &tk)
{
    t_0 *retval = new (std::nothrow) t_0;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '-'
const void *ft_1(const token_info &tk)
{
    t_1 *retval = new (std::nothrow) t_1;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '*'
const void *ft_2(const token_info &tk)
{
    t_2 *retval = new (std::nothrow) t_2;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '/'
const void *ft_3(const token_info &tk)
{
    t_3 *retval = new (std::nothrow) t_3;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '('
const void *ft_4(const token_info &tk)
{
    t_4 *retval = new (std::nothrow) t_4;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for ')'
const void *ft_5(const token_info &tk)
{
    t_5 *retval = new (std::nothrow) t_5;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for 'a'
const void *ft_6(const token_info &tk)
{
    t_6 *retval = new (std::nothrow) t_6;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

const void *(*ft_list[])(const token_info &) = {
    ft_0, ft_1, ft_2, ft_3, ft_4, ft_5, ft_6};
const void *(*f_list[])(const void **) = {
    f_0, f_1, f_2, f_3, f_4, f_5, f_6, f_7, f_8};
