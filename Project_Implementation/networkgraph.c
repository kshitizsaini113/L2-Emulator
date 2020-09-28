#include "networkgraph.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// Including the required header files.


void insert_link_between_two_network_nodes(network_node_t *network_node1,
                                            network_node_t *network_node2,
                                            char *sender_interface_name,
                                            char *reciever_interface_name,
                                            unsigned int traversal_cost)
{
// Used to add a link between two existing nodes.

    network_link_t *network_link = calloc(1, sizeof(network_link_t));
    // Allocates memory to a link.

    strncpy(network_link->interface1.interface_name, sender_interface_name, NETWORK_INTERFACE_NAME_SIZE);
    network_link->interface1.interface_name[NETWORK_INTERFACE_NAME_SIZE] = '\0';
    strncpy(network_link->interface2.interface_name, reciever_interface_name, NETWORK_INTERFACE_NAME_SIZE);
    network_link->interface2.interface_name[NETWORK_INTERFACE_NAME_SIZE] = '\0';
    // Assigns the name to the link on both of the nodes.
    
    network_link->interface1.attached_link= network_link;
    network_link->interface2.attached_link= network_link;
    // Sets the back pointer to the link to associate them with the nodes.

    network_link->interface1.attached_node = network_node1;
    network_link->interface2.attached_node = network_node2;
    network_link->traversal_cost = traversal_cost;
    // Assigns the details of the node and traversal cost.

    int empty_intf_slot;
    // Variable declared for future use.

    empty_intf_slot = get_network_node_interface_available_slots(network_node1);
    network_node1->interface[empty_intf_slot] = &network_link->interface1;
    empty_intf_slot = get_network_node_interface_available_slots(network_node2);
    network_node2->interface[empty_intf_slot] = &network_link->interface2;
    // Checks for the available slot and if the slot is available then assign it to the nodes.
}


network_graph_t * create_new_network_graph(char *network_topology_name)
{
// Creates a new network graph

    network_graph_t *network_graph = calloc(1, sizeof(network_graph_t));
    strncpy(network_graph->network_topology_name, network_topology_name, 32);
    network_graph->network_topology_name[32] = '\0';
    // Calloc assign the memory required to the graph
    // Name is also assigned to the graph

    initialize_doubly_linkedlist(&network_graph->network_node_list);
    // Initialises the linked list as a part of the graph to merge the data together.

    return network_graph;
}


network_node_t * create_network_graph_node(network_graph_t *network_graph,
                                            char *network_node_name)
{
// Creates a new node for the existing network graph.

    network_node_t *network_node = calloc(1, sizeof(network_node_t));
    strncpy(network_node->network_node_name, network_node_name, NETWORK_NODE_NAME_SIZE);
    network_node->network_node_name[NETWORK_NODE_NAME_SIZE] = '\0';
    // Assigns memory and name to the newly created node

    initialize_doubly_linkedlist(&network_node->graph_glue);
    doubly_linkedlist_add_next(&network_graph->network_node_list, &network_node->graph_glue);
    // Initialises the linked list and add it to the next of the node to achieve the functionality of 
    // merging the data together in the node

    return network_node;
}
