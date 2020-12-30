# # 046267 Computer Architecture - Winter 20/21 - HW #2


# Makefile for the smash program
CC = g++
# CFLAGS = -g -Wall
CFLAGS =-std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread -g
CCLINK = $(CC)
OBJS = Bank.o BankData.o
RM = rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CCLINK) $(CFLAGS) -o Bank $(OBJS)
# Creating the object files
BankData.o: BankData.cpp BankData.hpp
	$(CCLINK) $(CFLAGS)  -c BankData.cpp
Bank.o: Bank.cpp BankData.hpp
	$(CCLINK) $(CFLAGS)  -c Bank.cpp
# Cleaning old files before new make
clean:
	$(RM) Bank *.o *~ "#"* core.* log.txt
