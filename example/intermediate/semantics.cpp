/*****************************************************
 * @file semantics.cpp                               *
 * @brief Define semantic rules for each production. *
 *        Parameters of functions are pointer type.  *
 *        New objects will be created in functions.  *
 *****************************************************/

#include "semantics.h"
#include <cstdio>
#define DEF 0x7fffffff

struct hash_string_info
{
    int idx;
    list<wchar_t> str;
};

list<hash_string_info> aidx[HASH_SZ];
list<list<wchar_t>> names;
int addr = 100, tmp = 0;

int h(const list<wchar_t> &l)
{
    int ans = 0;
    for (int i = 0; i < l.size(); i++)
        ans = (ans * 0x80 + ((list<wchar_t>)l)[i]) % HASH_SZ;
    return ans;
}

void add_name(const list<wchar_t> &name)
{
    int i;
    list<hash_string_info> &info = aidx[h(name)];
    for (i = 0; i < info.size(); i++)
        if (info[i].str == name)
            break;
    if (i == info.size())
    {
        info.append({names.size(), name});
        names.append(name);
    }
}

/*****************************************************************************************************************************
 * The grammar is listed below:                                                                                              *
 * Aug ::= program                                                                                                           *
 * program ::= part_program program | part_program                                                                           *
 * part_program ::= var_decl | var_def | func_decl | func_def                                                                *
 * var_decl ::= 'extern' identifier identifier ';'                                                                           *
 * var_def ::= identifier identifier ';'                                                                                     *
 * func_decl ::= identifier identifier '(' param_list ')' ';'                                                                *
 * func_def ::= identifier identifier '(' param_list ')' block                                                               *
 * param_list ::= identifier identifier Temp143 | #E                                                                         *
 * block ::= '{' Temp146 '}'                                                                                                 *
 * statement ::= var_decl | func_decl | var_def | if_statement | while_statement | return_statement | expression ';' | block *
 * if_statement ::= 'if' '(' expression ')' statement Temp162                                                                *
 * while_statement ::= 'while' '(' expression ')' statement                                                                  *
 * return_statement ::= 'return' expression ';'                                                                              *
 * expression ::= bool_expression | expression '=' bool_expression                                                           *
 * bool_expression ::= secondary_expression | bool_expression RelOp secondary_expression                                     *
 * secondary_expression ::= term | secondary_expression '+' term | secondary_expression '-' term                             *
 * term ::= factor | term '*' factor | term '/' factor                                                                       *
 * factor ::= number | '(' expression ')' | variable                                                                         *
 * number ::= integer | real                                                                                                 *
 * variable ::= identifier | func_call                                                                                       *
 * func_call ::= identifier '(' arg_list ')'                                                                                 *
 * arg_list ::= expression Temp198 | #E                                                                                      *
 * Temp143 ::= ',' identifier identifier Temp143 | #E                                                                        *
 * Temp146 ::= statement Temp146 | #E                                                                                        *
 * Temp162 ::= 'else' statement | #E                                                                                         *
 * Temp198 ::= ',' expression Temp198 | #E                                                                                   *
 *****************************************************************************************************************************/

// function for Aug ::= program
const void *f_0(const void **argv)
{
    v_0 *retval = new (std::nothrow) v_0;
    if (retval == NULL)
        return NULL;
    v_1 *arg_0 = (v_1 *)argv[0]; // program

    int len_1 = 0, len_2 = 0, len_3 = 0;
    for (int i = 0; i < arg_0->tplist.size(); i++)
    {
        if ((unsigned int)arg_0->tplist[i].arg1 < (unsigned int)names.size() &&
            names[arg_0->tplist[i].arg1].size() > len_1)
            len_1 = names[arg_0->tplist[i].arg1].size();
        if ((unsigned int)arg_0->tplist[i].arg2 < (unsigned int)names.size() &&
            names[arg_0->tplist[i].arg2].size() > len_2)
            len_2 = names[arg_0->tplist[i].arg2].size();
        if ((unsigned int)arg_0->tplist[i].res < (unsigned int)names.size() &&
            names[arg_0->tplist[i].res].size() > len_3)
            len_3 = names[arg_0->tplist[i].res].size();
    }
    len_1 += 2;
    len_2 += 2;
    len_3 += 2;
    len_1 = len_1 < 8 ? 8 : len_1;
    len_2 = len_2 < 8 ? 8 : len_2;
    len_3 = len_3 < 8 ? 8 : len_3;
    for (int i = 0; i < arg_0->tplist.size(); i++)
    {
        printf("%6d |", i);
        switch (arg_0->tplist[i].op)
        {
        case op_t::ADD:
            printf("     +");
            break;
        case op_t::SUB:
            printf("     -");
            break;
        case op_t::MUL:
            printf("     *");
            break;
        case op_t::DIV:
            printf("     /");
            break;
        case op_t::UMIN:
            printf("    u-");
            break;
        case op_t::LT:
            printf("     <");
            break;
        case op_t::GT:
            printf("     >");
            break;
        case op_t::LE:
            printf("    <=");
            break;
        case op_t::GE:
            printf("    >=");
            break;
        case op_t::EQ:
            printf("    ==");
            break;
        case op_t::ASGN:
            printf("     =");
            break;
        case op_t::J:
            printf("     J");
            break;
        case op_t::JL:
            printf("    J<");
            break;
        case op_t::JG:
            printf("    J>");
            break;
        case op_t::ARG:
            printf("   ARG");
            break;
        case op_t::RET:
            printf("   RET");
            break;
        case op_t::CALL:
            printf("  CALL");
            break;
        default:
            printf("     -");
            break;
        }
        printf(" |");
        if (arg_0->tplist[i].imm1)
            printf("%*d", len_1, arg_0->tplist[i].arg1);
        else if (arg_0->tplist[i].arg1 < names.size())
        {
            if (names[arg_0->tplist[i].arg1].empty())
                printf("%*dt", len_1 - 1, arg_0->tplist[i].arg1);
            else
                for (int j = 0; j < len_1 - names[arg_0->tplist[i].arg1].size(); j++)
                    printf(" ");
            for (int j = 0; j < names[arg_0->tplist[i].arg1].size(); j++)
                printf("%c", names[arg_0->tplist[i].arg1][j]);
        }
        else
        {
            for (int j = 0; j < len_1 - 1; j++)
                printf(" ");
            printf("-");
        }
        printf(" |");
        if (arg_0->tplist[i].imm2)
            printf("%*d", len_2, arg_0->tplist[i].arg2);
        else if (arg_0->tplist[i].arg2 < names.size())
        {
            if (names[arg_0->tplist[i].arg2].empty())
                printf("%*dt", len_2 - 1, arg_0->tplist[i].arg2);
            else
                for (int j = 0; j < len_2 - names[arg_0->tplist[i].arg2].size(); j++)
                    printf(" ");
            for (int j = 0; j < names[arg_0->tplist[i].arg2].size(); j++)
                printf("%c", names[arg_0->tplist[i].arg2][j]);
        }
        else
        {
            for (int j = 0; j < len_2 - 1; j++)
                printf(" ");
            printf("-");
        }
        printf(" |");
        if (arg_0->tplist[i].imm)
            if (arg_0->tplist[i].op == op_t::J ||
                arg_0->tplist[i].op == op_t::JL ||
                arg_0->tplist[i].op == op_t::JG)
                printf("%*d", len_3, arg_0->tplist[i].res + i);
            else
                printf("%*d", len_3, arg_0->tplist[i].res);
        else if (arg_0->tplist[i].res < names.size())
        {
            if (names[arg_0->tplist[i].res].empty())
                printf("%*dt", len_3 - 1, arg_0->tplist[i].res);
            else
                for (int j = 0; j < len_3 - names[arg_0->tplist[i].res].size(); j++)
                    printf(" ");
            for (int j = 0; j < names[arg_0->tplist[i].res].size(); j++)
                printf("%c", names[arg_0->tplist[i].res][j]);
        }
        else if (arg_0->tplist[i].res == DEF)
        {
            for (int j = 0; j < len_3 - 1; j++)
                printf(" ");
            printf("-");
        }
        else
            printf("%6d", arg_0->tplist[i].res);
        printf("\n");
    }

    return retval;
}

// function for program ::= part_program program
const void *f_1(const void **argv)
{
    v_1 *retval = new (std::nothrow) v_1;
    if (retval == NULL)
        return NULL;
    const v_2 *arg_0 = (const v_2 *)argv[0]; // part_program
    const v_1 *arg_1 = (const v_1 *)argv[1]; // program

    retval->tplist = arg_0->tplist + arg_1->tplist;

    return retval;
}

// function for program ::= part_program
const void *f_2(const void **argv)
{
    v_1 *retval = new (std::nothrow) v_1;
    if (retval == NULL)
        return NULL;
    const v_2 *arg_0 = (const v_2 *)argv[0]; // part_program

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for part_program ::= var_decl
const void *f_3(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_3 *arg_0 = (const v_3 *)argv[0]; // var_decl

    return retval;
}

// function for part_program ::= var_def
const void *f_4(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_4 *arg_0 = (const v_4 *)argv[0]; // var_def

    return retval;
}

// function for part_program ::= func_decl
const void *f_5(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_5 *arg_0 = (const v_5 *)argv[0]; // func_decl

    return retval;
}

// function for part_program ::= func_def
const void *f_6(const void **argv)
{
    v_2 *retval = new (std::nothrow) v_2;
    if (retval == NULL)
        return NULL;
    const v_6 *arg_0 = (const v_6 *)argv[0]; // func_def

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for var_decl ::= 'extern' identifier identifier ';'
const void *f_7(const void **argv)
{
    v_3 *retval = new (std::nothrow) v_3;
    if (retval == NULL)
        return NULL;
    const t_0 *arg_0 = (const t_0 *)argv[0];   // 'extern'
    const t_16 *arg_1 = (const t_16 *)argv[1]; // identifier
    const t_16 *arg_2 = (const t_16 *)argv[2]; // identifier
    const t_1 *arg_3 = (const t_1 *)argv[3];   // ';'

    add_name(arg_2->name);

    return retval;
}

// function for var_def ::= identifier identifier ';'
const void *f_8(const void **argv)
{
    v_4 *retval = new (std::nothrow) v_4;
    if (retval == NULL)
        return NULL;
    const t_16 *arg_0 = (const t_16 *)argv[0]; // identifier
    const t_16 *arg_1 = (const t_16 *)argv[1]; // identifier
    const t_1 *arg_2 = (const t_1 *)argv[2];   // ';'

    add_name(arg_1->name);

    return retval;
}

// function for func_decl ::= identifier identifier '(' param_list ')' ';'
const void *f_9(const void **argv)
{
    v_5 *retval = new (std::nothrow) v_5;
    if (retval == NULL)
        return NULL;
    const t_16 *arg_0 = (const t_16 *)argv[0]; // identifier
    const t_16 *arg_1 = (const t_16 *)argv[1]; // identifier
    const t_2 *arg_2 = (const t_2 *)argv[2];   // '('
    const v_7 *arg_3 = (const v_7 *)argv[3];   // param_list
    const t_3 *arg_4 = (const t_3 *)argv[4];   // ')'
    const t_1 *arg_5 = (const t_1 *)argv[5];   // ';'

    add_name(arg_1->name);

    return retval;
}

// function for func_def ::= identifier identifier '(' param_list ')' block
const void *f_10(const void **argv)
{
    v_6 *retval = new (std::nothrow) v_6;
    if (retval == NULL)
        return NULL;
    const t_16 *arg_0 = (const t_16 *)argv[0]; // identifier
    const t_16 *arg_1 = (const t_16 *)argv[1]; // identifier
    const t_2 *arg_2 = (const t_2 *)argv[2];   // '('
    const v_7 *arg_3 = (const v_7 *)argv[3];   // param_list
    const t_3 *arg_4 = (const t_3 *)argv[4];   // ')'
    const v_8 *arg_5 = (const v_8 *)argv[5];   // block

    add_name(arg_1->name);
    retval->tplist = arg_5->tplist;

    return retval;
}

// function for param_list ::= identifier identifier Temp143
const void *f_11(const void **argv)
{
    v_7 *retval = new (std::nothrow) v_7;
    if (retval == NULL)
        return NULL;
    const t_16 *arg_0 = (const t_16 *)argv[0]; // identifier
    const t_16 *arg_1 = (const t_16 *)argv[1]; // identifier
    const v_22 *arg_2 = (const v_22 *)argv[2]; // Temp143

    add_name(arg_1->name);

    return retval;
}

// function for param_list ::= #E
const void *f_12(const void **argv)
{
    v_7 *retval = new (std::nothrow) v_7;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for block ::= '{' Temp146 '}'
const void *f_13(const void **argv)
{
    v_8 *retval = new (std::nothrow) v_8;
    if (retval == NULL)
        return NULL;
    const t_5 *arg_0 = (const t_5 *)argv[0];   // '{'
    const v_23 *arg_1 = (const v_23 *)argv[1]; // Temp146
    const t_6 *arg_2 = (const t_6 *)argv[2];   // '}'

    retval->tplist = arg_1->tplist;

    return retval;
}

// function for statement ::= var_decl
const void *f_14(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_3 *arg_0 = (const v_3 *)argv[0]; // var_decl

    // add rules here

    return retval;
}

// function for statement ::= func_decl
const void *f_15(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_5 *arg_0 = (const v_5 *)argv[0]; // func_decl

    // add rules here

    return retval;
}

// function for statement ::= var_def
const void *f_16(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_4 *arg_0 = (const v_4 *)argv[0]; // var_def

    // add rules here

    return retval;
}

// function for statement ::= if_statement
const void *f_17(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_10 *arg_0 = (const v_10 *)argv[0]; // if_statement

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for statement ::= while_statement
const void *f_18(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_11 *arg_0 = (const v_11 *)argv[0]; // while_statement

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for statement ::= return_statement
const void *f_19(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_12 *arg_0 = (const v_12 *)argv[0]; // return_statement

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for statement ::= expression ';'
const void *f_20(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_13 *arg_0 = (const v_13 *)argv[0]; // expression
    const t_1 *arg_1 = (const t_1 *)argv[1];   // ';'

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for statement ::= block
const void *f_21(const void **argv)
{
    v_9 *retval = new (std::nothrow) v_9;
    if (retval == NULL)
        return NULL;
    const v_8 *arg_0 = (const v_8 *)argv[0]; // block

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for if_statement ::= 'if' '(' expression ')' statement Temp162
const void *f_22(const void **argv)
{
    v_10 *retval = new (std::nothrow) v_10;
    if (retval == NULL)
        return NULL;
    const t_7 *arg_0 = (const t_7 *)argv[0];   // 'if'
    const t_2 *arg_1 = (const t_2 *)argv[1];   // '('
    const v_13 *arg_2 = (const v_13 *)argv[2]; // expression
    const t_3 *arg_3 = (const t_3 *)argv[3];   // ')'
    const v_9 *arg_4 = (const v_9 *)argv[4];   // statement
    const v_24 *arg_5 = (const v_24 *)argv[5]; // Temp162

    retval->tplist += arg_2->tplist;
    retval->tplist.append({op_t::JG, arg_2->tplist.top().res, 0, 2, false, true, true});
    retval->tplist.append({op_t::J, DEF, DEF,
                           arg_4->tplist.size() + 1 + (arg_5->tplist.empty() ? 0 : 1),
                           false, false, true});
    retval->tplist += arg_4->tplist;
    if (arg_5->tplist.empty())
        retval->tplist.append({op_t::NOP, DEF, DEF, DEF});
    else
    {
        retval->tplist.append({op_t::J, DEF, DEF, arg_5->tplist.size() + 1, false, false, true});
        retval->tplist += arg_5->tplist;
        retval->tplist.append({op_t::NOP, DEF, DEF, DEF});
    }

    return retval;
}

// function for while_statement ::= 'while' '(' expression ')' statement
const void *f_23(const void **argv)
{
    v_11 *retval = new (std::nothrow) v_11;
    if (retval == NULL)
        return NULL;
    const t_9 *arg_0 = (const t_9 *)argv[0];   // 'while'
    const t_2 *arg_1 = (const t_2 *)argv[1];   // '('
    const v_13 *arg_2 = (const v_13 *)argv[2]; // expression
    const t_3 *arg_3 = (const t_3 *)argv[3];   // ')'
    const v_9 *arg_4 = (const v_9 *)argv[4];   // statement

    retval->tplist += arg_2->tplist;
    retval->tplist.append({op_t::JG, arg_2->tplist.top().res, 0, 2, false, true, true});
    retval->tplist.append({op_t::J, DEF, DEF, arg_4->tplist.size() + 2, false, false, true});
    retval->tplist += arg_4->tplist;
    retval->tplist.append({op_t::J, DEF, DEF, -2 - arg_2->tplist.size() - arg_4->tplist.size(),
                           false, false, true});
    retval->tplist.append({op_t::NOP, DEF, DEF, DEF});

    return retval;
}

// function for return_statement ::= 'return' expression ';'
const void *f_24(const void **argv)
{
    v_12 *retval = new (std::nothrow) v_12;
    if (retval == NULL)
        return NULL;
    const t_10 *arg_0 = (const t_10 *)argv[0]; // 'return'
    const v_13 *arg_1 = (const v_13 *)argv[1]; // expression
    const t_1 *arg_2 = (const t_1 *)argv[2];   // ';'

    retval->tplist += arg_1->tplist;
    retval->tplist.append({op_t::RET, arg_1->tplist.top().res, DEF, DEF});

    return retval;
}

// function for expression ::= bool_expression
const void *f_25(const void **argv)
{
    v_13 *retval = new (std::nothrow) v_13;
    if (retval == NULL)
        return NULL;
    const v_14 *arg_0 = (const v_14 *)argv[0]; // bool_expression

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for expression ::= expression '=' bool_expression
const void *f_26(const void **argv)
{
    v_13 *retval = new (std::nothrow) v_13;
    if (retval == NULL)
        return NULL;
    const v_13 *arg_0 = (const v_13 *)argv[0]; // expression
    const t_11 *arg_1 = (const t_11 *)argv[1]; // '='
    const v_14 *arg_2 = (const v_14 *)argv[2]; // bool_expression

    retval->tplist += arg_2->tplist;
    retval->tplist += arg_0->tplist;
    retval->tplist.append({op_t::ASGN, arg_2->tplist.top().res, DEF, arg_0->tplist.top().res});

    return retval;
}

// function for bool_expression ::= secondary_expression
const void *f_27(const void **argv)
{
    v_14 *retval = new (std::nothrow) v_14;
    if (retval == NULL)
        return NULL;
    const v_15 *arg_0 = (const v_15 *)argv[0]; // secondary_expression

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for bool_expression ::= bool_expression RelOp secondary_expression
const void *f_28(const void **argv)
{
    v_14 *retval = new (std::nothrow) v_14;
    if (retval == NULL)
        return NULL;
    const v_14 *arg_0 = (const v_14 *)argv[0]; // bool_expression
    const t_19 *arg_1 = (const t_19 *)argv[1]; // RelOp
    const v_15 *arg_2 = (const v_15 *)argv[2]; // secondary_expression

    retval->tplist += arg_0->tplist;
    retval->tplist += arg_2->tplist;
    switch (arg_1->type)
    {
    case type_t::LT:
        retval->tplist.append(
            {op_t::LT, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
        break;
    case type_t::GT:
        retval->tplist.append(
            {op_t::GT, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
        break;
    case type_t::LE:
        retval->tplist.append(
            {op_t::LE, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
        break;
    case type_t::GE:
        retval->tplist.append(
            {op_t::GE, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
        break;
    case type_t::EQ:
        retval->tplist.append(
            {op_t::EQ, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
        break;
    }
    names.append(list<wchar_t>());

    return retval;
}

// function for secondary_expression ::= term
const void *f_29(const void **argv)
{
    v_15 *retval = new (std::nothrow) v_15;
    if (retval == NULL)
        return NULL;
    const v_16 *arg_0 = (const v_16 *)argv[0]; // term

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for secondary_expression ::= secondary_expression '+' term
const void *f_30(const void **argv)
{
    v_15 *retval = new (std::nothrow) v_15;
    if (retval == NULL)
        return NULL;
    const v_15 *arg_0 = (const v_15 *)argv[0]; // secondary_expression
    const t_12 *arg_1 = (const t_12 *)argv[1]; // '+'
    const v_16 *arg_2 = (const v_16 *)argv[2]; // term

    retval->tplist += arg_0->tplist;
    retval->tplist += arg_2->tplist;
    retval->tplist.append(
        {op_t::ADD, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
    names.append(list<wchar_t>());

    return retval;
}

// function for secondary_expression ::= secondary_expression '-' term
const void *f_31(const void **argv)
{
    v_15 *retval = new (std::nothrow) v_15;
    if (retval == NULL)
        return NULL;
    const v_15 *arg_0 = (const v_15 *)argv[0]; // secondary_expression
    const t_13 *arg_1 = (const t_13 *)argv[1]; // '-'
    const v_16 *arg_2 = (const v_16 *)argv[2]; // term

    retval->tplist += arg_0->tplist;
    retval->tplist += arg_2->tplist;
    retval->tplist.append(
        {op_t::SUB, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
    names.append(list<wchar_t>());

    return retval;
}

// function for term ::= factor
const void *f_32(const void **argv)
{
    v_16 *retval = new (std::nothrow) v_16;
    if (retval == NULL)
        return NULL;
    const v_17 *arg_0 = (const v_17 *)argv[0]; // factor

    retval->tplist = arg_0->tplist;

    return retval;
}

// function for term ::= term '*' factor
const void *f_33(const void **argv)
{
    v_16 *retval = new (std::nothrow) v_16;
    if (retval == NULL)
        return NULL;
    const v_16 *arg_0 = (const v_16 *)argv[0]; // term
    const t_14 *arg_1 = (const t_14 *)argv[1]; // '*'
    const v_17 *arg_2 = (const v_17 *)argv[2]; // factor

    retval->tplist += arg_0->tplist;
    retval->tplist += arg_2->tplist;
    retval->tplist.append(
        {op_t::MUL, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
    names.append(list<wchar_t>());

    return retval;
}

// function for term ::= term '/' factor
const void *f_34(const void **argv)
{
    v_16 *retval = new (std::nothrow) v_16;
    if (retval == NULL)
        return NULL;
    const v_16 *arg_0 = (const v_16 *)argv[0]; // term
    const t_15 *arg_1 = (const t_15 *)argv[1]; // '/'
    const v_17 *arg_2 = (const v_17 *)argv[2]; // factor

    retval->tplist += arg_0->tplist;
    retval->tplist += arg_2->tplist;
    retval->tplist.append(
        {op_t::DIV, arg_0->tplist.top().res, arg_2->tplist.top().res, names.size()});
    names.append(list<wchar_t>());

    return retval;
}

// function for factor ::= number
const void *f_35(const void **argv)
{
    v_17 *retval = new (std::nothrow) v_17;
    if (retval == NULL)
        return NULL;
    const v_18 *arg_0 = (const v_18 *)argv[0]; // number

    retval->tplist.append({op_t::ASGN, arg_0->val, DEF, names.size()});
    retval->tplist.top().imm1 = true;
    names.append(list<wchar_t>());

    return retval;
}

// function for factor ::= '(' expression ')'
const void *f_36(const void **argv)
{
    v_17 *retval = new (std::nothrow) v_17;
    if (retval == NULL)
        return NULL;
    const t_2 *arg_0 = (const t_2 *)argv[0];   // '('
    const v_13 *arg_1 = (const v_13 *)argv[1]; // expression
    const t_3 *arg_2 = (const t_3 *)argv[2];   // ')'

    retval->tplist += arg_1->tplist;

    return retval;
}

// function for factor ::= variable
const void *f_37(const void **argv)
{
    v_17 *retval = new (std::nothrow) v_17;
    if (retval == NULL)
        return NULL;
    const v_19 *arg_0 = (const v_19 *)argv[0]; // variable

    retval->tplist += arg_0->tplist;
    retval->tplist.append({op_t::ASGN, arg_0->addr, DEF, names.size()});
    names.append(list<wchar_t>());

    return retval;
}

// function for number ::= integer
const void *f_38(const void **argv)
{
    v_18 *retval = new (std::nothrow) v_18;
    if (retval == NULL)
        return NULL;
    const t_17 *arg_0 = (const t_17 *)argv[0]; // integer

    retval->val = arg_0->val;
    retval->type = retval->INT;

    return retval;
}

// function for number ::= real
const void *f_39(const void **argv)
{
    v_18 *retval = new (std::nothrow) v_18;
    if (retval == NULL)
        return NULL;
    const t_18 *arg_0 = (const t_18 *)argv[0]; // real

    retval->val_d = arg_0->val;
    retval->type = retval->DOUBLE;

    return retval;
}

// function for variable ::= identifier
const void *f_40(const void **argv)
{
    v_19 *retval = new (std::nothrow) v_19;
    if (retval == NULL)
        return NULL;
    const t_16 *arg_0 = (const t_16 *)argv[0]; // identifier

    list<hash_string_info> &info = aidx[h(arg_0->name) % HASH_SZ];
    int i;
    for (i = 0; i < info.size(); i++)
        if (info[i].str == arg_0->name)
            break;
    if (i == info.size())
    {
        printf("Identifier ");
        for (int j = 0; j < arg_0->name.size(); j++)
            printf("%c", ((list<wchar_t>)arg_0->name)[j]);
        printf(" does not exsit!\n");
    }
    else
        retval->addr = info[i].idx;

    return retval;
}

// function for variable ::= func_call
const void *f_41(const void **argv)
{
    v_19 *retval = new (std::nothrow) v_19;
    if (retval == NULL)
        return NULL;
    const v_20 *arg_0 = (const v_20 *)argv[0]; // func_call

    retval->tplist = arg_0->tplist;
    retval->addr = arg_0->tplist.top().res;

    return retval;
}

// function for func_call ::= identifier '(' arg_list ')'
const void *f_42(const void **argv)
{
    v_20 *retval = new (std::nothrow) v_20;
    if (retval == NULL)
        return NULL;
    const t_16 *arg_0 = (const t_16 *)argv[0]; // identifier
    const t_2 *arg_1 = (const t_2 *)argv[1];   // '('
    const v_21 *arg_2 = (const v_21 *)argv[2]; // arg_list
    const t_3 *arg_3 = (const t_3 *)argv[3];   // ')'

    int i;
    list<hash_string_info> &info = aidx[h(arg_0->name) % HASH_SZ];
    for (i = 0; i < info.size(); i++)
        if (info[i].str == arg_0->name)
            break;
    if (i == info.size())
    {
        printf("Identifier ");
        for (int j = 0; j < arg_0->name.size(); j++)
            printf("%c", ((list<wchar_t>)arg_0->name)[j]);
        printf(" does not exsit!\n");
    }
    else
    {
        retval->tplist = arg_2->tplist;
        retval->tplist.append({op_t::CALL, info[i].idx, DEF, names.size()});
        names.append(list<wchar_t>());
    }

    return retval;
}

// function for arg_list ::= expression Temp198
const void *f_43(const void **argv)
{
    v_21 *retval = new (std::nothrow) v_21;
    if (retval == NULL)
        return NULL;
    const v_13 *arg_0 = (const v_13 *)argv[0]; // expression
    const v_25 *arg_1 = (const v_25 *)argv[1]; // Temp198

    retval->tplist += arg_0->tplist;
    retval->tplist.append({op_t::ARG, arg_0->tplist.top().res, DEF, DEF});
    retval->tplist += arg_1->tplist;

    return retval;
}

// function for arg_list ::= #E
const void *f_44(const void **argv)
{
    v_21 *retval = new (std::nothrow) v_21;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for Temp143 ::= ',' identifier identifier Temp143
const void *f_45(const void **argv)
{
    v_22 *retval = new (std::nothrow) v_22;
    if (retval == NULL)
        return NULL;
    const t_4 *arg_0 = (const t_4 *)argv[0];   // ','
    const t_16 *arg_1 = (const t_16 *)argv[1]; // identifier
    const t_16 *arg_2 = (const t_16 *)argv[2]; // identifier
    const v_22 *arg_3 = (const v_22 *)argv[3]; // Temp143

    // add rules here

    return retval;
}

// function for Temp143 ::= #E
const void *f_46(const void **argv)
{
    v_22 *retval = new (std::nothrow) v_22;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for Temp146 ::= statement Temp146
const void *f_47(const void **argv)
{
    v_23 *retval = new (std::nothrow) v_23;
    if (retval == NULL)
        return NULL;
    const v_9 *arg_0 = (const v_9 *)argv[0];   // statement
    const v_23 *arg_1 = (const v_23 *)argv[1]; // Temp146

    retval->tplist = arg_0->tplist + arg_1->tplist;

    return retval;
}

// function for Temp146 ::= #E
const void *f_48(const void **argv)
{
    v_23 *retval = new (std::nothrow) v_23;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for Temp162 ::= 'else' statement
const void *f_49(const void **argv)
{
    v_24 *retval = new (std::nothrow) v_24;
    if (retval == NULL)
        return NULL;
    const t_8 *arg_0 = (const t_8 *)argv[0]; // 'else'
    const v_9 *arg_1 = (const v_9 *)argv[1]; // statement

    retval->tplist += arg_1->tplist;

    return retval;
}

// function for Temp162 ::= #E
const void *f_50(const void **argv)
{
    v_24 *retval = new (std::nothrow) v_24;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for Temp198 ::= ',' expression Temp198
const void *f_51(const void **argv)
{
    v_25 *retval = new (std::nothrow) v_25;
    if (retval == NULL)
        return NULL;
    const t_4 *arg_0 = (const t_4 *)argv[0];   // ','
    const v_13 *arg_1 = (const v_13 *)argv[1]; // expression
    const v_25 *arg_2 = (const v_25 *)argv[2]; // Temp198

    retval->tplist += arg_1->tplist;
    retval->tplist.append({op_t::ARG, arg_1->tplist.top().res, DEF, DEF});
    retval->tplist += arg_2->tplist;

    return retval;
}

// function for Temp198 ::= #E
const void *f_52(const void **argv)
{
    v_25 *retval = new (std::nothrow) v_25;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for 'extern'
const void *ft_0(const token_info &tk)
{
    t_0 *retval = new (std::nothrow) t_0;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for ';'
const void *ft_1(const token_info &tk)
{
    t_1 *retval = new (std::nothrow) t_1;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '('
const void *ft_2(const token_info &tk)
{
    t_2 *retval = new (std::nothrow) t_2;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for ')'
const void *ft_3(const token_info &tk)
{
    t_3 *retval = new (std::nothrow) t_3;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for ','
const void *ft_4(const token_info &tk)
{
    t_4 *retval = new (std::nothrow) t_4;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '{'
const void *ft_5(const token_info &tk)
{
    t_5 *retval = new (std::nothrow) t_5;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '}'
const void *ft_6(const token_info &tk)
{
    t_6 *retval = new (std::nothrow) t_6;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for 'if'
const void *ft_7(const token_info &tk)
{
    t_7 *retval = new (std::nothrow) t_7;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for 'else'
const void *ft_8(const token_info &tk)
{
    t_8 *retval = new (std::nothrow) t_8;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for 'while'
const void *ft_9(const token_info &tk)
{
    t_9 *retval = new (std::nothrow) t_9;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for 'return'
const void *ft_10(const token_info &tk)
{
    t_10 *retval = new (std::nothrow) t_10;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '='
const void *ft_11(const token_info &tk)
{
    t_11 *retval = new (std::nothrow) t_11;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '+'
const void *ft_12(const token_info &tk)
{
    t_12 *retval = new (std::nothrow) t_12;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '-'
const void *ft_13(const token_info &tk)
{
    t_13 *retval = new (std::nothrow) t_13;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '*'
const void *ft_14(const token_info &tk)
{
    t_14 *retval = new (std::nothrow) t_14;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for '/'
const void *ft_15(const token_info &tk)
{
    t_15 *retval = new (std::nothrow) t_15;
    if (retval == NULL)
        return NULL;

    // add rules here

    return retval;
}

// function for identifier
const void *ft_16(const token_info &tk)
{
    t_16 *retval = new (std::nothrow) t_16;
    if (retval == NULL)
        return NULL;

    int i = 0;
    while (tk.lexeme[i] != L'\0')
        retval->name.append(tk.lexeme[i++]);

    return retval;
}

// function for integer
const void *ft_17(const token_info &tk)
{
    t_17 *retval = new (std::nothrow) t_17;
    if (retval == NULL)
        return NULL;

    retval->val = 0;
    int i = 0;
    while (tk.lexeme[i] != L'\0')
    {
        retval->val *= 10;
        retval->val += tk.lexeme[i] - L'0';
        i++;
    }

    return retval;
}

// function for real
const void *ft_18(const token_info &tk)
{
    t_18 *retval = new (std::nothrow) t_18;
    if (retval == NULL)
        return NULL;

    retval->val = 3.14;

    return retval;
}

// function for RelOp
const void *ft_19(const token_info &tk)
{
    t_19 *retval = new (std::nothrow) t_19;
    if (retval == NULL)
        return NULL;

    if (tk.lexeme[0] == L'<' && tk.lexeme[1] == L'\0')
        retval->type = type_t::LT;
    else if (tk.lexeme[0] == L'>' && tk.lexeme[1] == L'\0')
        retval->type = type_t::GT;
    else if (tk.lexeme[0] == L'<' && tk.lexeme[1] == L'=')
        retval->type = type_t::LE;
    else if (tk.lexeme[0] == L'>' && tk.lexeme[1] == L'=')
        retval->type = type_t::GE;
    else if (tk.lexeme[0] == L'=' && tk.lexeme[1] == L'=')
        retval->type = type_t::EQ;

    return retval;
}

const void *(*ft_list[])(const token_info &) = {
    ft_0, ft_1, ft_2, ft_3, ft_4, ft_5, ft_6, ft_7, ft_8, ft_9, ft_10, ft_11, ft_12, ft_13, ft_14, ft_15,
    ft_16, ft_17, ft_18, ft_19};
const void *(*f_list[])(const void **) = {
    f_0, f_1, f_2, f_3, f_4, f_5, f_6, f_7, f_8, f_9, f_10, f_11, f_12, f_13, f_14, f_15,
    f_16, f_17, f_18, f_19, f_20, f_21, f_22, f_23, f_24, f_25, f_26, f_27, f_28, f_29, f_30, f_31,
    f_32, f_33, f_34, f_35, f_36, f_37, f_38, f_39, f_40, f_41, f_42, f_43, f_44, f_45, f_46, f_47,
    f_48, f_49, f_50, f_51, f_52};
