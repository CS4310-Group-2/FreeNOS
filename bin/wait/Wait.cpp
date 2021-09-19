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
    parser().registerPositional("PID", "Process waits for certain Process given by PID");
    
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
    printf("Waiting on PID %d\n",myPID);
    pid_t wait_call_status = waitpid(myPID,&status,0);

    if(wait_call_status!=myPID){
        printf("Wait Failed\n");
        return TimedOut;

    }
    // Done
    printf("Wait was a success\n");
    return Success;
}

