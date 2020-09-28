#include "networkgraph.h"


network_graph_t * build_first_network_topology()
{
    network_graph_t *network_topology_1 = create_new_network_graph("Minor-1 Example-1");
    network_node_t *router_0 = create_network_graph_node(network_topology_1, "router_0");
    network_node_t *router_1 = create_network_graph_node(network_topology_1, "router_1");
    network_node_t *router_2 = create_network_graph_node(network_topology_1, "router_2");

    insert_link_between_two_network_nodes(router_0, router_1, "eth0/0", "eth0/1", 1);
    insert_link_between_two_network_nodes(router_1, router_2, "eth0/2", "eth0/3", 1);
    insert_link_between_two_network_nodes(router_0, router_2, "eth0/4", "eth0/5", 1);

    return network_topology_1;
}


