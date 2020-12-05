#ifndef __DATALINKLAYER__
#define __DATALINKLAYER__

#include "../network.h"
#include "../linkedlist/linkedlist.h"

#define ARP_BROADCAST_REQUEST           1
#define ARP_REPLY                       2
#define ARP_MESSAGE                     806
#define BROADCAST_MAC_ADDRESS           0xFFFFFFFFFFFF

#define ETHERNET_HEADER_SIZE_EXCLUDING_PAYLOAD 18

typedef struct network_interface_ network_interface_t;

#pragma pack (push, 1)

typedef struct arp_header_
{
    short hardware_type;
    // HardCoded as 1 for ethernet cable
    short protocol_type;
    // Hardcoded as 0x0800 for IPv4
    char hardware_address_length;
    // 6 for MAC Address
    char protocol_address_length;
    // 4 for IPv4 
    short operation_code;
    // Can be request or reply
    mac_address_t source_mac;
    unsigned int source_ip;
    // MAC and IP of sender
    mac_address_t destignation_mac;
    unsigned int destignation_ip;
    // Only the IP ofdestignation as we wish to know the MAC of reciever
} arp_header_t;


typedef struct ethernet_header_
{
    mac_address_t destignation_mac;
    mac_address_t source_mac;
    unsigned short type_feild;
    char payload[248];
    unsigned int frame_check_sequence;
}ethernet_header_t;

#pragma pack (pop)

typedef struct arp_table_
{
    doublylinkedlist_t arp_entries;
} arp_table_t;

typedef struct arp_entry_
{
    ip_address_t ip_address;
    mac_address_t mac_address;
    char oif_name[NETWORK_INTERFACE_NAME_SIZE];
    doublylinkedlist_t arp_glue;
} arp_entry_t;
LINKED_LIST_TO_STRUCT(arp_glue_to_arp_entry, arp_entry_t, arp_glue);

static inline boolean_t layer_2_frame_recieving_qualify_on_interface(network_interface_t *network_interface, ethernet_header_t *ethernet_header)
{
    if(!IS_INTERFACE_LAYER_3_MODE(network_interface) && INTERFACE_LAYER_2_MODE(network_interface) == LAYER_2_MODE_UNKNOWN)
    {
        return FALSE;
    }
    // Checks if the interface is working in L3 mode by checking the presence of
    // IP-Address. The absence of IP-Address automatically makes it work in L2 Mode.
    // 
    // For the interfaces working in L2-Mode we need to accept all the frames as
    // L2-Switch never discards the frames based on MAC Addresses.

    if(!IS_INTERFACE_LAYER_3_MODE(network_interface) && (INTERFACE_LAYER_2_MODE(network_interface) == ACCESS || INTERFACE_LAYER_2_MODE(network_interface) == TRUNK))
    {
        return TRUE;
    }
    // Returns true if the recieving machine must accept the frame.

    if(IS_INTERFACE_LAYER_3_MODE(network_interface) && memcmp(INTERFACE_MAC(network_interface), ethernet_header->destignation_mac.mac_address, sizeof(mac_address_t)) == 0)
    {
        return TRUE;
    }
    // Returns true if the destignation address is broadcast address.

    if(IS_INTERFACE_LAYER_3_MODE(network_interface) && IS_MAC_BROADCAST_ADDRESS(ethernet_header->destignation_mac.mac_address))
    {
        return TRUE;
    }
    // Returns true if the destignation address is broadcast address.

    return FALSE;
}

void send_arp_broadcast_request(network_node_t *network_node, network_interface_t *network_interface, char *ip_address);

#define ARE_ARP_ENTRIES_EQUAL(arp_entry_1, arp_entry_2)                                                 \
    (                                                                                                   \
     strncmp(arp_entry_1->ip_address.ip_address, arp_entry_2->ip_address.ip_address, 16) == 0 &&        \
     strncmp(arp_entry_1->mac_address.mac_address, arp_entry_2->mac_address.mac_address, 6) == 0 &&     \
     strncmp(arp_entry_1->oif_name, arp_entry_2->oif_name, NETWORK_INTERFACE_NAME_SIZE) == 0            \
    )

void initialize_arp_table(arp_table_t **arp_table);

arp_entry_t * arp_table_lookup(arp_table_t *arp_table, char *ip_address);

void clear_arp_table(arp_table_t *arp_table);

void delete_arp_table_entry(arp_table_t *arp_table, char *ip_address);

boolean_t arp_table_add_entry(arp_table_t *arp_table, arp_entry_t *arp_entry);

void dump_arp_table(arp_table_t *arp_table);

void arp_table_update_from_reply(arp_table_t *arp_table, arp_header_t *arp_header, network_interface_t *iif);

void network_node_set_interface_datalink_mode(network_node_t *network_node, char * interface_name, interface_layer_2_mode_t interface_layer_2_mode);

#endif