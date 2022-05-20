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
 *    0              'extern'   T              *
 *    1                   ';'   T              *
 *    2                   ';'   T              *
 *    3                   '('   T              *
 *    4                   ')'   T              *
 *    5                   ';'   T              *
 *    6                   '('   T              *
 *    7                   ')'   T              *
 *    8                   ','   T              *
 *    9                   '{'   T              *
 *   10                   '}'   T              *
 *   11                   ';'   T              *
 *   12                  'if'   T              *
 *   13                   '('   T              *
 *   14                   ')'   T              *
 *   15                'else'   T              *
 *   16               'while'   T              *
 *   17                   '('   T              *
 *   18                   ')'   T              *
 *   19              'return'   T              *
 *   20                   ';'   T              *
 *   21                   '='   T              *
 *   22                   '+'   T              *
 *   23                   '-'   T              *
 *   24                   '*'   T              *
 *   25                   '/'   T              *
 *   26                   '%'   T              *
 *   27                   '('   T              *
 *   28                   ')'   T              *
 *   29                   '['   T              *
 *   30                   ']'   T              *
 *   31                   '('   T              *
 *   32                   ')'   T              *
 *   33                   ','   T              *
 *   34                 [0-9]   T              *
 *   35              [a-zA-Z]   T              *
 *   36                   '.'   T              *
 *   37                   '.'   T              *
 *   38                   '<'   T              *
 *   39                  '<='   T              *
 *   40                   '>'   T              *
 *   41                  '>='   T              *
 *   42                  '=='   T              *
 *   43                  '!='   T              *
 *   44       [#x9#xA#xD#x20]   T              *
 *   45               program   V              *
 *   46          part_program   V              *
 *   47              var_decl   V              *
 *   48               var_def   V              *
 *   49             func_decl   V              *
 *   50              func_def   V              *
 *   51            param_list   V              *
 *   52                 block   V              *
 *   53             statement   V              *
 *   54          if_statement   V              *
 *   55       while_statement   V              *
 *   56      return_statement   V              *
 *   57            expression   V              *
 *   58       bool_expression   V              *
 *   59  secondary_expression   V              *
 *   60                  term   V              *
 *   61                factor   V              *
 *   62                 index   V              *
 *   63                number   V              *
 *   64              variable   V              *
 *   65             func_call   V              *
 *   66              arg_list   V              *
 *   67                 digit   T              *
 *   68                digits   T              *
 *   69                letter   T              *
 *   70            identifier   T              *
 *   71               integer   T              *
 *   72                  real   T              *
 *   73                 RelOp   T              *
 *   74                 space   I              *
 * (V -> variable, T -> terminal, I -> ignore) *
 ***********************************************/

extern FILE *fp_o;

namespace op_t
{
    enum op_t
    {
        NOP,
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        UMIN,
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE,
        ASGN,
        J,
        JL,
        JG,
        ARG,
        CALL,
        RET
    };
}

namespace type_t
{
    enum type_t
    {
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE
    };
}

struct tuple
{
    op_t::op_t op;
    int arg1, arg2, res;
    bool imm1 = false, imm2 = false, imm = false;
};

// class for Aug
class v_0
{
public:
};

// class for program
class v_1
{
public:
    list<tuple> tplist;
};

// class for part_program
class v_2
{
public:
    list<tuple> tplist;
};

// class for var_decl
class v_3
{
public:
};

// class for var_def
class v_4
{
public:
};

// class for func_decl
class v_5
{
public:
};

// class for func_def
class v_6
{
public:
    list<tuple> tplist;
};

// class for param_list
class v_7
{
public:
    list<unsigned int> arg_list;
};

// class for block
class v_8
{
public:
    list<tuple> tplist;
};

// class for statement
class v_9
{
public:
    list<tuple> tplist;
};

// class for if_statement
class v_10
{
public:
    list<tuple> tplist;
};

// class for while_statement
class v_11
{
public:
    list<tuple> tplist;
};

// class for return_statement
class v_12
{
public:
    list<tuple> tplist;
};

// class for expression
class v_13
{
public:
    list<tuple> tplist;
};

// class for bool_expression
class v_14
{
public:
    list<tuple> tplist;
};

// class for secondary_expression
class v_15
{
public:
    list<tuple> tplist;
};

// class for term
class v_16
{
public:
    list<tuple> tplist;
};

// class for factor
class v_17
{
public:
    list<tuple> tplist;
};

// class for index
class v_18
{
public:
    list<tuple> tplist;
};

// class for number
class v_19
{
public:
    enum t
    {
        INT,
        DOUBLE
    } type;
    int val;
    double val_d;
};

// class for variable
class v_20
{
public:
    int addr;
    list<tuple> tplist;
};

// class for func_call
class v_21
{
public:
    int faddr;
    list<tuple> tplist;
};

// class for arg_list
class v_22
{
public:
    list<tuple> tplist;
};

// class for Temp136: index?
class v_23
{
public:
    list<tuple> tplist;
};

// class for Temp156: param_list
class v_24
{
public:
    list<unsigned int> arg_list;
};

// class for Temp159: statement*
class v_25
{
public:
    list<tuple> tplist;
};

// class for Temp175: else statement
class v_26
{
public:
    list<tuple> tplist;
};

// class for Temp220: arg_list
class v_27
{
public:
    list<tuple> tplist;
};

// class for 'extern'
class t_0
{
public:
};

// class for ';'
class t_1
{
public:
};

// class for '('
class t_2
{
public:
};

// class for ')'
class t_3
{
public:
};

// class for ','
class t_4
{
public:
};

// class for '{'
class t_5
{
public:
};

// class for '}'
class t_6
{
public:
};

// class for 'if'
class t_7
{
public:
};

// class for 'else'
class t_8
{
public:
};

// class for 'while'
class t_9
{
public:
};

// class for 'return'
class t_10
{
public:
};

// class for '='
class t_11
{
public:
};

// class for '+'
class t_12
{
public:
};

// class for '-'
class t_13
{
public:
};

// class for '*'
class t_14
{
public:
};

// class for '/'
class t_15
{
public:
};

// class for '%'
class t_16
{
public:
};

// class for '['
class t_17
{
public:
};

// class for ']'
class t_18
{
public:
};

// class for identifier
class t_19
{
public:
    list<wchar_t> name;
};

// class for integer
class t_20
{
public:
    int val;
};

// class for real
class t_21
{
public:
    double val;
};

// class for RelOp
class t_22
{
public:
    type_t::type_t type;
};

extern const void *(*f_list[])(const void **);
extern const void *(*ft_list[])(const token_info &);
