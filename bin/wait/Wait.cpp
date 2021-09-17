#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Wait.h"


Wait::Wait(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Cause a process to wait for some time");
    parser().registerPositional("SECONDS", "Process waits for certain number of seconds");
}

Wait::~Wait()
{

}

Wait::Result Wait::exec()
{
    
}
