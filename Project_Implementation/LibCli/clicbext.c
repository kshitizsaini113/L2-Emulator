#include <stdio.h>
#include <stdlib.h>
#include "clicbext.h"

void
terminate_signal_handler(){

    /*-----------------------------------------------------------------------------
     *  Process has malfunctioned for some reason, user would like to see the memory
     *  state of the process for debugging. This function triggers all the show command
     *  one by one and write the output data to the file for future reference.
     *  -----------------------------------------------------------------------------*/
    collect_supportsave_data();
    exit(0);
}

void
collect_supportsave_data(){
    printf("%s() is called ...\n", __FUNCTION__);
}
