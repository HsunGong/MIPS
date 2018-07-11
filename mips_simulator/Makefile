#ATTENTION! all make orders is under LINUX SYSTEM


CC = g++
CFLAGS = -Wall -std=c++17 -O


######################    Modify    ######################
NAME = code

HPP = Header.hpp storage.hpp instruction.hpp #.hpp
CPP = main.cpp Header.cpp storage.cpp instruction.cpp # *.cpp
OBJ = main.o Header.o storage.o instruction.o

##########################################################

#link all .o files into one executable file named NAME
#g++ grammer: g++ file1.o file2.o -o pro_name.exe
$(NAME) : $(OBJ) 
	$(CC) -o $(NAME) $(CPP) $(CFLAGS)

#complie all cpp into .o files
#g++ grammer: g++ -c file.cpp
$(OBJ) : $(HPP)
	$(CC) -c $(CPP) $(CPP) $(CFLAGS)

#normal order
#g++ grammer: g++ file.cpp -o pro_name.exe

#clean order
.PHONY : clean
clean:
#windows
	# del -f $(NAME)
	# del -f $(OBJ)
#bash
	rm -f $(NAME)
	rm -f $(OBJ)



#link library...