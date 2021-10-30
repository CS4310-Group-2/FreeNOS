#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Kill.h"
#include "signal.h"
#include <ProcessClient.h>

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
        ProcessClient::Info info;
        const ProcessClient process;
        const ProcessClient::Result result = process.processInfo(pid, info);
        if(result != ProcessClient::Success)
        {
          ERROR("Process " << pid << " was not found.")
          return InvalidArgument;
        }
        if (info.textState != "Running")
        {

          int wasKilled = kill(pid,SIGKILL);
          if(wasKilled == 0){//Kill returned 0, success.
          printf("Process %d was successfully killed.\n", pid);
          return Success;
          }else{//Kill returned -1, failure.
          ERROR("Process " << pid << " was not killed.");
          return InvalidArgument;
          }
        }else{

            ERROR("Kill cannot be used on itself.");
            return InvalidArgument;
        }
        /*
    int result = kill(pid,SIGKILL);
    if(result == 0){//Kill returned 0, success.
       printf("Process %d was successfully killed.\n", pid);
       return Success;
    }else{//Kill returned -1, failure.
      ERROR("Process " << pid << " was not killed.");
      return InvalidArgument;
    }*/
}
