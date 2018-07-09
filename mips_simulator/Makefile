CC = g++
CFLAGS = -Wall -std=c++17 -O


######################    Modify    ######################
NAME = code

H_PP = Header.h storage.hpp instruction.hpp
CPP = Source.cpp
OBJ = Header.h.gch storage.hpp.gch instruction.hpp.gch
##########################################################

$(NAME):
	$(CC) -o $(NAME) $(CPP) $(CFLAGS)

.PHONY : clean
clean:
#windows
	# del -f $(NAME)
	# del -f $(OBJ)
#bash
	rm -f $(NAME)
	rm -f $(OBJ)
