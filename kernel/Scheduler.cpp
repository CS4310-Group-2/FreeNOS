/*
 * Copyright (C) 2015 Niek Linnenbank
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <Log.h>
#include "Kernel.h"
#include "Scheduler.h"
#include "Process.h"


Scheduler::Scheduler()
{
    DEBUG("");
}

Size Scheduler::count() const
{
    return m_queue.count();
}


/**
 * Enqueing based on Priority Level
 * 
 * Use a switch case to determine the priority level
 * instead of if else
*/
Scheduler::Result Scheduler::enqueue(Process *proc, bool ignoreState)
{
    //ERROR("Is it executing here?");
    if (proc->getState() != Process::Ready && !ignoreState)
    {
        ERROR("process ID " << proc->getID() << " not in Ready state");
        return InvalidArgument;
    }

    //m_queue.push(proc);
    PriorityLevel priorityLvl = proc->getPriorityLevel();
    switch (priorityLvl)
    {

    case Process::Min:

        min_queue.push(proc);
        break;
    case Process::Lower:

        lower_queue.push(proc);
        break;
    case Process::Default:

        default_queue.push(proc);
        break;
    case Process::Higher:

        higher_queue.push(proc);
        break;

    case Process::Max:

        max_queue.push(proc);
        break;

    default:

        break;
    }

    return Success;
    
}

Scheduler::Result Scheduler::dequeue(Process *proc, bool ignoreState)
{
    if (proc->getState() == Process::Ready && !ignoreState)
    {
        ERROR("process ID " << proc->getID() << " is in Ready state");
        return InvalidArgument;
    }
    

    Size min_count = min_queue.count();
    Size lower_count = lower_queue.count();
    Size def_count = default_queue.count();
    Size high_count = higher_queue.count();
    Size max_count = max_queue.count();

    

        // Traverse the Queue to remove the Process
    for (Size i = 0; i < min_count; i++)
    {
        Process *p = min_queue.pop();
        if (p == proc)
        {
            return Success;
        }

        else
            min_queue.push(p);
    }

            // Traverse the Queue to remove the Process
    for (Size i = 0; i < lower_count; i++)
    {
        Process *p = lower_queue.pop();
        if (p == proc)
        {
            return Success;
        }

        else
            lower_queue.push(p);
    }

            // Traverse the Queue to remove the Process
    for (Size i = 0; i < def_count; i++)
    {
        //ERROR("Trying to dequeue default")
        Process *p = default_queue.pop();
        if (p == proc)
        {
            return Success;
        }

        else
            default_queue.push(p);
    }

            // Traverse the Queue to remove the Process
    for (Size i = 0; i < high_count; i++)
    {
        Process *p = higher_queue.pop();
        if (p == proc)
        {
            return Success;
        }

        else
            higher_queue.push(p);
    }

            // Traverse the Queue to remove the Process
    for (Size i = 0; i < max_count; i++)
    {
        Process *p = max_queue.pop();
        if (p == proc)
        {
            return Success;
        }

        else
            max_queue.push(p);
    }
        /*

        if (p == proc)
                return Success;
            else
                m_queue.push(p);
        }*/
    
    /**
     * dequeing based on Priority Level
     * 
     * Use a switch case to determine the priority level
     * instead of if else
     * 
     * 
            
    */
    

    FATAL("process ID " << proc->getID() << " is not in the schedule");
    return InvalidArgument;
}

/*
    Void Scheduler::Organize
    Take the queue items
    Organize the processes by PriorityLevel 
    Process.getPriorityLevel,
    Sort with info
*/

Process * Scheduler::select()
{
    if(max_queue.count()>0){
        Process *p = max_queue.pop();
        max_queue.push(p);
        return p;
    }
    else if(higher_queue.count()>0){
        Process *p = higher_queue.pop();
        higher_queue.push(p);
        return p;       
    }
    else if(default_queue.count()>0){
        Process *p = default_queue.pop();
        default_queue.push(p);
        return p;       
    }
    else if(lower_queue.count()>0){
        Process *p = lower_queue.pop();
        lower_queue.push(p);
        return p;       
    }
    else if(min_queue.count()>0){
        Process *p = min_queue.pop();
        min_queue.push(p);
        return p;        
    }


    return (Process *) NULL;
}
    /**
     * if (max_queue > 0)
     *     return head of max queue
     * if (higher > 0)
     *     return head of max queue
     * ....
     * if (min >0)
     *     return head of min queue
     * 
    *     else(m_queue.count()>0){
        Process *p = m_queue.pop();
        m_queue.push(p);
        return p;
    }
    */