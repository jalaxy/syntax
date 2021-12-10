/****************************************************************
 * @file expr.cpp
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Function definition of expr.h
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#include "expr.h"

/**
 * @brief Comparison between two expression object
 * 
 * @param b another object
 * @return whether lhs is less than b.lhs
 */
bool expr::operator<(const expr &b) const { return lhs < b.lhs; }

/**
 * @brief Get the minimum priority of the expression
 * 
 * @return minimum priority
 */
unsigned int expr::prior() const
{
    unsigned int prior = OP_KLCLS;
    for (int i = 0; i < rhs.size(); i++)
    {
        list<unsigned int> &RHS = (list<unsigned int> &)rhs;
        if (RHS[i] > OP_RPRTH && RHS[i] < prior)
            prior = ((list<unsigned int> &)rhs)[i];
        else if (RHS[i] == OP_LPRTH)
        {
            list<unsigned int> st;
            st.push_back(OP_LPRTH);
            while (!st.empty() && i + 1 < rhs.size())
            {
                i++;
                if (RHS[i] == OP_LPRTH)
                    st.push_back(OP_LPRTH);
                else if (RHS[i] == OP_RPRTH)
                    st.pop_back();
            }
        }
    }
    return prior;
}

/**
 * @brief Alternation operator
 * 
 * @param b another RE
 * @return A | B 
 */
expr expr::operator|(const expr &b) const
{
    list<unsigned int> RHS;
    if (prior() < OP_ALTER)
        RHS += OP_LPRTH;
    RHS += rhs;
    if (prior() < OP_ALTER)
        RHS += OP_RPRTH;
    RHS += OP_ALTER;
    if (b.prior() < OP_ALTER)
        RHS += OP_LPRTH;
    RHS += b.rhs;
    if (b.prior() < OP_ALTER)
        RHS += OP_RPRTH;
    return {NON_ACC, RHS};
}

/**
 * @brief Concatenation operator
 * 
 * @param b another RE
 * @return A B
 */
expr expr::operator<<(const expr &b) const
{
    list<unsigned int> RHS;
    if (prior() < OP_CNCAT)
        RHS += OP_LPRTH;
    RHS += rhs;
    if (prior() < OP_CNCAT)
        RHS += OP_RPRTH;
    RHS += OP_CNCAT;
    if (b.prior() < OP_CNCAT)
        RHS += OP_LPRTH;
    RHS += b.rhs;
    if (b.prior() < OP_CNCAT)
        RHS += OP_RPRTH;
    return {NON_ACC, RHS};
}

/**
 * @brief Kleene Closure operation
 * 
 * @return A*
 */
expr expr::operator*() const
{
    list<unsigned int> RHS;
    if (prior() < OP_KLCLS)
        RHS += OP_LPRTH;
    RHS += rhs;
    if (prior() < OP_KLCLS)
        RHS += OP_RPRTH;
    RHS += OP_KLCLS;
    return {NON_ACC, RHS};
}

/**
 * @brief Complementation
 * 
 * @return ~A
 */
expr expr::operator~() const
{
    list<unsigned int> RHS;
    RHS += OP_CMPLM;
    if (prior() < OP_CMPLM)
        RHS += OP_LPRTH;
    RHS += rhs;
    if (prior() < OP_CMPLM)
        RHS += OP_RPRTH;
    return {NON_ACC, RHS};
}

/**
 * @brief Get infomation of a symbol
 * 
 * @param symbol the symbol to query
 * @return infomation including symbol type and index, NULL if not exist
 */
const hash_symbol_info *QuerySymbol(const list<hash_symbol_info> *aidx, unsigned int symbol)
{
    for (int i = 0; i < aidx[symbol % HASH_SZ].size(); i++)
        if (((list<hash_symbol_info> *)aidx)[symbol % HASH_SZ][i].symbol == symbol)
            return &((list<hash_symbol_info> *)aidx)[symbol % HASH_SZ][i];
    return NULL;
}
