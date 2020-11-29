#include <stdio.h>
#include <stdlib.h>
#include "../networkgraph.h"
#include "dataLinkLayer.h"
#include "../linkedlist/linkedlist.h"


typedef struct mac_table_entry_
{
    mac_address_t mac_address;
    char oif_name[NETWORK_INTERFACE_NAME_SIZE];
    doublylinkedlist_t mac_entry_glue;
} mac_table_entry_t;
LINKED_LIST_TO_STRUCT(mac_glue_to_mac_entry, mac_table_entry_t, mac_entry_glue);


typedef struct mac_table_
{
    doublylinkedlist_t mac_entries;
} mac_table_t;


extern void initialize_mac_table(mac_table_t **mac_table)
{
    *mac_table = calloc(1, sizeof(mac_table_t));
    initialize_doubly_linkedlist(&((*mac_table)->mac_entries));
}


mac_table_entry_t * mac_table_lookup(mac_table_t *mac_table, char *mac)
{
    doublylinkedlist_t *current;
    mac_table_entry_t *mac_table_entry;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&mac_table->mac_entries, current)
    {
        mac_table_entry = mac_glue_to_mac_entry(current);

        if(strncmp(mac_table_entry->mac_address.mac_address, mac, sizeof(mac_address_t)) == 0)
        {
            return mac_table_entry;
        }
    } ITERATE_DOUBLY_LINKED_LIST_END(&mac_table->mac_entries, current);

    return NULL;
}


void clear_mac_table(mac_table_t *mac_table)
{
    doublylinkedlist_t *current;
    mac_table_entry_t *mac_table_entry;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&mac_table->mac_entries, current)
    {
        mac_table_entry = mac_glue_to_mac_entry(current);
        remove_doubly_linkedlist(current);
        free(mac_table_entry);
    } ITERATE_DOUBLY_LINKED_LIST_END(&mac_table->mac_entries, current);
}


void delete_mac_table_entry(mac_table_t *mac_table, char *mac)
{
    mac_table_entry_t *mac_table_entry;
    mac_table_entry = mac_table_lookup(mac_table, mac);

    if(!mac_table_entry)
    {
        return;
    }

    remove_doubly_linkedlist(&mac_table_entry->mac_entry_glue);

    free(mac_table_entry);
}


#define IS_MAC_TABLE_ENTRY_EQUAL(mac_entry_1, mac_entry_2)                                                                  \
    (                                                                                                                       \
     strncmp(mac_entry_1->mac_address.mac_address, mac_entry_2->mac_address.mac_address, sizeof(mac_address_t)) == 0 &&     \
     strncmp(mac_entry_1->oif_name, mac_entry_2->oif_name, NETWORK_INTERFACE_NAME_SIZE) == 0                                \
    )


boolean_t mac_table_entry_add(mac_table_t *mac_table, mac_table_entry_t *mac_table_entry)
{
    mac_table_entry_t *mac_table_entry_old = mac_table_lookup(mac_table, mac_table_entry->mac_address.mac_address);

    if(mac_table_entry_old && IS_MAC_TABLE_ENTRY_EQUAL(mac_table_entry_old, mac_table_entry))
    {
        return FALSE;
    }

    if(mac_table_entry_old)
    {
        delete_mac_table_entry(mac_table, mac_table_entry_old->mac_address.mac_address);
    }

    initialize_doubly_linkedlist(&mac_table_entry->mac_entry_glue);
    doubly_linkedlist_add_next(&mac_table->mac_entries, &mac_table_entry->mac_entry_glue);

    return TRUE;
}

void dump_mac_table(mac_table_t *mac_table)
{
    doublylinkedlist_t *current;
    mac_table_entry_t *mac_table_entry;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&mac_table->mac_entries, current)
    {
        mac_table_entry = mac_glue_to_mac_entry(current);
        printf("\tMAC Address : %u:%u:%u:%u:%u:%u \t Interface : %s\n",
                mac_table_entry->mac_address.mac_address[0],
                mac_table_entry->mac_address.mac_address[1],
                mac_table_entry->mac_address.mac_address[2],
                mac_table_entry->mac_address.mac_address[3],
                mac_table_entry->mac_address.mac_address[4],
                mac_table_entry->mac_address.mac_address[5],
                mac_table_entry->oif_name);
    } ITERATE_DOUBLY_LINKED_LIST_END(&mac_table->mac_entries, current);
}


static void l2_switch_mac_learning(network_node_t *network_node, char *source_mac, char *interface_name)
{
    boolean_t r_bytes;
    mac_table_entry_t *mac_table_entry = calloc(1, sizeof(mac_table_entry_t));
    memcpy(mac_table_entry->mac_address.mac_address, source_mac, sizeof(mac_address_t));
    strncpy(mac_table_entry->oif_name, interface_name, NETWORK_INTERFACE_NAME_SIZE);
    mac_table_entry->oif_name[NETWORK_INTERFACE_NAME_SIZE - 1] = '\0';

    r_bytes = mac_table_entry_add(NODE_MAC_TABLE(network_node), mac_table_entry);

    if(r_bytes == FALSE)
    {
        free(mac_table_entry);
    }
}


static void l2_switch_frame_forward(network_node_t *network_node, network_interface_t *recieving_interface, char *packet, unsigned int packet_size)
{
    ethernet_header_t *ethernet_header = (ethernet_header_t *)(packet);
    if(IS_MAC_BROADCAST_ADDRESS(ethernet_header->destignation_mac.mac_address))
    {
        send_packet_flood(network_node, recieving_interface, packet, packet_size);
        return;
    }

    mac_table_entry_t *mac_table_entry = mac_table_lookup(NODE_MAC_TABLE(network_node), ethernet_header->destignation_mac.mac_address);

    if(!mac_table_entry)
    {
        send_packet_flood(network_node, recieving_interface, packet, packet_size);
        return;
    }

    char *oif_name = mac_table_entry->oif_name;
    network_interface_t *oif = get_network_node_interface_by_name(network_node, oif_name);
    if(!oif)
    {
        return;
    }

    send_packet_out(packet, packet_size, oif);
}


void l2_switch_recieve_frame(network_interface_t *network_interface, char *packet, unsigned int packet_size)
{
    network_node_t *network_node = network_interface->attached_node;

    ethernet_header_t *ethernet_header = (ethernet_header_t *)packet;

    char *destignation_mac = (char *)ethernet_header->destignation_mac.mac_address;
    char *source_mac = (char *)ethernet_header->source_mac.mac_address;

    l2_switch_mac_learning(network_node, source_mac, network_interface->interface_name);
    l2_switch_frame_forward(network_node, network_interface, packet, packet_size);
}