EBNFFILE = ebnf.txt
INPUT = test.c
OUTPUT = output.s
CXXFLAGS = -g -I include
CC = g++
SRCOBJS = src/grammar.o src/expr.o src/fa.o src/lalr.o src/lexical.o

.PHONY : analyse clean compiler translate compile

analyse : bin/pregen
	cd bin; ./pregen $(EBNFFILE)

compiler : bin/sdt

translate : compiler
	bin/sdt $(INPUT) $(OUTPUT)

compile : translate
	cc start.s $(OUTPUT) -o $(basename $(INPUT)) -nostdlib -m32
	$(RM) $(OUTPUT)

bin/pregen : src/pregen
	rm -f $@
	cp $< $@

src/pregen : src/pregen.o $(SRCOBJS)

bin/sdt : bin/semantics.o bin/sdt.o $(SRCOBJS)

clean :
	$(RM) src/*.o src/pregen bin/*.o
