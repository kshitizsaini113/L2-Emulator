#include "networkgraph.h"
#include <stdio.h>
#include <unistd.h>
#include "LibCli/libcli.h"

extern network_graph_t *build_first_network_topology();
extern network_graph_t *build_linear_topo();
extern network_graph_t *build_layer_2_switch_topology();

extern void initialize_network_cli();

network_graph_t *network_topology = NULL;

int main(int argc, char **argv)
{
    initialize_network_cli();
    
    network_topology = build_layer_2_switch_topology();

    start_shell();

    return 0;
}
