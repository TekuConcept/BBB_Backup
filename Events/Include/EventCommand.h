/**
 * Created by TekuConcept on June 12, 2017
 */

#ifndef EVENT_COMMAND_H
#define EVENT_COMMAND_H

#include "Object.h"
#include "EventArgs.h"

template <class Args>
class EventCommand {
    static_assert(std::is_base_of<EventArgs, Args>::value, "Args must derive from EventArgs");
public:
    virtual void invoke(Object& sender, Args e) = 0;
    virtual ~EventCommand() {}
};

#endif