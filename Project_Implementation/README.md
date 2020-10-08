# Generic Graph Construction

Here, we are focusing on implementation of network graph and its functions in the files [networkgraph.h](networkgraph.h) and [networkgraph.c](networkgraph.c). Further the network topology is created in the file [network_topology.c](network_topology.c) and all the function calling is done in [main.c](main.c).

1) The file [networkgraph.h](networkgraph.h) contains the structure definations, function declarations and macros used for the Network Graph implementation.

2) The file [networkgraph.c](networkgraph.c) contains the functions definations for the functions defined in [networkgraph.h](networkgraph.h) to be used for Network Graph implementation.

3) The file [network.h](network.h) contains the structures, macros and function declarations used for the Networking part. *i.e. IP, MAC and Loopback* 

4) The file [network.c](network.c) contains the functional definations for the functions defined in [network.h](network.h) to assign network properties to our network. *Network constrains like IP, MAC and Loopback.*

5) The file [network_topology.c](network_topology.c) is used for creating network topologies.

6) The file [main.c](main.c) is used to call the functions.

7) Makefile is created to make the project compilation easy.

## Running the project

For running the project you need to run the following commands in the Project_Implementation directory.

```
make
./main.sh
```

----------------------

> The code is well defined in form of comments, in case you find any more efficient way, the changes are accepted. Please generate the pull request in the same way the project is already documented making it easier to understand. Refer to CONTRIBUTING for cotributation guidelines.
