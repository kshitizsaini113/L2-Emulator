#include "networkgraph.h"

extern network_graph_t *build_first_network_topology();

int main(int argc, char **argv)
{
    network_graph_t *network_topology = build_first_network_topology();
    dump_network_graph(network_topology);
    return 0;
}
