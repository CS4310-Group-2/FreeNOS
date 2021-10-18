#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Renice.h"
#include <ProcessClient.h>

Renice::Renice(int argc, char **argv)
      : POSIXApplication(argc,argv)
{
      parser().setDescription("Changes the priority level of a Process");
      parser().registerPositional("PRIORITY","The priority that the process will change to.");
      parser().registerPositional("PID","Changes the priority level of a given.");
      parser().registerFlag('n',"new_priority_level","The new priority level of the process");
      parser().registerFlag('l',"list_of_priorities","Displays the list of available priorites to change to.");
}

Renice::~Renice()
{

}

Renice::Result Renice::exec()
{
    const ProcessClient process;

    // printf("running renice\n");
    int PID = atoi(arguments().get("PID"));
    const int PRIORITY = atoi(arguments().get("PRIORITY"));

    if(PRIORITY<1||PRIORITY>5)
    {
          ERROR("invalid Priority `" << arguments().get("PRIORITY") << "'");
          return InvalidArgument;
    }
    process.RenicePID(PID,PRIORITY);
    printf("The new Priority of process %d is %d\n",PID,PRIORITY);
    return Success;
}
