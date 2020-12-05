#include "networkgraph.h"
#include "communication.h"
#include <stdio.h>

extern void network_start_packet_reciever_thread(network_graph_t *network_topology);

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

    network_start_packet_reciever_thread(network_topology_1);
    // We will be iterating over UDP file descriptors of each node and we will listen on them.

    printf("\n\n\n");
    printf("                                       +---------------+\n");
    printf("                                eth0/4 |               | eth0/0\n");
    printf("         +-----------------------------+   router_0    +-----------------------------+\n");
    printf("         |                 40.1.1.1/24 |               | 20.1.1.1/24                 |\n");
    printf("         |                             +---------------+                             |\n");
    printf("         |                                                                           |\n");
    printf("         |                                                                           |\n");
    printf("         |                                                                           |\n");
    printf("         | 40.1.1.2/24                                                   20.1.1.2/24 |\n");
    printf("         | eth0/5                                                             eth0/1 |\n");
    printf(" +-------+-------+                                                           +-------+-------+\n");
    printf(" |               | eth0/3                                             eth0/2 |               |\n");
    printf(" |    router_2   +-----------------------------------------------------------+   router_1    |\n");
    printf(" |               | 30.1.1.2/24                                   30.1.1.1/24 |               |\n");
    printf(" +---------------+                                                           +---------------+\n");
    printf("\n\n\n");

    return network_topology_1;
}


network_graph_t * build_linear_topo()
{

    network_graph_t *network_topology_2 = create_new_network_graph("Minor-1 Example-2");
    network_node_t *router_0 = create_network_graph_node(network_topology_2, "router_0");
    network_node_t *router_1 = create_network_graph_node(network_topology_2, "router_1");
    network_node_t *router_2 = create_network_graph_node(network_topology_2, "router_2");
    
    insert_link_between_two_network_nodes(router_0, router_1, "eth0/1", "eth0/2", 1);
    insert_link_between_two_network_nodes(router_1, router_2, "eth0/3", "eth0/4", 1);

    network_node_device_type(router_0, L3_ROUTER);
    network_node_device_type(router_1, L3_ROUTER);
    network_node_device_type(router_2, L3_ROUTER);

    network_node_set_loopback_address(router_0, "122.1.1.1");
    network_node_set_loopback_address(router_1, "122.1.1.2");
    network_node_set_loopback_address(router_2, "122.1.1.3");

    network_node_set_interface_ip_address(router_0, "eth0/1", "10.1.1.1", 24);
    network_node_set_interface_ip_address(router_1, "eth0/2", "10.1.1.2", 24);
    network_node_set_interface_ip_address(router_1, "eth0/3", "20.1.1.2", 24);
    network_node_set_interface_ip_address(router_2, "eth0/4", "20.1.1.1", 24);

    network_start_packet_reciever_thread(network_topology_2);

    printf("\n\n\n");
    printf("+---------------+                              +---------------+                               +---------------+\n");
    printf("|               |                              |               |                               |               |\n");
    printf("|   router_0    |eth0/1                  eth0/2|   router_1    |eth0/3                   eth0/4|   router_2    |\n");
    printf("|   122.1.1.1   +------------------------------+   122.1.1.2   |+------------------------------+   122.1.1.3   |\n");
    printf("|               |10.1.1.1/24        10.1.1.2/24|               |11.1.1.2/24         11.1.1.1/24|               |\n");
    printf("+---------------+                              +---------------+                               +---------------+\n");
    printf("\n\n\n");

    return network_topology_2;
}


network_graph_t * build_layer_2_switch_topology()
{

    network_graph_t *network_topology_3 = create_new_network_graph("Simple L2 Switch Demo graph");
    network_node_t *router_1 = create_network_graph_node(network_topology_3, "router_1");
    network_node_t *router_2 = create_network_graph_node(network_topology_3, "router_2");
    network_node_t *router_3 = create_network_graph_node(network_topology_3, "router_3");
    network_node_t *router_4 = create_network_graph_node(network_topology_3, "router_4");
    network_node_t *L2Switch = create_network_graph_node(network_topology_3, "L2Switch");

    insert_link_between_two_network_nodes(router_1, L2Switch, "eth0/5", "eth0/4", 1);
    insert_link_between_two_network_nodes(router_2, L2Switch, "eth0/8", "eth0/3", 1);
    insert_link_between_two_network_nodes(router_3, L2Switch, "eth0/6", "eth0/2", 1);
    insert_link_between_two_network_nodes(router_4, L2Switch, "eth0/7", "eth0/1", 1);

    network_node_device_type(router_1, L3_ROUTER);
    network_node_set_loopback_address(router_1, "122.1.1.1");
    network_node_set_interface_ip_address(router_1, "eth0/5", "10.1.1.2", 24);
    
    network_node_device_type(router_2, L3_ROUTER);
    network_node_set_loopback_address(router_2, "122.1.1.2");
    network_node_set_interface_ip_address(router_2, "eth0/8", "10.1.1.4", 24);

    network_node_device_type(router_3, L3_ROUTER);
    network_node_set_loopback_address(router_3, "122.1.1.3");
    network_node_set_interface_ip_address(router_3, "eth0/6", "10.1.1.1", 24);
    
    network_node_device_type(router_4, L3_ROUTER);
    network_node_set_loopback_address(router_4, "122.1.1.4");
    network_node_set_interface_ip_address(router_4, "eth0/7", "10.1.1.3", 24);
    
    network_node_device_type(L2Switch, L2_SWITCH);
    network_node_set_interface_datalink_mode(L2Switch, "eth0/1", ACCESS);
    network_node_set_interface_datalink_mode(L2Switch, "eth0/2", ACCESS);
    network_node_set_interface_datalink_mode(L2Switch, "eth0/3", ACCESS);
    network_node_set_interface_datalink_mode(L2Switch, "eth0/4", ACCESS);

    network_start_packet_reciever_thread(network_topology_3);

    printf("\n\n\n");
    printf("                                            +---------------+\n");
    printf("                                            |               |\n");
    printf("                                            |    router_4   |\n");
    printf("                                            |    122.1.1.4  |\n");
    printf("                                            |               |\n");
    printf("                                            +-------+-------+\n");
    printf("                                                    |eth0/7 - 10.1.1.3/24 \n");
    printf("                                                    |\n");
    printf("                                                    |\n");
    printf("                                                    |eth0/1\n");
    printf("                                            +-------+-------+\n");
    printf("   +---------------+                        |               |                        +---------------+\n");
    printf("   |               |10.1.1.2/24             |               |eth0/2       10.1.1.1/24|               |\n");
    printf("   |    router_1   +------------------------+   L2Switch    +------------------------+    router_3   |\n");
    printf("   |    122.1.1.1  |eth0/5            eth0/4|               |                 eth0/6 |    122.1.1.3  |\n");
    printf("   |               |                        |               |                        |               |\n");
    printf("   +---------------+                        +-------+-------+                        +---------------+\n");
    printf("                                                    |eth0/3 \n");
    printf("                                                    |\n");
    printf("                                                    |\n");
    printf("                                                    |eth0/8 - 10.1.1.4/24\n");
    printf("                                            +-------+-------+\n");
    printf("                                            |               |\n");
    printf("                                            |    router_2   |\n");
    printf("                                            |    122.1.1.2  |\n");
    printf("                                            |               |\n");
    printf("                                            +---------------+\n");
    printf("\n\n\n");

    return network_topology_3;
}
