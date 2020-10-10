COMPILER=gcc
COMPILER_FLAGS=-g

TARGET:main.sh LibCli/libcli.a

LIBS=-lpthread -L ./LibCli -lcli
OBJECT_FILES=linkedlist/linkedlist.o 			\
		  networkgraph.o 		   				\
		  network_topology.o					\
		  network.o								\
		  cli_module.o

main.sh:main.o ${OBJECT_FILES} LibCli/libcli.a
	${COMPILER} ${COMPILER_FLAGS} main.o ${OBJECT_FILES} -o main.sh ${LIBS}

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

cli_module.o:cli_module.c
	${CC} ${CFLAGS} -c -I . cli_module.c  -o cli_module.o

LibCli/libcli.a:
	(cd LibCli; make)

clean:
	rm *.o
	rm linkedlist/linkedlist.o
	rm *sh

cleanall:
	rm *.o
	rm linkedlist/linkedlist.o
	rm *sh
	(cd LibCli; make clean)
	
all:
	make
	(cd LibCli; make)
