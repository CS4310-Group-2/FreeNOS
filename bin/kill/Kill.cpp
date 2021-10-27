#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Kill.h"
#include "signal.h"

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
    int pid = atoi(arguments().get("PID"));
    int result = kill(pid,SIGKILL);

    if(result != 0)
    {
        printf("The kill had failed\nThe result given from kill.cpp was %d\n",result);
        return NotFound;
    }


    printf("Process %d was killed\n",pid);
    return Success;

}