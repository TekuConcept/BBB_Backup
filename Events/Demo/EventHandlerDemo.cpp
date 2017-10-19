/**
 * Created by TekuConcept on June 12, 2017
 */

#include <iostream>
#include <memory>
#include "EventHandler.h"

#define DMSG(x) std::cout << x << std::endl

class DummyCommandA : public EventCommand<EventArgs> {
public:
    void invoke(Object& sender, EventArgs e) {
        DMSG("Dummy Command A Executed!");
    }
};

class DummyCommandB : public EventCommand<EventArgs> {
public:
    void invoke(Object& sender, EventArgs e) {
        DMSG("Dummy Command B Executed!");
    }
};

void testEvent(EventHandler<EventArgs> handler) {
    Object obj(typeid(int), NULL);
    EventArgs e;
    handler.invoke(obj, e);
}

int main() {
    DMSG("- EVENT HANDLER DEMO -");
    
    EventHandler<EventArgs> handler;
    handler += std::make_shared<DummyCommandA>();
    handler += std::make_shared<DummyCommandB>();
    testEvent(handler);
    
    DMSG("- DEMO FINISHED -");
    return 0;
}