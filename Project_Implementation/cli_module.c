#include "networkgraph.h"
#include <stdio.h>
#include "LibCli/libcli.h"
#include "LibCli/cmdtlv.h"
#include "commandcodes.h"

extern network_graph_t *network_topology;


void display_graph_nodes(param_t *param, ser_buff_t *tlv_buf)
{
    network_node_t *network_node;
    doublylinkedlist_t *current;

    ITERATE_DOUBLY_LINKED_LIST_BEGINING(&network_topology->network_node_list, current)
    {
        network_node = graph_glue_to_node(current);
        printf("%s\n", network_node->network_node_name);
    } ITERATE_DOUBLY_LINKED_LIST_END(&network_topology->network_node_list, current);
}


int validate_node_existance(char *network_node_name)
{
    network_node_t *network_node = get_network_node_by_node_name(network_topology, network_node_name);
    if(network_node)
    {
        return VALIDATION_SUCCESS;
    }
    printf("ERROR : NODE %s doesn't exists.\n", network_node_name);
}


int validate_l2_mode_value(char *l2_mode_value)
{
    if((strncmp(l2_mode_value, "access", strlen("access")) == 0) || 
       (strncmp(l2_mode_value, "trunk", strlen("trunk")) == 0))
    {
        return VALIDATION_SUCCESS;
    }

    return VALIDATION_FAILED;
}


static int show_network_topology_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
    int CMDCODE = -1;
    CMDCODE = EXTRACT_CMD_CODE(tlv_buf);

    switch(CMDCODE)
    {
        case CMDCODE_SHOW_NW_TOPOLOGY:
            dump_network_graph_net(network_topology);
            break;

        default:
            break;
    }
}


typedef struct arp_table_ arp_table_t;

extern void dump_arp_table(arp_table_t *arp_table);

static int show_arp_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
    network_node_t *network_node;
    char *node_name;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv)
    {
        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) == 0)
        {
            node_name = tlv->value;
        }
    } TLV_LOOP_END;

    network_node = get_network_node_by_node_name(network_topology, node_name);
    dump_arp_table(NODE_ARP_TABLE(network_node));

    return 0;
}

typedef struct mac_table_ mac_table_t;

extern void dump_mac_table(mac_table_t *mac_table);

static int show_mac_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
    network_node_t *network_node;
    char *node_name;
    tlv_struct_t *tlv = tlv->value;

    TLV_LOOP_BEGIN(tlv_buf, tlv)
    {
        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
        {
            node_name = tlv->value;
        }
    } TLV_LOOP_END;

    network_node = get_network_node_by_node_name(network_topology, node_name);
    dump_mac_table(NODE_MAC_TABLE(network_node));

    return 0;
}


extern void send_arp_broadcast_request(network_node_t *network_node, network_interface_t *outgoing_network_interface, char *ip_address);


static int arp_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
    network_node_t *network_node;
    char *node_name;
    char *ip_address;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv)
    {
        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;
        else if(strncmp(tlv->leaf_id, "ip-address", strlen("ip-address")) ==0)
            ip_address = tlv->value;
    } TLV_LOOP_END;

    network_node = get_network_node_by_node_name(network_topology, node_name);
    send_arp_broadcast_request(network_node, NULL, ip_address);
    return 0;
}


extern void network_interface_set_l2_node(network_node_t *network_node, network_interface_t *network_interface, char *layer2_mode_operation);


static int intf_config_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable){

   char *node_name;
   char *intf_name;
   char *l2_mode_option;
   int CMDCODE;
   tlv_struct_t *tlv = NULL;
   network_node_t *network_node;
   network_interface_t *network_interface;

   CMDCODE = EXTRACT_CMD_CODE(tlv_buf);
   
    TLV_LOOP_BEGIN(tlv_buf, tlv)
    {

        if (strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
        {
            node_name = tlv->value;
        }
        else if(strncmp(tlv->leaf_id, "if-name", strlen("if-name")) ==0)
        {
            intf_name = tlv->value;
        }
        else if(strncmp(tlv->leaf_id, "l2-mode-val", strlen("l2-mode-val")) == 0)
        {
            l2_mode_option = tlv->value;
        }
        else
        {
            assert(0);
        }
    } TLV_LOOP_END;

    network_node = get_network_node_by_node_name(network_topology, node_name);
    network_interface = get_network_node_interface_by_name(network_node, intf_name);

    if(!network_interface)
    {
        printf("ERROR : Interface %s do not exist\n", network_interface->interface_name);
        return -1;
    }

    switch(CMDCODE)
    {
        case CMDCODE_INTF_CONFIG_L2_MODE:
            switch(enable_or_disable)
            {
                case CONFIG_ENABLE:
                    network_interface_set_l2_node(network_node, network_interface, l2_mode_option);
                    break;
                default:
                    break;
            }
            break;    
    }
    return 0;
}


void initialize_network_cli()
{
    init_libcli();

    param_t *show   = libcli_get_show_hook();  //predefined
    param_t *debug  = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *run    = libcli_get_run_hook();
    param_t *debug_show = libcli_get_debug_show_hook();
    param_t *root = libcli_get_root();

    {
        static param_t topology;   
        init_param(&topology, CMD, "topology", show_network_topology_handler, 0, INVALID, 0, "Dump Complete Network Topology");
        libcli_register_param(show, &topology); 
        set_param_cmd_code(&topology, CMDCODE_SHOW_NW_TOPOLOGY);

        {
            static param_t node;
            init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
            libcli_register_param(show, &node);
            libcli_register_display_callback(&node, display_graph_nodes);
            {
               /*show node <node-name>*/ 
                static param_t node_name;
                init_param(&node_name, LEAF, 0, 0, validate_node_existance, STRING, "node-name", "Node Name");
                libcli_register_param(&node, &node_name);
                {
                   /*show node <node-name> arp*/
                   static param_t arp;
                   init_param(&arp, CMD, "arp", show_arp_handler, 0, INVALID, 0, "\"arp\" keyword");
                   libcli_register_param(&node_name, &arp);
                   set_param_cmd_code(&arp, CMDCODE_SHOW_NODE_ARP_TABLE);
                }
                {
                   /*show node <node-name> mac*/
                   static param_t mac;
                   init_param(&mac, CMD, "mac", show_mac_handler, 0, INVALID, 0, "Dump Mac table");
                   libcli_register_param(&node_name, &mac);
                   set_param_cmd_code(&mac, CMDCODE_SHOW_NODE_MAC_TABLE);
                }      
            }
        }
    }

    {
        /*run node*/
        static param_t node;
        init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
        libcli_register_param(run, &node);
        libcli_register_display_callback(&node, display_graph_nodes);
        {
            /*run node <node-name>*/
            static param_t node_name;
            init_param(&node_name, LEAF, 0, 0, validate_node_existance, STRING, "node-name", "Node Name");
            libcli_register_param(&node, &node_name);
            {
                /*run node <node-name> ping */
                static param_t ping;
                init_param(&ping, CMD, "ping" , 0, 0, INVALID, 0, "Ping utility");
                libcli_register_param(&node_name, &ping);
                {
                    /*run node <node-name> ping <ip-address>*/    
                    static param_t ip_addr;
                    init_param(&ip_addr, LEAF, 0, "resolve", 0, IPV4, "ip-address", "Ipv4 Address");
                    libcli_register_param(&ping, &ip_addr);
                    set_param_cmd_code(&ip_addr, CMDCODE_PING);
                }
            }
            {
                /*run node <node-name> resolve-arp*/    
                static param_t resolve_arp;
                init_param(&resolve_arp, CMD, "resolve", 0, 0, INVALID, 0, "Resolve ARP");
                libcli_register_param(&node_name, &resolve_arp);
                {
                    /*run node <node-name> resolve-arp <ip-address>*/    
                    static param_t ip_addr;
                    init_param(&ip_addr, LEAF, 0, arp_handler, 0, IPV4, "ip-address", "Nbr IPv4 Address");
                    libcli_register_param(&resolve_arp, &ip_addr);
                    set_param_cmd_code(&ip_addr, CMDCODE_RUN_ARP);
                }
            }
        }
    }

    {
        static param_t node;
      init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
      libcli_register_param(config, &node);  
      libcli_register_display_callback(&node, display_graph_nodes);
      {
        /*config node <node-name>*/
        static param_t node_name;
        init_param(&node_name, LEAF, 0, 0, validate_node_existance, STRING, "node-name", "Node Name");
        libcli_register_param(&node, &node_name);
        {
            /*config node <node-name> interface*/
            static param_t interface;
            init_param(&interface, CMD, "interface", 0, 0, INVALID, 0, "\"interface\" keyword");    
            libcli_register_param(&node_name, &interface);
            {
                /*config node <node-name> interface <if-name>*/
                static param_t if_name;
                init_param(&if_name, LEAF, 0, 0, 0, STRING, "if-name", "Interface Name");
                libcli_register_param(&interface, &if_name);
                {
                    /*config node <node-name> interface <if-name> l2mode*/
                    static param_t l2_mode;
                    init_param(&l2_mode, CMD, "l2mode", 0, 0, INVALID, 0, "\"l2mode\" keyword");
                    libcli_register_param(&if_name, &l2_mode);
                    {
                        /*config node <node-name> interface <if-name> l2mode <access|trunk>*/
                        static param_t l2_mode_val;
                        init_param(&l2_mode_val, LEAF, 0, intf_config_handler, validate_l2_mode_value,  STRING, "l2-mode-val", "access|trunk");
                        libcli_register_param(&l2_mode, &l2_mode_val);
                        set_param_cmd_code(&l2_mode_val, CMDCODE_INTF_CONFIG_L2_MODE);
                    } 
                }
            }
            
        }    
        support_cmd_negation(&node_name);
      }
    }
    support_cmd_negation(config);
}

