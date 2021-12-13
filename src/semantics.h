/*********************************************************
 * @file semantics.h                                     *
 * @brief Define class of every symbol.                  *
 *        For variables, the class name has prefix "v_". *
 *        For terminals, the class name has prefix "t_", *
 *                       and has an initial element lex, *
 *                       representing lexeme string.     *
 *        Users should define attributes for each class. *
 *********************************************************/

#include "LALR.h"

/***********************************************
 * The symbols are listed below:               *
 *    0              '+'   T                   *
 *    1              '-'   T                   *
 *    2              '*'   T                   *
 *    3              '/'   T                   *
 *    4              '('   T                   *
 *    5              ')'   T                   *
 *    6              'a'   T                   *
 *    7  [#x20#xA#xD#x9]   T                   *
 *    8                E   V                   *
 *    9                T   V                   *
 *   10                F   V                   *
 *   11                S   I                   *
 * (V -> variable, T -> terminal, I -> ignore) *
 ***********************************************/

// class for Aug
class v_0
{
public:
};

// class for E
class v_1
{
public:
};

// class for T
class v_2
{
public:
};

// class for F
class v_3
{
public:
};

// class for '+'
class t_0
{
public:
};

// class for '-'
class t_1
{
public:
};

// class for '*'
class t_2
{
public:
};

// class for '/'
class t_3
{
public:
};

// class for '('
class t_4
{
public:
};

// class for ')'
class t_5
{
public:
};

// class for 'a'
class t_6
{
public:
};

extern const void *(*f_list[])(const void **);
extern const void *(*ft_list[])(const token_info &);
