COMPILER=gcc
COMPILER_FLAGS=-g
TARGET:main.sh

OBJECT_FILES=linkedlist/linkedlist.o 			\
		  networkgraph.o 		   				\
		  network_topology.o					\
		  network.o

main.sh:main.o ${OBJECT_FILES}
	${COMPILER} ${COMPILER_FLAGS} main.o ${OBJECT_FILES} -o main.sh

main.o:main.c
	${COMPILER} ${COMPILER_FLAGS} -c main.c -o main.o

linkedlist/linkedlist.o:linkedlist/linkedlist.c
	${COMPILER} ${COMPILER_FLAGS} -c -I linkedlist linkedlist/linkedlist.c -o linkedlist/linkedlist.o

networkgraph.o:networkgraph.c
	${COMPILER} ${COMPILER_FLAGS} -c -I . networkgraph.c -o networkgraph.o

network_topology.o:network_topology.c
	${COMPILER} ${COMPILER_FLAGS} -c -I . network_topology.c -o network_topology.o

network.o:network.c
	${COMPILER} ${COMPILER_FLAGS} -c -I . network.c -o network.o


clean:
	rm *.o
	rm linkedlist/linkedlist.o
	rm *sh
