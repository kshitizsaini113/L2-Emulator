#include "communication.h"
#include "networkgraph.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

static int _send_packet_out(int socket_file_descriptor, char *packet_data, unsigned int packet_size, unsigned int destignation_udp_port_number)
{
// Sends the packet to the specified IP and port.

    int r_bytes;
    struct sockaddr_in destignation_address;

    struct hostent *host = (struct hostent *) gethostbyname("127.0.0.1");
    // Search the host by the name 127.0.0.1
    destignation_address.sin_family = AF_INET;
    destignation_address.sin_port = destignation_udp_port_number;
    destignation_address.sin_addr = *((struct in_addr *)host->h_addr);

    r_bytes = sendto(socket_file_descriptor, packet_data, packet_size, 0, (struct sockaddr *)&destignation_address, sizeof(struct sockaddr));
    // Sends the packet using sendto system API and returns the number of bytes sent.

    return r_bytes;
}

static unsigned int udp_port_number = 40000;

static unsigned int get_next_udp_port_number()
{
// Returns a unique udp port number.

    return udp_port_number++;
}

void initialize_udp_port_socket(network_node_t *network_node)
{
// Initializes the udp port socket for the network node.

    network_node->udp_port_number = get_next_udp_port_number();
    // Fetches a unique UDP port number.

    int udp_socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // Creating a unique UDP Socket.
    // 
    // AF_INET is an address family that is used to designate the type of addresses that 
    // socket can communicate with (in this case, Internet Protocol v4 addresses)
    // 
    // SOCK_DGRAM is a datagram-based protocol. 
    // We send one datagram and get one reply and then the connection terminates.
    // 
    // IPPROTO_UDP defined the protocols to be used for the UDP programming.

    struct sockaddr_in node_address;
    // The SOCKADDR_IN structure specifies a transport address and port for the AF_INET address family.

    node_address.sin_family             = AF_INET;
    node_address.sin_port               = network_node->udp_port_number;
    node_address.sin_addr.s_addr        = INADDR_ANY;
    // INADDR_ANY is used when we don't need to bind a socket to a specific IP. 
    // When we use this value as the address when calling bind(), 
    // the socket accepts connections to all the IPs of the machine.

    if(bind(udp_socket_file_descriptor, (struct sockaddr *)&node_address, sizeof(struct sockaddr)) == -1)
    {
        printf("ERROR: Socket binding failed for the network node %s\n", network_node->network_node_name);
        return;
    }
    // bind function binds the socket to the address and port number specified in addr.

    network_node->udp_socket_file_descriptor = udp_socket_file_descriptor;
    // Assigns the created UDP file descriptor.
}


static char recieved_buffer[MAXIMUM_PACKET_BUFFER_SIZE];
static char send_buffer[MAXIMUM_PACKET_BUFFER_SIZE];

static void _packet_recieve(network_node_t *recieving_node, char *packet_with_aux_data, unsigned int packet_size)
{
// Used to segrigate header and the payload in the packet recieved.

    char *recieving_interface_name = packet_with_aux_data;
    network_interface_t *recieving_interface = get_network_node_interface_by_name(recieving_node, recieving_interface_name);

    if(!recieving_interface)
    {
        printf("ERROR : Packet recieved on unknown network interface %s on network node %s\n", recieving_interface->interface_name, recieving_node->network_node_name);
        return;
    }

    packet_recieve(recieving_node, recieving_interface, packet_with_aux_data + NETWORK_INTERFACE_NAME_SIZE, packet_size - NETWORK_INTERFACE_NAME_SIZE);
    // Third argument is the actual data.
}


static void * _network_start_packet_reciever_thread(void *argument)
{
    // The defined function will be executed by thread.
    // Based on destignation port number, number in the echoed data recieved by our application,
    // application hand over the data to destignation node.

    network_node_t *network_node;
    doublylinkedlist_t *current;

    fd_set active_socket_file_descriptor, backup_socket_file_descriptor;
    // Defining variables to store udp socket file descriptors.

    int socket_max_file_descriptor = 0;
    int bytes_recieved = 0;

    network_graph_t *topology = (void *)argument;

    int address_length = sizeof(struct sockaddr);

    FD_ZERO(&active_socket_file_descriptor);
    FD_ZERO(&backup_socket_file_descriptor);

    struct sockaddr_in sender_Address;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&topology->network_node_list, current)
    {
        network_node = graph_glue_to_node(current);

        if(!network_node->udp_socket_file_descriptor)
        {
            continue;
        }
        // Skips the node for which the file descriptor is not defined.

        if(network_node->udp_socket_file_descriptor > socket_max_file_descriptor)
        {
            socket_max_file_descriptor = network_node->udp_socket_file_descriptor;
        }
        // Assigns the maximum file descriptor to another variable.

        FD_SET(network_node->udp_socket_file_descriptor, &backup_socket_file_descriptor);

    } ITERATE_DOUBLY_LINKED_LIST_END(&topology->network_node_list, current)
    // Iterating over each node of the topology and add add each udp file descriptor of each node
    // to the backup file descriptor in orfer to monitor them.

    while(1)
    {
    // We need to listen for infinite time as the user can recieve the packet anytime.
        memcpy(&active_socket_file_descriptor, &backup_socket_file_descriptor, sizeof(fd_set));
        // Copying all the backup file descriptor to active.
        select(socket_max_file_descriptor+1, &active_socket_file_descriptor, NULL, NULL, NULL);
        // Select is a blocking system call.
        // For all the present file descriptor, unless one is activated the system call will 
        // remain blocked.

        // When the data is recieved we need to iterate over all of the data and process it.
        ITERATE_DOUBLY_LINKED_LIST_BEGINING(&topology->network_node_list, current)
        {
            network_node = graph_glue_to_node(current);
            // We will fetch a particular node from the graph.

            if(FD_ISSET(network_node->udp_socket_file_descriptor, &active_socket_file_descriptor))
            {
            // Cecking if the file descriptor of the current node is active or not.
            // If activated, we need to recieve the data on the specified node.
                memset(recieved_buffer, 0, MAXIMUM_PACKET_BUFFER_SIZE);
                bytes_recieved = recvfrom(network_node->udp_socket_file_descriptor, (char *)recieved_buffer, MAXIMUM_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_Address, &address_length);
                // Recieving the data from recvfrom API.

                _packet_recieve(network_node, recieved_buffer, bytes_recieved);
                // Processing the recieved packet.
            }
        } ITERATE_DOUBLY_LINKED_LIST_END(&topology->network_node_list, current)

    }
}


void network_start_packet_reciever_thread(network_graph_t *network_topology)
{
    // We will do this process using threads as we are required to listen to all the nodes
    // of the network simultaneously. (Multithreading)
    // 
    // pthread library is being used for multithreading support.
    pthread_attr_t attribute;
    pthread_t recieve_packet_thread;

    pthread_attr_init(&attribute);
    // Initialising pthread attribute.
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
    // When a thread is created detached (PTHREAD_CREATE_DETACHED), its thread ID and 
    // other resources can be reused as soon as the thread terminates.

    pthread_create(&recieve_packet_thread, &attribute, _network_start_packet_reciever_thread, (void *)network_topology);
    // Creates a thread and uses the followinf function on thread creation.
}

int send_packet_out(char *packet, unsigned int packet_size, network_interface_t *network_interface)
{
// Node communicates by sending data to Node's port number with IP=127.0.0.1
    int r_bytes=0;

    network_node_t *sending_node = network_interface->attached_node;
    network_node_t *neighbour_node = get_neighbour_node(network_interface);

    if(!neighbour_node)
    {
        // Returns -1 in case no node is found to avoid errors as data can only be transfered
        // between connecting nodes.
        return -1;
    }

    unsigned int destignation_udp_port_number = neighbour_node->udp_port_number;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // Creating a network udp socket.

    if(sock < 0)
    {
        printf("ERROR : Sending Socket Creation failed, errorno = %d", errno);
        return -1;
    }

    network_interface_t *other_interface = &network_interface->attached_link->interface1== \
        network_interface?&network_interface->attached_link->interface2:&network_interface->attached_link->interface1;
    // Checking which interface to send data to by checking the connected interface.

    memset(send_buffer, 0, MAXIMUM_PACKET_BUFFER_SIZE);
    // Clears the memory (removes garbage) from the send_buffer

    char *packet_with_auxilarry_data = send_buffer;

    strncpy(packet_with_auxilarry_data, other_interface->interface_name, NETWORK_INTERFACE_NAME_SIZE);
    // Assigns header to the packet.

    packet_with_auxilarry_data[NETWORK_INTERFACE_NAME_SIZE] = '\0';
    // Terminates the packet header.

    memcpy(packet_with_auxilarry_data + NETWORK_INTERFACE_NAME_SIZE, packet, packet_size);
    // Assigns data to the packet.

    r_bytes = _send_packet_out(sock, packet_with_auxilarry_data, packet_size + NETWORK_INTERFACE_NAME_SIZE, destignation_udp_port_number);
    // Send the packet to the destignation.

    close(sock);
    // Closes the socket created.

    return r_bytes;
}

int packet_recieve(network_node_t *network_node, network_interface_t *network_interface, char *packet, unsigned int packet_size)
{
// Entry point into data link layer from physical layer
// Represent the start of ingress journey of the packet start from here in TCP/IP Stack

    printf("Message Recieved : %s, on node : %s, IIF: %s\n", packet, network_node->network_node_name, network_interface->interface_name);

    return 0;
}