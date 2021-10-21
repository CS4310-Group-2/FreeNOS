#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Kill.h"


Kill::Kill(int argc, char **argv): POSIXApplication(argc,argv)
{
    parser().setDescription("Kills a process of a given PID");
    parser().registerPositional("PID", "The Process ID that is to be killed");

}


Kill::~Kill()
{
}


Kill::Result Kill::exec()
{
    printf("The process killed was %d\n",atoi(arguments().get("PID")));

}