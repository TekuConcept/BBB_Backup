/**
 * Created by TekuConcept on June 12, 2017
 */

#include <iostream>
#include <EventCommand.h>

#define DMSG(x) std::cout << x << std::endl

class CustomArgs : public EventArgs {
public:
    CustomArgs() : X(0) {}
    CustomArgs(int val) : X(val) {}

    int X;
};

template <class Args>
class DummyEventCommand : public EventCommand<Args> {
public:
    void invoke(object& sender, Args e) {
        DMSG("Event executed!");
        if(typeid(Args) == typeid(CustomArgs)) {
            DMSG("Event arguments are { x: " << e.X << " }");
        }
    }
};

void testEvent(EventCommand<CustomArgs> &event) {
    Object obj(typeid(int), NULL);
    CustomArgs e(2);
    event.invoke(obj, e);
}

int main() {
    DMSG("- COMMAND DEMO -");
    
    DummyEventCommand<CustomArgs> dummy;
    testEvent(dummy);
    
    DMSG("- DEMO FINISHED -");
    return 0;
}