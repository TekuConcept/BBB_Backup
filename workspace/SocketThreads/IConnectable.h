#ifndef _ICONNECTABLE_
#define _ICONNECTABLE_

class IConnectable {
public:
    virtual int getLocalPort() = 0;
};

#endif
