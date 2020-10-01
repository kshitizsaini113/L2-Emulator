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

    network_node_device_type(router_0, L3_ROUTER);
    network_node_set_loopback_address(router_0, "122.1.1.0");
    network_node_set_interface_ip_address(router_0, "eth0/4", "40.1.1.1", 24);
    network_node_set_interface_ip_address(router_0, "eth0/0", "20.1.1.1", 24);

    network_node_device_type(router_1, L3_ROUTER);
    network_node_set_loopback_address(router_1, "122.1.1.1");
    network_node_set_interface_ip_address(router_1, "eth0/1", "20.1.1.1", 24);
    network_node_set_interface_ip_address(router_1, "eth0/2", "30.1.1.1", 24);

    network_node_device_type(router_2, L3_ROUTER);
    network_node_set_loopback_address(router_2, "122.1.1.2");
    network_node_set_interface_ip_address(router_2, "eth0/3", "30.1.1.1", 24);
    network_node_set_interface_ip_address(router_2, "eth0/5", "40.1.1.1", 24);


    return network_topology_1;
}


