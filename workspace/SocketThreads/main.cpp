#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <sstream>

#include "SocketHost.h"

class Object {
	std::thread async;
	std::string _name_;
public:
	Object(std::string name);
	~Object();
	void foo();
	static void bar(std::string * ptr);
};

int main() {
	SocketHost host(0);
	SocketHost test(0);

	int nport = host.getLocalPort();
	std::cout << "SERVER: " << nport << std::endl;
	host.startListening();

	nport = test.getLocalPort();
	std::cout << "SERVER2: " << nport << std::endl;
	test.startListening();
	//Object objA("objA");
	//Object objB("objB");

	//objA.foo();
	//objB.foo();

	std::cout << "- END OF LINE -" << std::endl;
}

Object::Object(std::string name) : _name_(name) { }

Object::~Object() {
	std::cout << "destructor called" << std::endl;
	if(async.joinable())
		async.join();
}

void Object::foo() {
	// TODO: Add flag to prevent creating another async thread
	std::string * s_ptr = &_name_;
	std::cout << _name_ << ": foo() called" << std::endl;
	async = std::thread([](std::string * ptr){
		std::cout << *ptr << ": running anonymous child thread" << std::endl;
		std::vector<std::thread> children;
		for(int i = 0; i < 3; i++) {
			std::stringstream ss;
			ss << *ptr << "_" << i;
			std::string * line = new std::string(ss.str().c_str());
			children.push_back(std::thread(&Object::bar, line));
		}
		for(int i = 0; i < children.size(); i++)
			children[i].join();
	}, s_ptr);
}

void Object::bar(std::string * ptr) {
	std::cout << *ptr << ": bar() called" << std::endl;
	delete ptr;
}