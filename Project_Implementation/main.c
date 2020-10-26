#include "networkgraph.h"
#include "communication.h"
#include "LibCli/libcli.h"
#include <unistd.h>

extern network_graph_t *build_first_network_topology();
extern void initialize_network_cli();
network_graph_t *network_topology = NULL;

int main(int argc, char **argv)
{
    initialize_network_cli();
    network_topology = build_first_network_topology();

    sleep(3);

    network_node_t *snode = get_network_node_by_node_name(network_topology, "router_0");
    network_interface_t *oif = get_network_node_interface_by_name(snode, "eth0/0");

    char msg[]="Hello, from Minor 1 project.";
    send_packet_out(msg, strlen(msg), oif);

    start_shell();
    return 0;
}
