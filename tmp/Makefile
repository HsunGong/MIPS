CC = g++
CFLAGS = -Wall -std=c++11 -O2

src=$(wildcard *.c ./sub/*.c)
dir=$(notdir $(src))
obj=$(patsubst %.c,%.o,$(dir) )

all:
@echo $(src)
@echo $(dir)
@echo $(obj)
@echo "end"
