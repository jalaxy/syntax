#include <iostream>
#include <iomanip>
#include "grammar.h"
void print(parsing_tree &, list<list<wchar_t>> &);
void print(list<int> l);
void print(list<unsigned int> l, bool ch = false);
void print(list<unsigned int> *l, int n);
void print(expr e);
void print(fa &nfa);
void print(dfa_table table);
void print(list<expr> l);
void print(grammar &g);
void print(ll1_parsing_table table);
void print(list<list<wchar_t>> names);
void print(list<hash_symbol_info> l);
