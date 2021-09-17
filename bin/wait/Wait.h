/**
 * 
 * 
 *modules to check
 *
 *waitpid
 *declaration
 *found in lib/libposix/sys/wait.h
 *waitpid.cpp
 *lib/libposix/sys/wait/waitpid.cpp
 * 
*/

#ifndef __BIN_WAIT_WAIT_H
#define __BIN_WAIT_WAIT_H

#include <POSIXApplication.h>

/**
 * @addtogroup bin
 * @{
 */

class Wait: public POSIXApplication
{
 public:
    /**
     * Constructor
     * 
     * @param argc Argument count
     * @param argv Argument values
     */
    Wait(int argc, char **argv);

    /**
     * Destructor
     */
    virtual ~Wait();

    /**
     * Executing the application
     * 
     * @return Result code
    */
    virtual Result exec();



};

/**
 * @}
 */
#endif /*__BIN_SLEEP_SLEEP_H*/