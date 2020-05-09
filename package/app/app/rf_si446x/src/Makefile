################################################################################
# File: Makefile
# Description: 
# Author:
#
# Fix hisoty:
#
################################################################################

# 当前路径
PWD := $(shell pwd)

DEBUG := 1
ifdef DEBUG
CFLAGS += -g
else
CFLAGS += -O2
endif

CC := $(CC)

BIN := rf_si446x

SRC :=$(wildcard ./src/*.c)

#.h源文件
HEAD_SRC=$(wildcard ./inc/*.h)
#HEAD_SRC+=$(wildcard ./si446x/*.h)
#.o中间文件
OBJ=$(SRC:%.c=%.o)

#参数
CFLAGS += -I./inc
LDFLAGS += -L./src

#库
LIB +=

.PHONY : clean all

all: ${BIN}

#编译生成二进制文件	
${BIN}: ${OBJ} ${HEAD_SRC}
	${CC} -o $@ ${OBJ} ${CFLAGS}

#编译生成.o中间文件
%.o: %.c %.h
	${CC} -c ${CFLAG} $< -o $@ 

clean:	
	rm -f ${BIN} ${OBJ} *.o

