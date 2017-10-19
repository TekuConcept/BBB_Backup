# C#-Style C++ Events

Header-only light-weight C#-style C++ events for object-oriented event-driven programming.

```
// Simple Example
#include "EventHandler.h"
#include "FunctionCommand.h"
typedef EventHandler<EventArgs> DefaultEventHandler;

class Action {
public:
  DefaultEventHandler onEvent;
  void doWork() {
    // do work
    Object obj(typeid(this), this);
    EventArgs e;
    onEvent.invoke(obj, e);
  }
}

class Responder {
public:
  void eventOnAction(Object& sender, EventArgs e) {
    // do more work
  }
}

int main() {
  Action action;
  Responder responder;
  action.onEvent += FunctionCommandPtr(
    Responder,
    EventArgs,
    &responder,
    eventOnAction
  );
  action.doWork();
  return 0;
}
```

Private functions can also be used as event callbacks. (Be careful not to break encapsulation in your project.)
