#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Renice.h"

Renice::Renice(int argc, char **argv)
      : POSIXApplication(argc,argv)
{
      parser().setDescription("Changes the priority level of a ");

}

Renice::~Renice()
{

}

Renice::Result Renice::exec()
{
    printf("running renice");
    return Success;
}