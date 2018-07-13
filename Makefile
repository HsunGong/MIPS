#ATTENTION! all make orders is under LINUX SYSTEM

# include xxxxxx # include 其他makefile文件

CC = g++
CFLAGS = -Wall -std=c++17 -O

# = 是最基本的赋值
# := 是覆盖之前的值
######################    Modify    ######################
NAME = code

_HPP = head.hpp store.hpp instruction.hpp exception.hpp #.hpp
_CPP = main.cpp head.cpp store.cpp instruction.cpp # *.cpp
_OBJ = main.o head.o store.o instruction.o
# g++ -c $< -l folder
# vpath  %.cpp mips_simulator
VPATH = mips_simulator:obj

##########################################################
# .PHONY # 编译多个可执行文件时的伪目标
# all : ....


# patsubst : $(patsubst %.c, %.o, $(DIR)), repalce all .c to .o in DIR
# way2 : obj = $(DIR : %.c = %.o)
# wildcard : SRC = $(wildcard *.c) $(wildcard inc/*.c)  指定编译目录下所有文件
# @echo : display orders
OBJPATH = obj
OBJ = $(patsubst %,$(OBJPATH)/%,$(_OBJ))
IPATH = mips_simulator
HPP = $(patsubst %,$(IPATH)/%,$(_HPP))
CPP = $(patsubst %,$(IPATH)/%,$(_CPP))
# exist = $(shell if [ -f $(FILE) ]; then echo "exist"; else echo "notexist"; fi;)

#link all .o files into one executable file named NAME
#g++ grammer: g++ file1.o file2.o -o pro_name.exe
$(NAME) : $(OBJ) 
	$(CC) -o $(NAME) $(CPP) $(CFLAGS)

#complie all cpp into .o files
#g++ grammer: g++ -c file.cpp
$(OBJPATH)/%.o : $(HPP) $(CPP)
	$(shell mkdir $(OBJPATH))
	$(CC) -c -o $@ $< $(CFLAGS) 
# @echo $(HPP)
	
# $(CC) $(CFLAGS) -c $< -o $@

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
	rm -rf obj


#link library...




# all g++ orders
# g++ -e .cpp > .i # 预处理文件，宏定义的替换，无关代码消除
# g++ -s .cpp # 生成汇编语言
# g++ -c .cpp # 生成。o目标文件
# g++ .o -o xx.exe -l ##*folder## # 生成可执行文件
# g++ -o main main.cpp ##-I /usr/local/include/python/##
# -I 在这个目录下寻找；-l 在这个目录下生成

# （3）gcc -I -L -l的区别：
# gcc -o hello hello.c -I /home/hello/include -L /home/hello/lib -lworld
# 上面这句表示在编译hello.c时-I /home/hello/include表示将/home/hello/include目录作为第一个寻找头文件的目录，
# 寻找的顺序是：/home/hello/include-->/usr/include-->/usr/local/include
# L /home/hello/lib表示将/home/hello/lib目录作为第一个寻找库文件的目录，
# 寻找的顺序是：/home/hello/lib-->/lib-->/usr/lib-->/usr/local/lib
# -lworld表示在上面的lib的路径中寻找libworld.so动态库文件（如果gcc编译选项中加入了“-static”表示寻找libworld.a静态库文件）

# （1）Makefile中的 符号 $@, $^, $< 的意思：
# $@	目标集合
# $%	当目标是函数库文件时, 表示其中的目标文件名
# $<	第一个依赖目标. 如果依赖目标是多个, 逐个表示依赖目标
# $?	比目标新的依赖目标的集合
# $^	所有依赖目标的集合, 会去除重复的依赖目标
# $+	所有依赖目标的集合, 不会去除重复的依赖目标
# $*	这个是GNU make特有的, 其它的make不一定支持

# （2）wildcard、notdir、patsubst的意思：

# 　　wildcard : 扩展通配符
# 　　notdir ： 去除路径
# 　　patsubst ：替换通配符