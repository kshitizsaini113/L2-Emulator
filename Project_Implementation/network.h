#ifndef __NET__
#define __NET__
// Performing header file management to avoid duplicate inclusion of header file.


#include "utils.h"
#include <memory.h>
// Including the required header files


#define L3_ROUTER       ( 1 << 0 )
#define L2_SWITCH       ( 1 << 1 )
#define HUB             ( 1 << 2 )
// We use 1<<0, 1<<1 and 1<<2 instead of 1, 2 and 4 because
// we want our values to be bit masks instead of just normal integers
// And for a macro we cant define a data-type, using the shift operator makes this possible.
// 
// A mask defines which bits we want to keep, and which bits we want to clear.
// Masking is the act of applying a mask to a value. This is accomplished by doing:
//      Bitwise ANDing in order to extract a subset of the bits in the value
//      Bitwise ORing in order to set a subset of the bits in the value
//      Bitwise XORing in order to toggle a subset of the bits in the value
// 
// Common use-case: Extracting individual bytes from a larger word. We define the high-order 
// bits in the word as the first byte. We use two operators for this, &, and >> (shift right).


// Forward Declarations Begin
typedef struct network_graph_ network_graph_t;

typedef struct network_interface_ network_interface_t;

typedef struct network_node_ network_node_t;
// Forward Declarations End

#pragma pack (push, 1)

typedef struct ip_address_
{
    char ip_address[16];
} ip_address_t;
// Defining a structure to represent an IP Address


typedef struct mac_address_
{
  unsigned  char mac_address[6];
} mac_address_t;
// Defining a structure to represent a Mac Address

#pragma pack(pop)

typedef struct arp_table_ arp_table_t;
typedef struct mac_table_ mac_table_t;

typedef struct node_network_properties_
{
    unsigned int flags;
    // Flag to represent the number of connected devices.

    arp_table_t *arp_table;
    mac_table_t *mac_table;

    boolean_t is_loopback_address_configured;
    ip_address_t loopback_address;
    // Defining two variables for our usage to make our program efficient.
    // The boolean value will check if the loopback address is configured or not,
    // If the flag is true, then only we will try to access the loopback address.
} node_network_properties_t;


extern void initialize_arp_table(arp_table_t **arp_table);
extern void initialize_mac_table(mac_table_t **mac_table);

static inline void initialize_node_network_properties(node_network_properties_t *node_network_properties)
{
// Initializes the network node.
    node_network_properties->flags = 0;
    node_network_properties->is_loopback_address_configured = FALSE;
    memset(node_network_properties->loopback_address.ip_address, 0, 16);
    // memset() is used to fill a block of memory with a particular value.
    // The syntax of memset() function is as follows :
        // void *memset(void *ptr, int x, size_t n);
            // ptr ==> Starting address of memory to be filled
            // x   ==> Value to be filled
            // n   ==> Number of bytes to be filled starting from ptr to be filled
    // memset is used instead of any string assignment because memset clears the budder for assigning
    // a value while string only assigns the value in the specified set and keeps the else value as a 
    // garbage making us access a completely different thing
    initialize_arp_table(&(node_network_properties->arp_table));
    initialize_mac_table(&(node_network_properties->mac_table));
}

typedef enum{
    ACCESS,
    TRUNK,
    LAYER_2_MODE_UNKNOWN
} interface_layer_2_mode_t;

static inline char * interface_layer_2_mode_string(interface_layer_2_mode_t interface_layer_2_mode)
{
    switch (interface_layer_2_mode)
    {
        case ACCESS:
            return "access";
        case TRUNK:
            return "trunk";
        default:
            return "LAYER_2_MODE_UNKNOWN";
    }
}


#define MAXIMUM_VLAN_MEMBERSHIP 10


typedef struct interface_network_properties_
{
    mac_address_t mac_address;

    interface_layer_2_mode_t interface_layer_2_mode;

    unsigned int vlans[MAXIMUM_VLAN_MEMBERSHIP];
    boolean_t is_ip_address_configured_backup;

    boolean_t is_ip_address_configured;
    ip_address_t ip_address;
    char mask;
} interface_network_properties_t;


static inline void initialize_interface_network_properties(interface_network_properties_t *interface_network_properties)
{
    memset(interface_network_properties->mac_address.mac_address, 0, sizeof(interface_network_properties->mac_address.mac_address));
    interface_network_properties->interface_layer_2_mode = LAYER_2_MODE_UNKNOWN;
    memset(interface_network_properties->vlans, 0, sizeof(interface_network_properties->vlans));


    interface_network_properties->is_ip_address_configured = FALSE;
    memset(interface_network_properties->ip_address.ip_address, 0, 16);
    interface_network_properties->mask = 0;
}


void interface_assign_mac_address(network_interface_t *network_interface);


// Defining shorthand macros to avoid code repetition
#define INTERFACE_MAC(interface_ptr)                                                            \
    ((interface_ptr)->interface_network_properties.mac_address.mac_address)

#define INTERFACE_IP(interface_ptr)                                                             \
    ((interface_ptr)->interface_network_properties.ip_address.ip_address)

#define NODE_LOOPBACK_ADDRESS(network_node_ptr)                                                 \
    (network_node_ptr->node_network_properties.loopback_address.ip_address)

#define NODE_ARP_TABLE(network_node_ptr)                                                        \
    (network_node_ptr->node_network_properties.arp_table)

#define NODE_MAC_TABLE(network_node_ptr)                                                        \
    (network_node_ptr->node_network_properties.mac_table)

#define INTERFACE_LAYER_2_MODE(network_interface_ptr)                                           \
    (network_interface_ptr->interface_network_properties.interface_layer_2_mode)

#define IS_INTERFACE_LAYER_3_MODE(network_interface_ptr)                                        \
    (network_interface_ptr->interface_network_properties.is_ip_address_configured == TRUE)
// Finishing the shorthand macro declaration


// Defining functions to be defined as API in network.c
boolean_t network_node_device_type(network_node_t *network_node,
                                    unsigned int F);

boolean_t network_node_set_loopback_address(network_node_t *network_node, 
                                            char *ip_address);

boolean_t network_node_set_interface_ip_address(network_node_t *network_node, 
                                                char *local_interface, 
                                                char *ip_address, 
                                                char mask);
                                    
boolean_t network_node_unset_interface_ip_address(network_node_t *network_node, 
                                                    char *local_interface);
// Function declarations


// Functions to dump information on the screen
void dump_network_graph_net(network_graph_t *network_graph);

void dump_network_node_properties(network_node_t *network_node);

void dump_network_interface_properties(network_interface_t *network_interfacef);
// Function declaration ends

network_interface_t * node_get_matching_subnet_interface(network_node_t *network_node, char *ip_address);

#endif
// Ending Header File Management
