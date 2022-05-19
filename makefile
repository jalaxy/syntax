EBNFFILE = ebnf.txt
INPUT = test
OUTPUT = test
CXXFLAGS = -g -I include
CC = g++
SRCOBJS = src/grammar.o src/expr.o src/fa.o src/lalr.o src/lexical.o

# pregen: generate syntax analyser
# analyse: use "pregen" to generate template
# sdt: generate translator from template with modification
# translate: use "sdt" to translate a file from .c to .s
# compile: assemble and link assembly files with startup
# clean: clean all temporary files

.PHONY : pregen analyse sdt translate compile clean

pregen : bin/pregen

analyse : bin/pregen
	cd bin; ./pregen $(EBNFFILE)

sdt : bin/sdt

translate : sdt
	bin/sdt $(INPUT).c $(OUTPUT).s

compile : translate
	$(AS) start.s $(OUTPUT).s -o $(OUTPUT).o --32
	$(LD) $(OUTPUT).o -o $(OUTPUT) -m elf_i386

bin/pregen : src/pregen
	rm -f $@
	cp $< $@

src/pregen : src/pregen.o $(SRCOBJS)

bin/sdt : bin/semantics.o bin/sdt.o $(SRCOBJS)

clean :
	$(RM) src/*.o src/pregen bin/*.o bin/sdt bin/pregen $(OUTPUT).o $(OUTPUT) $(OUTPUT).s
