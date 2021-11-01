/**
 * @file gramma.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Data structures and functions to process gramma
 * @version 0.1
 * @date 2021-10-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

class ebnf
{
public:
    unsigned int varible;
    unsigned int *expression;
    ebnf();
    ebnf(const ebnf &);
    ~ebnf();
};

// Extended Backus-Naur Form (EBNF) notation, from XML specification
// Reference link: https://www.w3.org/TR/xml/

// Each rule in the grammar defines
// one symbol, in the form
//     symbol ::= expression
// Symbols are written with an initial capital letter if they are the start
// symbol of a regular language, otherwise with an initial lowercase letter.
// Literal strings are quoted.

// Within the expression on the right-hand side of a rule, the following expressions
// are used to match strings of one or more characters:
//     #xN
// where N is a hexadecimal integer, the expression matches the character whose
// number (code point) in ISO/IEC 10646 is N. The number of leading zeros in the
// #xN form is insignificant.
//     [a-zA-Z], [#xN-#xN]
// matches any Char with a value in the range(s) indicated (inclusive).
//     [abc], [#xN#xN#xN]
// matches any Char with a value among the characters enumerated. Enumerations and
// ranges can be mixed in one set of brackets.
//     [^a-z], [^#xN-#xN]
// matches any Char with a value outside the range indicated.
//     [^abc], [^#xN#xN#xN]
// matches any Char with a value not among the characters given. Enumerations and
// ranges of forbidden values can be mixed in one set of brackets.
//     "string"
// matches a literal string matching that given inside the double quotes.
//     'string'
// matches a literal string matching that given inside the single quotes.

// These symbols may be combined to match more complex patterns as follows, where
// A and B represent simple expressions:
//     (expression)
// expression is treated as a unit and may be combined as described in this list.
//     A?
// matches A or nothing; optional A.
//     A B
// matches A followed by B. This operator has higher precedence than alternation;
// thus A B | C D is identical to (A B) | (C D).
//     A | B
// matches A or B.
//     A - B
// matches any string that matches A but does not match B.
//     A+
// matches one or more occurrences of A. Concatenation has higher precedence than
// alternation; thus A+ | B+ is identical to (A+) | (B+).
//     A*
// matches zero or more occurrences of A. Concatenation has higher precedence than
// alternation; thus A* | B* is identical to (A*) | (B*).

// Other notations used in the productions are:
//     /* ... */
// comment.
