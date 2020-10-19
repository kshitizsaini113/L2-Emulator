#ifndef __NETWORKGRAPH__
#define __NETWORKGRAPH__
// Performing header file management to avoid duplicate inclusion of header file.


#include <assert.h>
#include "linkedlist/linkedlist.h"
#include "network.h"
// Including the required header files


#define NETWORK_NODE_NAME_SIZE          16
#define NETWORK_INTERFACE_NAME_SIZE     16
#define MAXIMUM_INTERFACE_PER_NODE      10
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
    interface_network_properties_t interface_network_properties;
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

    unsigned int udp_port_number;
    int udp_socket_file_descriptor;
    // Adding additional members to add communication properties to the node.
    // On node creation we will assign a unique udp port number to the node.

    node_network_properties_t node_network_properties;
};
LINKED_LIST_TO_STRUCT(graph_glue_to_node, network_node_t, graph_glue);


typedef struct network_graph_
{
// Structure defining a network graph
    char network_topology_name[32];
    doublylinkedlist_t network_node_list; 
} network_graph_t;


// Declaring the functions to be defined in the c file.
network_node_t * create_network_graph_node(network_graph_t *network_graph,
                                            char *network_node_name);

network_graph_t * create_new_network_graph(char *network_topology_name);

void insert_link_between_two_network_nodes(network_node_t *network_node1, 
                                            network_node_t *network_node2,
                                            char *sender_interface_name, 
                                            char *reciever_interface_name, 
                                            unsigned int traversal_cost);

void dump_network_graph(network_graph_t *network_graph);

void dump_network_node(network_node_t *network_node);

void dump_network_interface(network_interface_t *network_interface);
// Finishing the declarations.


static inline network_node_t * get_neighbour_node(network_interface_t *network_interface)
{
// Function returns the neighbour node of an interface

    assert(network_interface->attached_node);
    assert(network_interface->attached_link);
    // Using assert to test if node and link are present
    // It will continue with True, and will do nothing
    // If it evaluates to False, will stop the program.
    
    network_link_t *network_link = network_interface->attached_link;
    // Creating a link object.

    if(&network_link->interface1 == network_interface)
    {
        return network_link->interface2.attached_node;
    }
    else
    {
        return network_link->interface1.attached_node;
    }
    // Checking which node is connected and returning the node.
}


static inline int get_network_node_interface_available_slots(network_node_t *network_node)
{
// Function that returns the available network node.

    int i ;
    for( i = 0 ; i < MAXIMUM_INTERFACE_PER_NODE; i++)
    {
        if(network_node->interface[i])
        {
            continue;
        }
        return i;
    }
    // Iterate through each interface and check if it is empty, if empty returns the index of interface

    return -1;
    // Returns -1 if all the interfaces are full.
}


static inline network_interface_t * get_network_node_interface_by_name(network_node_t *network_node,
                                                                        char *interface_name)
{
    int i;
    network_interface_t *interface;

    for(i=0; i<MAXIMUM_INTERFACE_PER_NODE; i++)
    {
        interface = network_node->interface[i];
        if(!interface)
        {
            return NULL;
        }
        if(strncmp(interface->interface_name, interface_name, NETWORK_INTERFACE_NAME_SIZE) == 0)
        {
            return interface;
        }
    }

    return NULL;
}


#endif
// Ending Header File Management