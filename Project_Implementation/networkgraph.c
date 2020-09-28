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


void dump_network_graph(network_graph_t *network_graph)
{
// Prints the information about network graph on the screen.

    doublylinkedlist_t *current;
    network_node_t *network_node;
    // Creates variables for future use.
    
    printf("Network Topology Name : %s\n\n\n", network_graph->network_topology_name);
    // Printing the network topology name.

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&network_graph->network_node_list, current){

        network_node = graph_glue_to_node(current);
        dump_network_node(network_node);    
    } ITERATE_DOUBLY_LINKED_LIST_END(&network_graph->network_node_list, current);
    // Using macrodefined function to iterate over the doubly linked list to print the data of each node of the network.
}


void dump_network_node(network_node_t *network_node)
{
// Prints the information about network nodes on the screen.

    unsigned int i = 0;
    network_interface_t *network_interface;
    // Initialises the variables for future use.

    printf("Network Node Name : %s\n", network_node->network_node_name);
    // Prints the node name on the screen.
    for( ; i < MAXIMUM_INTERFACE_PER_NODE; i++)
    {   
        network_interface = network_node->interface[i];
        if(!network_interface)
        {
            break;
        }
        dump_network_interface(network_interface);
    }
    printf("\n");
    // Iterates over each interface of the node and prints the information about it.
    // When all the used interfaces are complete then break the loop.
}


void dump_network_interface(network_interface_t *network_interface)
{
// Prints the information about the interface.

    network_link_t *network_link = network_interface->attached_link;
    network_node_t *neighbour_node = get_neighbour_node(network_interface);
    // Fetches the information about the attached link as well as the neighbour nodes.

    printf("Local Node : %s, Interface Name : %s, Neighbour Node : %s, Traversal Cost : %u\n", 
            network_interface->attached_node->network_node_name, 
            network_interface->interface_name, neighbour_node->network_node_name,
            network_link->traversal_cost); 
    // Prints the information about a single node on the screen.
}