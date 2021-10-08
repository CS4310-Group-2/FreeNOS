

#ifndef __BIN__RENICE__RENICE_H
#define __BIN__RENICE__RENICE_H

#include <POSIXApplication.h>

/**
 * @addtogroup bin
 * @{
 */

class Renice: public POSIXApplication
{
 public:
    /**
     * Constructor
     * 
     * @param argc Argument count
     * @param argv Argument values
     */
    Renice(int argc, char **argv);

    /**
     * Destructor
     */
    virtual ~Renice();

    /**
     * Executing the application
     * 
     * @return Result code
    */
    virtual Result exec();

};
#endif /*__BIN__RENICE__RENICE_H*/