/**
 * Created by TekuConcept on June 12, 2017
 */

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>
#include <vector>
#include <algorithm>
#include "EventCommand.h"

#define EVENT_COMMAND_PTR std::shared_ptr<EventCommand<Args>>

template <class Args>
class EventHandler {
public:
    EventHandler() { }
    EventHandler<Args>& operator += (EVENT_COMMAND_PTR delegate) {
        _delegates_.push_back(delegate);
        return *this;
    }
    EventHandler<Args>& operator -= (EVENT_COMMAND_PTR delegate) {
        typename std::vector<EVENT_COMMAND_PTR>::iterator position =
        std::find(
                _delegates_.begin(),
                _delegates_.end(),
                delegate
        );
        if (position != _delegates_.end())
            _delegates_.erase(position);
        return *this;
    }
    void invoke(Object& sender, Args e) {
        for(auto token : _delegates_)
            token->invoke(sender, e);
    }
    
private:
    std::vector<EVENT_COMMAND_PTR> _delegates_;
};

#endif