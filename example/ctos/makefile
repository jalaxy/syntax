INPUT = test
OUTPUT = test
CXXFLAGS = -g -I ../../include
SRCOBJS = ../../src/grammar.o ../../src/expr.o ../../src/fa.o \
	../../src/lalr.o ../../src/lexical.o
CC = g++

.PHONY : compile clean

compile : translate start.s
	$(AS) start.s $(OUTPUT).s -o $(OUTPUT).o --32
	$(LD) $(OUTPUT).o -o $(OUTPUT) -m elf_i386

translate : sdt test.c
	./sdt $(INPUT).c $(OUTPUT).s

sdt : semantics.o sdt.o $(SRCOBJS)

clean :
	$(RM) $(OUTPUT).s *.o ../../src/*.o $(OUTPUT) ./sdt
