/**
 * Created by TekuConcept on June 12, 2017
 */

#include <iostream>
#include <memory>
#include "FunctionCommand.h"

#define DMSG(x) std::cout << x << std::endl
#define EVENT_PTR std::shared_ptr<FunctionCommand<Sender, CustomArgs>>
#define EVENT_MK std::make_shared<FunctionCommand<Sender, CustomArgs>>

class CustomArgs : public EventArgs {
public:
    CustomArgs() : X(0) {}
    CustomArgs(int val) : X(val) {}
    int X;
};

class Sender {
    void privateListener(object& sender, CustomArgs e) { DMSG("Private function executed!"); }
public:
    void publicListener(Object& sender, CustomArgs e) { DMSG("Public function executed!"); }
    EVENT_PTR getListener() {
        return EVENT_MK(this, &Sender::privateListener);
    }
};

void testEvent(EventCommand<CustomArgs> &event) {
    Object obj(typeid(int), NULL);
    CustomArgs e(2);
    event.invoke(obj, e);
}

int main() {
    DMSG("- ADVANCED COMMAND DEMO -");
    
    Sender sender;
    FunctionCommand<Sender, CustomArgs> event1(&sender, &Sender::publicListener);
    EVENT_PTR event2 = sender.getListener();
    
    testEvent(event1);
    testEvent(*event2);
    
    DMSG("- DEMO FINISHED -");
    return 0;
}