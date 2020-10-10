#include "networkgraph.h"
#include <stdio.h>
#include "LibCli/libcli.h"
#include "LibCli/cmdtlv.h"
#include "commandcodes.h"

extern network_graph_t *network_topology;


/*Generic Topology Commands*/
static int
show_network_topology_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable){

    int CMDCODE = -1;
    CMDCODE = EXTRACT_CMD_CODE(tlv_buf);

    switch(CMDCODE){

        case CMDCODE_SHOW_NW_TOPOLOGY:
            dump_network_graph(network_topology);
            break;
        default:
            ;
    }
}



void
initialize_network_cli(){

    init_libcli();

    param_t *show   = libcli_get_show_hook();  //predefined
    param_t *debug  = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *run    = libcli_get_run_hook();
    param_t *debug_show = libcli_get_debug_show_hook();
    param_t *root = libcli_get_root();

    {
        /*show topology*/
         static param_t topology;
         init_param(&topology, //refrence to the topology
                    CMD, //command parameter
                    "topology", //keyword given to specify a command parameter,must be meaningful
                    show_network_topology_handler,  //application routine to coordinate commands with the project
                    0, INVALID, 0, //null for command parameter.used in leaf params.
                    "Dump Complete Network Topology");
         libcli_register_param(show, &topology); //attawching node "topology" with node "show" in a tree
         set_param_cmd_code(&topology, CMDCODE_SHOW_NW_TOPOLOGY);    //unique ID is given to the command
    }
 
 

    support_cmd_negation(config);
}


