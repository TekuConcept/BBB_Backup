/*
 * BinaryUtils.h
 *
 *  Created on: Mar 21, 2014
 *      Author: tekuconcept
 */

#ifndef BINARY_UTILS
#define BINARY_UTILS

#include <unistd.h>
#include <cstdio>
#include <string>

class BinaryUtils {
public:
	// formats object's memory with given value
	static void* memset(void* b, int c, size_t len);
	// copies memory from one object into another object
	static void* memcpy(void* dst, const void* src, size_t count);
	// gets the bit value of 'c' at a given index 'idx'
	static bool getBit(char c, int idx);
	// sets the bit value of 'c' at a given index 'idx' to 1
	static void setBit(char &c, int idx);
	// un-sets the bit value of 'c' at a given index 'idx' to 0
	static void unsetBit(char &c, int idx);
	// toggles the bit value of 'c' at a given index 'idx'
	static void toggleBit(char &c, int idx);
	// tests if given file path exists on the current system
	static bool exists(std::string path);
	// runs a shell command 'cmd' and returns the echo'ed value
	static std::string ssystem(const char* cmd);
};

#endif /* BINARYUTILS_H_ */
