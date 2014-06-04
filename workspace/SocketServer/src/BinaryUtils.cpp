/*
 * BinaryUtils.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: tekuconcept
 */

#include "BinaryUtils.h"

void* BinaryUtils::memset(void* b, int c, size_t len) {
    char* p = (char*)b;
    for (size_t i = 0; i != len; ++i) {
        p[i] = c;
    }
    return b;
}


void* BinaryUtils::memcpy(void* dst, const void* src, size_t count) {
	void * ret = dst;
	while(count--) {
		*(char *)dst = *(char *)src;
		dst = (char *)dst + 1;
		src = (char *)src + 1;
	}
	return(ret);
}


bool BinaryUtils::getBit(char c, int idx)
{
	return (c >> idx) & 1;
}


void BinaryUtils::setBit(char &c, int idx)
{
	c |= 1 << idx;
}


void BinaryUtils::unsetBit(char &c, int idx)
{
	c &= ~(1 << idx);
}


void BinaryUtils::toggleBit(char &c, int idx)
{
	c ^= 1 << idx;
}


bool BinaryUtils::exists(std::string path)
{
	return (access(path.c_str(), F_OK) != -1);
}


std::string BinaryUtils::ssystem(const char* cmd) {
	FILE* pipe = popen(cmd, "r");
	if(!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

