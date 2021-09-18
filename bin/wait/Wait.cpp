#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Wait.h"

int pid;
Wait::Wait(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Cause a process to wait for some time");
    parser().registerPositional("SECONDS", "Process waits for certain number of seconds");
    
    pid = atoi(argv[1]);
}

Wait::~Wait()
{

}

// Wait arg1 , arg2 , arg3
Wait::Result Wait::exec()
{
    int myPID = pid;
    int status;
    printf("%d",myPID);
    waitpid(myPID,&status,0);
    

    // Done
    return Success;
}

