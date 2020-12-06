# Makefile for the smash program
CC = g++
# CFLAGS = -g -Wall
CFLAGS =-std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -g
CCLINK = $(CC)
OBJS = bank.o
RM = rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CCLINK) -o Bank $(OBJS)
# Creating the object files
bank.o: bank.cpp
# commands.o: commands.cpp commands.h data.hpp
# smash.o: smash.cpp commands.h data.hpp
# signals.o: signals.cpp signals.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

