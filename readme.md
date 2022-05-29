# Syntax-directed translator

In this repository is a compiler-compiler based on syntax-directed translation. It is also a project of compiler principle course design in Tongji University.

The functionality of this is to generate templates and LR parsing tables of a given grammar described as EBNF. It can be compiled and implemented on any OS theoretically, and has been tested on Windows and Linux.

The process of building a compile or anything else to process language of context-free grammar is listed below:

1. Compile `pregen.cpp` with other source files in `src` directory and obtain executable syntax analyser.
2. Use command `/path/to/pregen ebnffile` to analyse a file of EBNF grammar description.
3. After step 2, three files `semantics.h`, `semantics.cpp` and `sdt.h` will be generated. They implement an S-attribute postfix translation.
4. In `semantics.h` there are classes of every grammar variables and terminals, attributes of each symbol should be added as member of related class. `semantics.cpp` contains callback functions for each production and terminals. the functions of productions are called when reductions happen during parsing, and functions for terminals are called before parsing, during lexical analysis. `sdt.cpp` includes entry point, LR parsing table. Input and output behavior should be specified here.
5. After rules are filled in three files above, they can be compiled with files in `src` directory excluding `pregen.cpp`, so that the executable file `sdt` is generated, and this is the parser related to the grammar and filled rules.

Additionally, there are some examples and experiments in directory `example`, grammars and documents in directory `ebnf` and `doc`.