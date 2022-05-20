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
 *    6              '+'   T                   *
 *    7              '-'   T                   *
 *    8            [0-9]   T                   *
 *    9              '.'   T                   *
 *   10            [0-9]   T                   *
 *   11  [#x20#xA#xD#x9]   T                   *
 *   12                E   V                   *
 *   13                T   V                   *
 *   14                F   V                   *
 *   15              int   T                   *
 *   16             frac   T                   *
 *   17                S   I                   *
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
    double val;
};

// class for T
class v_2
{
public:
    double val;
};

// class for F
class v_3
{
public:
    double val;
};

// class for Temp51
class v_4
{
public:
    bool neg;
};

// class for Temp53
class v_5
{
public:
    double val;
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

// class for int
class t_6
{
public:
    double val;
};

// class for frac
class t_7
{
public:
    double val;
};

extern const void *(*f_list[])(const void **);
extern const void *(*ft_list[])(const token_info &);
