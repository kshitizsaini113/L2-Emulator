#include "networkgraph.h"
#include <memory.h>
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

static unsigned int hashing_function_mac(void *pointer, unsigned int max_size)
{
    unsigned int mac_address=0, i =0;
    char *addition_string = (char*)pointer;
    while(i < max_size)
    {
        mac_address += *addition_string;
        mac_address*=97;
        addition_string++;
        i++;
    }
    return mac_address;
}

void interface_assign_mac_address(network_interface_t *network_interface)
{
    unsigned int hash_code_val = hashing_function_mac(network_interface, sizeof(network_interface_t));
// Function used to assigns the mac address to existing interface.
    memset(INTERFACE_MAC(network_interface), 0, sizeof(INTERFACE_MAC(network_interface)));
    memcpy(INTERFACE_MAC(network_interface), (char *)&hash_code_val, sizeof(unsigned int));
    
}


boolean_t network_node_device_type(network_node_t *network_node,
                                    unsigned int F)
{
// Sets the type of device (hub/switch/router)

    SET_BIT(network_node->node_network_properties.flags, F);
    // Sets the device bit according to the flag provided to define it's type.
    return TRUE;
}


boolean_t network_node_set_loopback_address(network_node_t *network_node, 
                                            char *ip_address)
{
// Sets the loopback address for a machine if not already set.
    assert(ip_address);

    if(IS_BIT_SET(network_node->node_network_properties.flags, HUB))
    {
        assert(0);
        // Stops the execution as a hub dont habve any IP Address.
    }

    if(!IS_BIT_SET(network_node->node_network_properties.flags, L3_ROUTER))
    {
        assert(0);
        // Stops the execution as L3 routing must be enabled.
    }

    network_node->node_network_properties.is_loopback_address_configured = TRUE;
    // Change the loopback flag to true as loopback adds is assigned.
    strncpy(NODE_LOOPBACK_ADDRESS(network_node), ip_address, 16);
    // Copies the string in empty ip buffer to be used as loopback ip.
    NODE_LOOPBACK_ADDRESS(network_node)[16] = '\0';

    return TRUE;
}


boolean_t network_node_set_interface_ip_address(network_node_t *network_node, 
                                                char *local_interface, 
                                                char *ip_address, 
                                                char mask)
{
// Sets the IP Address to a device.
    network_interface_t *network_interface = get_network_node_interface_by_name(network_node, local_interface);
    if(!network_interface)
    {
        assert(0);
        // If interface if already configured, then stop the execution.
    }

    strncpy(INTERFACE_IP(network_interface), ip_address, 16);
    INTERFACE_IP(network_interface)[16] = '\0';
    // Copies the ip address to the cleared ip buffer.
    network_interface->interface_network_properties.mask = mask;
    network_interface->interface_network_properties.is_ip_address_configured = TRUE;
    // Sets the mask for the IP and changes it's flag to true.

    return TRUE;
}


boolean_t network_node_unset_interface_ip_address(network_node_t *network_node, 
                                                    char *local_interface)
{
// Unsets the IP Address
    return TRUE;
}


void dump_network_graph_net(network_graph_t *network_graph)
{
    network_node_t *network_node;
    doublylinkedlist_t *current;
    network_interface_t *network_interface;
    unsigned int i;
    // Variable declarations for future use.

    printf("Topology Name : %s\n", network_graph->network_topology_name);
    // Prints the topology name

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&network_graph->network_node_list, current)
    {
        network_node = graph_glue_to_node(current);
        dump_network_node_properties(network_node);
        for(i=0; i<MAXIMUM_INTERFACE_PER_NODE; i++)
        {
            network_interface = network_node->interface[i];
            if(!network_interface)
            {
                break;
            }
            dump_network_interface_properties(network_interface);
        }
    } ITERATE_DOUBLY_LINKED_LIST_END(&network_graph->network_node_list, current)
    // Iterates the doubly linked list from begining to end using the macro defined loop.
    // During each iteration, it is printing the node properties and dump network Interface
}


void dump_network_node_properties(network_node_t *network_node)
{
    printf("\nNode Name : %s, UDP Port Number = %u\n", network_node->network_node_name, network_node->udp_port_number);
    printf("\nNode Flags : %u", network_node->node_network_properties.flags);
    if(network_node->node_network_properties.is_loopback_address_configured)
    {
        printf("\tLoopback Address : %s/32\n", NODE_LOOPBACK_ADDRESS(network_node));
    }
    // Dumps the information about a particular Node.
}


void dump_network_interface_properties(network_interface_t *network_interface)
{
    dump_network_interface(network_interface);

    if(network_interface->interface_network_properties.is_ip_address_configured)
    {
        printf("\tIP Address : %s/%u", INTERFACE_IP(network_interface), network_interface->interface_network_properties.mask);

        printf("\tMAC Address : %u:%u:%u:%u:%u:%u\n", INTERFACE_MAC(network_interface)[0],
                                                    INTERFACE_MAC(network_interface)[1],
                                                    INTERFACE_MAC(network_interface)[2],
                                                    INTERFACE_MAC(network_interface)[3],
                                                    INTERFACE_MAC(network_interface)[4],
                                                    INTERFACE_MAC(network_interface)[5]);
    }
    // Prints the IP which means that %s/%u show linux interface properties.
    else
    {
        printf("\tL2 Mode : %s\n", interface_layer_2_mode_string(INTERFACE_LAYER_2_MODE(network_interface)));

    }

    
}

char * packet_buffer_shift_right(char *packet, unsigned int packet_size, unsigned int total_buffer_size)
{
    char *temp = NULL;
    boolean_t need_temporary_memory = FALSE;

    if(packet_size * 2 > total_buffer_size)
    {
        need_temporary_memory = TRUE;
    }

    if(need_temporary_memory)
    {
        temp = calloc(1, packet_size);
        memcpy(temp, packet, packet_size);
        memset(packet, 0, total_buffer_size);
        memcpy(packet + (total_buffer_size - packet_size), temp, packet_size);
        free(temp);
        return packet + (total_buffer_size - packet_size);
    }

    memcpy(packet + (total_buffer_size - packet_size), packet, packet_size);
    memset(packet, 0, packet_size);
    
    return packet + (total_buffer_size - packet_size);
}

network_interface_t * node_get_matching_subnet_interface(network_node_t *network_node, char *ip_address)
{
    unsigned int i = 0;
    network_interface_t *network_interface;

    char *network_interface_address = NULL;
    char mask;
    char network_interface_subnet[16];
    char subnet2[16];

    for( ; i<MAXIMUM_INTERFACE_PER_NODE; i++)
    {
        network_interface = network_node->interface[i];

        if(!network_interface)
        {
            return NULL;
        }

        if(network_interface->interface_network_properties.is_ip_address_configured == FALSE)
        {
            continue;
        }

        network_interface_address = INTERFACE_IP(network_interface);
        mask = network_interface->interface_network_properties.mask;

        memset(network_interface_subnet, 0, 16);
        memset(subnet2, 0, 16);

        apply_subnet_mask(network_interface_address, mask, network_interface_subnet);
        apply_subnet_mask(ip_address, mask, subnet2);

        if(strncmp(network_interface_subnet, subnet2, 16) == 0)
        {
            return network_interface;
        }
    }
}