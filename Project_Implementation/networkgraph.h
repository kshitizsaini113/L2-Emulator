#ifndef __NETWORKGRAPH__
#define __NETWORKGRAPH__
// Performing header file management to avoid duplicate inclusion of header file.


#include <assert.h>
#include "linkedlist/linkedlist.h"
// Including the required header files


#define NETWORK_NODE_NAME_SIZE 16
#define NETWORK_INTERFACE_NAME_SIZE 16
#define MAXIMUM_INTERFACE_PER_NODE 10
// Defining macros to be used in the network graph


typedef struct network_node_ network_node_t;
typedef struct network_link_ network_link_t;
// Forward declarations to avoid compilation errors


typedef struct network_interface_
{
// Structure defining a network interface
    char interface_name[NETWORK_INTERFACE_NAME_SIZE];
    struct network_node_ *attached_node;
    struct network_link_ *attached_link;
} network_interface_t;


struct network_link_
{
// Structure defining a link in the network
    network_interface_t interface1;
    network_interface_t interface2;
    unsigned int traversal_cost;
};


struct network_node_
{
// Structure defining a network node
    char network_node_name[NETWORK_NODE_NAME_SIZE];
    network_interface_t *interface[MAXIMUM_INTERFACE_PER_NODE];
    doublylinkedlist_t graph_glue;
};
LINKED_LIST_TO_STRUCT(graph_glue_to_node, network_node_t, graph_glue);


typedef struct network_graph_
{
// Structure defining a network graph
    char network_topology_name[32];
    doublylinkedlist_t network_node_list;
} network_graph_t;


#endif
// Ending Header File Management