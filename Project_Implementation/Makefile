CC=gcc
CFLAGS=-g
TARGET:main.sh

OBJS=linkedlist/linkedlist.o \
		  networkgraph.o 		   \
		  network_topology.o

main.sh:main.o ${OBJS}
	${CC} ${CFLAGS} main.o ${OBJS} -o main.sh

main.o:main.c
	${CC} ${CFLAGS} -c main.c -o main.o

linkedlist/linkedlist.o:linkedlist/linkedlist.c
	${CC} ${CFLAGS} -c -I linkedlist linkedlist/linkedlist.c -o linkedlist/linkedlist.o

networkgraph.o:networkgraph.c
	${CC} ${CFLAGS} -c -I . networkgraph.c -o networkgraph.o

network_topology.o:network_topology.c
	${CC} ${CFLAGS} -c -I . network_topology.c -o network_topology.o


clean:
	rm *.o
	rm linkedlist/linkedlist.o
	rm *sh
