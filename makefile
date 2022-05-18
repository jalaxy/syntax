EBNFFILE = ebnf.txt
CXXFLAGS = -g -I include
CC = g++
SRCOBJS = src/grammar.o src/expr.o src/fa.o src/lalr.o src/lexical.o

.PHONY : analyse clean

analyse : bin/pregen
	cd bin; ./pregen $(EBNFFILE)

compile : bin/sdt

bin/pregen : src/pregen
	rm -f $@
	cp $< $@

src/pregen : src/pregen.o $(SRCOBJS)

bin/sdt : bin/semantics.o bin/sdt.o $(SRCOBJS)

clean :
	$(RM) src/*.o src/pregen bin/*.o
