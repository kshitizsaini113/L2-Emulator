#ifndef __COMMUNICATION__
#define __COMMUNICATION__

#define MAXIMUM_PACKET_BUFFER_SIZE 2048

typedef struct network_node_ network_node_t;
typedef struct network_interface_ network_interface_t;

int send_packet_out(char *packet, unsigned int packet_size, network_interface_t *network_interface);
// Defining API to send the network packet out of the interface.
// Accepting the packet is dependent on reciever end.

#endif