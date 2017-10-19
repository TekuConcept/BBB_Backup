/**
 * Created by TekuConcept on June 12, 2017
 */

#include <iostream>
#include "Object.h"

#define DMSG(x) std::cout << x << std::endl

class Dummy {
    int x;
public:
    Dummy(int arg) : x(arg) { }
    int getX() { return x; }
};

int main() {
    DMSG("- OBJECT DEMO -");
    
    Dummy dummy(2);
    Object obj(typeid(dummy), &dummy);
    
    if(obj.getType() == typeid(Dummy)) {
        DMSG("Types are identical!");
        Dummy* ref = static_cast<Dummy*>(obj.data());
        DMSG("Dummy value is " << ref->getX());
    }
    
    DMSG("- DEMO FINISHED -");
    return 0;
}