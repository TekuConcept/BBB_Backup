/*
 * ThrusterControl.h
 *
 *  Created on: Mar 21, 2014
 *      Author: tekuconcept
 */

#ifndef THRUSTER
#define THRUSTER

#include "BinaryUtils.h"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>

class ThrusterControl {
public:
	// sets up the environment for motor control
	static void initialize();
	// given a binary digest, will control and manipulate attached devices
	// feature includes: Reset, Cruise, & GRT-Control
	static void processData(char* data, int size);
	// arms ESCs - provides guaranteed control of thrusters
	static void arm();

private:
	static const int PERIOD = 2000000;
};

#endif
