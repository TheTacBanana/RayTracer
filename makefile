#OBJS
OBJS = main.cpp glad.c

#Compiler
CC = g++

#COMPILER_FLAGS
# -w suppresses all warnings
COMPILER_FLAGS = 

#LINKER_FLAGS
LINKER_FLAGS = -ldl -lglfw 

#OBJ_NAME 
OBJ_NAME = out.exe

#This is the target that compiles our executable
default : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

run:
	./$(OBJ_NAME)

clear:
	rm ./$(OBJ_NAME)

all:
	make
	make run