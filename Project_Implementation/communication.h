#ifndef __COMMUNICATION__
#define __COMMUNICATION__

#define MAXIMUM_PACKET_BUFFER_SIZE 2048

typedef struct network_node_ network_node_t;
typedef struct network_interface_ network_interface_t;

int send_packet_out(char *packet, unsigned int packet_size, network_interface_t *network_interface);
// Defining API to send the network packet out of the interface.
// Accepting the packet is dependent on reciever end.

int packet_recieve(network_node_t *network_node, network_interface_t *network_interface, char *packet, unsigned int packet_size);
// Defining API used to recieve packet from the reciever.

int send_packet_flood(network_node_t *network_node, network_interface_t *exempted_interface, char *packet, unsigned int packet_size);
// Defining API used to flood the packet out of all the interfaces of the node.

#endif