#include "networkgraph.h"
#include <stdio.h>
#include "dataLinkLayer.h"
#include <stdlib.h>
#include <sys/socket.h>


static void promote_packet_to_network_layer(network_node_t *network_node, network_interface_t *network_interface, char *packet, unsigned int packet_size)
{
    printf("Network Layer (L3) Packet Recieved : Reciever Node %s, Interface %s, Data Recieved %s, PacketSize %u\n", network_node->network_node_name, network_interface->interface_name, packet, packet_size);
}

void send_arp_broadcast_request(network_node_t *network_node, network_interface_t *outgoing_network_interface, char *ip_address)
{
    ethernet_header_t *ethernet_header = calloc(1, sizeof(ethernet_header_t)+sizeof(arp_header_t));

    if(!outgoing_network_interface)
    {
        outgoing_network_interface = node_get_matching_subnet_interface(network_node, ip_address);

        if(!outgoing_network_interface)
        {
            printf("ERROR : %s, No subnet is elligible for ARP Resolution of the IP Address : %s", network_node->network_node_name, ip_address);
            return;
        }

        if(strncmp(INTERFACE_IP(outgoing_network_interface), ip_address, 16) == 0)
        {
            printf("ERROR : %s, Attempt for resolving the ARP for the local interface with IP : %s", network_node->network_node_name, ip_address);
            return;
        }
    }

    layer2_fill_with_broadcast_mac(ethernet_header->destignation_mac.mac_address);
    memcpy(ethernet_header->source_mac.mac_address, INTERFACE_MAC(outgoing_network_interface), sizeof(mac_address_t));
    ethernet_header->type_feild = ARP_MESSAGE;

    arp_header_t *arp_header = (arp_header_t *)(ethernet_header->payload);
    arp_header->hardware_type = 1;
    arp_header->protocol_type = 0x0800;
    arp_header->hardware_address_length = sizeof(mac_address_t);
    arp_header->protocol_address_length = 4;

    arp_header->operation_code = ARP_BROADCAST_REQUEST;

    memcpy(arp_header->source_mac.mac_address, INTERFACE_MAC(outgoing_network_interface), sizeof(mac_address_t));

    inet_pton(AF_INET, INTERFACE_IP(outgoing_network_interface), &arp_header->source_ip);
    // The inet_pton() function converts an address in its standard text presentation form into its numeric binary form.
    arp_header->source_ip = htonl(arp_header->source_ip);
    // The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.

    memset(arp_header->destignation_mac.mac_address, 0, sizeof(mac_address_t));

    inet_pton(AF_INET, ip_address, &arp_header->destignation_ip);
    arp_header->destignation_ip = htonl(arp_header->destignation_ip);

    ethernet_header->frame_check_sequence = 0;

    send_packet_out((char *)ethernet_header, sizeof(ethernet_header_t) + sizeof(arp_header_t), outgoing_network_interface);

    free(ethernet_header);
}


static void send_arp_reply_message(ethernet_header_t *ethernet_header_interface, network_interface_t *outgoing_network_interface)
{
    arp_header_t *arp_header_interface = (arp_header_t *)(ethernet_header_interface->payload);

    ethernet_header_t *ethernet_header_reply = calloc(1, sizeof(ethernet_header_t) + sizeof(arp_header_t));

    memcpy(ethernet_header_reply->destignation_mac.mac_address, arp_header_interface->source_mac.mac_address, sizeof(mac_address_t));
    memcpy(ethernet_header_reply->source_mac.mac_address, INTERFACE_MAC(outgoing_network_interface), sizeof(mac_address_t));

    ethernet_header_reply->type_feild = ARP_MESSAGE;

    arp_header_t *arp_header_reply = (arp_header_t *)(ethernet_header_reply->payload);

    arp_header_reply->hardware_type = 1;
    arp_header_reply->protocol_type = 0x0800;
    arp_header_reply->hardware_address_length = sizeof(mac_address_t);
    arp_header_reply->protocol_address_length = 4;

    arp_header_reply->operation_code = ARP_REPLY;
    memcpy(arp_header_reply->source_mac.mac_address, INTERFACE_MAC(outgoing_network_interface), sizeof(mac_address_t));

    inet_pton(AF_INET, INTERFACE_IP(outgoing_network_interface), &arp_header_reply->source_ip);
    arp_header_reply->source_ip = htonl(arp_header_reply->source_ip);

    memcpy(arp_header_reply->destignation_mac.mac_address, arp_header_interface->source_mac.mac_address, sizeof(mac_address_t));
    arp_header_reply->destignation_ip = arp_header_interface->source_ip;

    send_packet_out((char *)ethernet_header_reply, sizeof(ethernet_header_t) + sizeof(arp_header_t), outgoing_network_interface);

    ethernet_header_reply->frame_check_sequence = 0;

    free(ethernet_header_reply);
}


static void process_arp_reply_message(network_node_t *network_node, network_interface_t *iif, ethernet_header_t *ethernet_header)
{
    printf("%s : ARP Reply Message recieved on %s of node %s\n", __FUNCTION__, iif->interface_name, iif->attached_node->network_node_name);

    arp_table_update_from_reply(NODE_ARP_TABLE(network_node), (arp_header_t *)(ethernet_header->payload), iif);
}


static void process_arp_broadcast_request(network_node_t *network_node, network_interface_t *iif, ethernet_header_t *ethernet_header)
{
    printf("%s : ARP Broadcast Message recieved on %s of node %s\n", __FUNCTION__, iif->interface_name, iif->attached_node->network_node_name);

    char ip_address[16];
    arp_header_t *arp_header = (arp_header_t *)(ethernet_header->payload);

    unsigned int arp_destignation_ip = htonl(arp_header->destignation_ip);

    inet_ntop(AF_INET, &arp_destignation_ip, ip_address, 16);
    ip_address[15] = '\0';

    if(strncmp(INTERFACE_IP(iif), ip_address, 16))
    {
        printf("%s : ARP Broadcast Message dropped, Destignation IP didn't match.", network_node->network_node_name);
        return;
    }

    send_arp_reply_message(ethernet_header, iif);
}


extern void l2_switch_recieve_frame(network_interface_t *network_interface, char *packet, unsigned int packet_size);


void datalink_layer_frame_recieve(network_node_t *network_node, network_interface_t *network_interface, char *packet, unsigned int packet_size)
{
    ethernet_header_t *ethernet_header = (ethernet_header_t *)packet;

    if(layer_2_frame_recieving_qualify_on_interface(network_interface, ethernet_header) == FALSE)
    {
        printf("DataLink Layer Frame Rejected\n");
        return;
    }

    printf("DataLink Layer Frame Accepted\n");

    if(IS_INTERFACE_LAYER_3_MODE(network_interface))
    {
        switch(ethernet_header->type_feild)
        {
            case ARP_MESSAGE:
            {
                arp_header_t *arp_header = (arp_header_t *)(ethernet_header->payload);
                switch(arp_header->operation_code)
                {
                    case ARP_BROADCAST_REQUEST:
                        process_arp_broadcast_request(network_node, network_interface, ethernet_header);
                        break;
                    case ARP_REPLY:
                        process_arp_reply_message(network_node, network_interface, ethernet_header);
                        break;
                    default:
                        break;
                }
            }
            break;

            default:
                promote_packet_to_network_layer(network_node, network_interface, packet, packet_size);
                break;
        }
    }
    else if(INTERFACE_LAYER_2_MODE(network_interface) == ACCESS || INTERFACE_LAYER_2_MODE(network_interface) == TRUNK)
    {
        l2_switch_recieve_frame(network_interface, packet, packet_size);
    }
    else
    {
        return;
    }
}

void initialize_arp_table(arp_table_t **arp_table)
{
    *arp_table = calloc(1, sizeof(arp_table_t));
    initialize_doubly_linkedlist(&((*arp_table)->arp_entries));
}

arp_entry_t * arp_table_lookup(arp_table_t *arp_table, char *ip_address)
{
    doublylinkedlist_t *current;
    arp_entry_t *arp_entry;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&arp_table->arp_entries, current)
    {
        arp_entry = arp_glue_to_arp_entry(current);

        if(strncmp(arp_entry->ip_address.ip_address, ip_address, 16) == 0)
        {
            return arp_entry;
        }
    } ITERATE_DOUBLY_LINKED_LIST_END(&arp_table->arp_entries, current);

    return NULL;
}

void clear_arp_table(arp_table_t *arp_table)
{
    doublylinkedlist_t *current;
    arp_entry_t *arp_entry;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&arp_table->arp_entries, current)
    {
        arp_entry = arp_glue_to_arp_entry(current);
        remove_doubly_linkedlist(current);
        free(arp_entry);
    } ITERATE_DOUBLY_LINKED_LIST_END(&arp_table->arp_entries, current);
}

void delete_arp_table_entry(arp_table_t *arp_table, char *ip_address)
{
    arp_entry_t *arp_entry = arp_table_lookup(arp_table, ip_address);

    if(!arp_entry)
    {
        return;
    }

    remove_doubly_linkedlist(&arp_entry->arp_glue);
    free(arp_entry);
}

boolean_t arp_table_add_entry(arp_table_t *arp_table, arp_entry_t *arp_entry)
{
    arp_entry_t *arp_old_entry = arp_table_lookup(arp_table, arp_entry->ip_address.ip_address);

    if(arp_old_entry && memcmp(arp_old_entry, arp_entry, sizeof(arp_entry_t)) == 0)
    {
        return FALSE;
    }

    if(arp_old_entry)
    {
        delete_arp_table_entry(arp_table, arp_entry->ip_address.ip_address);
    }

    initialize_doubly_linkedlist(&arp_entry->arp_glue);
    doubly_linkedlist_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);

    return TRUE;
}

void arp_table_update_from_reply(arp_table_t *arp_table, arp_header_t *arp_header, network_interface_t *iif)
{
    unsigned int source_ip = 0;
    assert(arp_header->operation_code == ARP_REPLY);
    arp_entry_t *arp_entry = calloc(1, sizeof(arp_entry_t));
    source_ip = htonl(arp_header->source_ip);
    inet_ntop(AF_INET, &source_ip, &arp_entry->ip_address.ip_address, 16);
    arp_entry->ip_address.ip_address[15] = '\0';
    memcpy(arp_entry->mac_address.mac_address, arp_header->source_mac.mac_address, sizeof(mac_address_t));
    strncpy(arp_entry->oif_name, iif->interface_name, NETWORK_INTERFACE_NAME_SIZE);

    boolean_t rc = arp_table_add_entry(arp_table, arp_entry);
    if(rc == FALSE)
    {
        free(arp_entry);
    }
}

void dump_arp_table(arp_table_t *arp_table)
{
    doublylinkedlist_t *current;
    arp_entry_t *arp_entry;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&arp_table->arp_entries, current)
    {
        arp_entry = arp_glue_to_arp_entry(current);
        printf("IP: %s , MAC: %u:%u:%u:%u:%u:%u , OIF: %s\n",
            arp_entry->ip_address.ip_address,
            arp_entry->mac_address.mac_address[0],
            arp_entry->mac_address.mac_address[1],
            arp_entry->mac_address.mac_address[2],
            arp_entry->mac_address.mac_address[3],
            arp_entry->mac_address.mac_address[4],
            arp_entry->mac_address.mac_address[5],
            arp_entry->oif_name);
    } ITERATE_DOUBLY_LINKED_LIST_END(&arp_table->arp_entries, current);
}

void network_interface_set_l2_node(network_node_t *network_node, network_interface_t *network_interface, char *layer2_mode_operation)
{
    interface_layer_2_mode_t interface_layer_2_mode;

    if(strncmp(layer2_mode_operation, "access", strlen("access")) == 0)
    {
        interface_layer_2_mode = ACCESS;
    }
    else if(strncmp(layer2_mode_operation, "trunk", strlen("trunk")) == 0)
    {
        interface_layer_2_mode = TRUNK;
    }
    else
    {
        assert(0);
    }

    if(IS_INTERFACE_LAYER_3_MODE(network_interface))
    {
        network_interface->interface_network_properties.is_ip_address_configured_backup = TRUE;
        network_interface->interface_network_properties.is_ip_address_configured = FALSE;

        INTERFACE_LAYER_2_MODE(network_interface) = interface_layer_2_mode;
    }

    if(INTERFACE_LAYER_2_MODE(network_interface) == LAYER_2_MODE_UNKNOWN)
    {
        INTERFACE_LAYER_2_MODE(network_interface) = interface_layer_2_mode;
        return;
    }

    if(INTERFACE_LAYER_2_MODE(network_interface) == interface_layer_2_mode)
    {
        return;
    }

    if(INTERFACE_LAYER_2_MODE(network_interface) == ACCESS && interface_layer_2_mode == TRUNK)
    {
        INTERFACE_LAYER_2_MODE(network_interface) = interface_layer_2_mode;
        return;
    }

    if(INTERFACE_LAYER_2_MODE(network_interface) == TRUNK && interface_layer_2_mode == ACCESS)
    {
        INTERFACE_LAYER_2_MODE(network_interface) = interface_layer_2_mode;
    }
}

void network_node_set_interface_datalink_mode(network_node_t *network_node, char * interface_name, interface_layer_2_mode_t interface_layer_2_mode)
{
    network_interface_t *network_interface = get_network_node_interface_by_name(network_node, interface_name);
    assert(network_interface);

    network_interface_set_l2_node(network_node, network_interface, interface_layer_2_mode_string(interface_layer_2_mode));
}