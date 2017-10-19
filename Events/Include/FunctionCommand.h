/**
 * Created by TekuConcept on June 12, 2017
 */

#ifndef FUNCTION_COMMAND_H
#define FUNCTION_COMMAND_H

#include <memory>
#include "EventCommand.h"

template <class Receiver, class Args>
class FunctionCommand : public EventCommand<Args> {
public:
    typedef void (Receiver::* Action)(Object& sender, Args e);
    
    FunctionCommand(Receiver* r, Action a);
    
    virtual void invoke(Object& sender, Args e) {
        (_receiver_->*_action_)(sender, e);
    }
    
private:
    Action _action_;
    Receiver* _receiver_;
};

template <class Receiver, class Args>
FunctionCommand<Receiver, Args>::FunctionCommand(Receiver* r, Action a) :
    _action_(a), _receiver_(r) {};

#define FunctionCommandPtr(c, e, o, f) std::make_shared<FunctionCommand<c,e>>(&o,&c::f)

#endif