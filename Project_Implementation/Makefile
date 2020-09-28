COMPILER=gcc
COMPILER_FLAG = -g
TARGET:linkedlist/linkedlist.o

OBJECT_FILES=linkedlist/linkedlist.o

linkedlist/linkedlist.o:linkedlist/linkedlist.c
	${COMPILER} ${COMPILER_FLAG} -c -I linkedlist linkedlist/linkedlist.c -o linkedlist/linkedlist.o

clean:
	rm linkedlist/linkedlist.o