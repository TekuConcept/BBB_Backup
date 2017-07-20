/**
 * Created by TekuConcept on June 12, 2017
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <typeinfo>

class Object {
    const std::type_info& _type_;
    void* _data_;
public:
    Object(const std::type_info& type, void* data)
        : _type_(type), _data_(data) { }
    
    const std::type_info& getType() {
        return _type_;
    }
    
    void* data() {
        return _data_;
    }
    
    // static Object empty() {
    //     Object obj(NULL, NULL);
    //     return obj;
    // }
};
typedef Object object;

#endif