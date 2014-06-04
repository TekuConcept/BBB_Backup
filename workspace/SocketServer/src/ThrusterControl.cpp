/*
 * ThrusterControl.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: tekuconcept
 */

#include "ThrusterControl.h"

int err = 0;

void ThrusterControl::initialize()
{
	// set up environment for first time (pwm, spi, i2c)
	err = system("echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots;");
	sleep(1);

	std::ostringstream sos("");
	// create gpio pins
	if(!BinaryUtils::exists("/sys/class/gpio/gpio30/value"))
	{
		sos << "echo 30 > /sys/class/gpio/export;";
		sos << "echo low > /sys/class/gpio/gpio30/direction;";
	}

	if(!BinaryUtils::exists("/sys/class/gpio/gpio31/value"))
	{
		sos << "echo 31 > /sys/class/gpio/export;";
		sos << "echo low > /sys/class/gpio/gpio31/direction;";
	}

	if(!BinaryUtils::exists("/sys/class/gpio/gpio48/value"))
	{
		sos << "echo 48 > /sys/class/gpio/export;";
		sos << "echo low > /sys/class/gpio/gpio48/direction;";
	}

	if(!BinaryUtils::exists("/sys/class/gpio/gpio60/value"))
	{
		sos << "echo 60 > /sys/class/gpio/export;";
		sos << "echo low > /sys/class/gpio/gpio60/direction;";
	}
	err = system(sos.str().c_str());
	sos.flush();



	// create pwm pins
	if(!BinaryUtils::exists("/sys/devices/ocp.3/pwm_test_P8_13.15/period"))
	{
		err = system("echo bone_pwm_P8_13 > /sys/devices/bone_capemgr.9/slots;");
		// set pwm period and default duty cycle (2ms)
		sos << "echo " << PERIOD << " > /sys/devices/ocp.3/pwm_test_P8_13.15/period;";
		sos << "echo " << PERIOD << " > /sys/devices/ocp.3/pwm_test_P8_13.15/duty;";
		err = system(sos.str().c_str());
		sos.flush();
	}
	if(!BinaryUtils::exists("/sys/devices/ocp.3/pwm_test_P9_14.16/period"))
	{
		err = system("echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots;");
		sos << "echo " << PERIOD << " > /sys/devices/ocp.3/pwm_test_P9_14.16/period;";
		sos << "echo " << PERIOD << " > /sys/devices/ocp.3/pwm_test_P9_14.16/duty;";
		err = system(sos.str().c_str());
	}

}

void ThrusterControl::processData(char* data, int size)
{
	static std::ofstream gpio30("/sys/class/gpio/gpio30/value");
	static std::ofstream gpio31("/sys/class/gpio/gpio31/value");
	static std::ofstream gpio48("/sys/class/gpio/gpio48/value");
	static std::ofstream gpio60("/sys/class/gpio/gpio60/value");
	static std::ofstream pwm13("/sys/devices/ocp.3/pwm_test_P8_13.15/duty");
	static std::ofstream pwm14("/sys/devices/ocp.3/pwm_test_P9_14.16/duty");

	gpio30.seekp(0, gpio30.beg);
	gpio31.seekp(0, gpio31.beg);
	gpio48.seekp(0, gpio48.beg);
	gpio60.seekp(0, gpio60.beg);
	pwm13.seekp(0, pwm13.beg);
	pwm14.seekp(0, pwm14.beg);

	gpio30 << BinaryUtils::getBit(data[0], 0);
	gpio31 << BinaryUtils::getBit(data[0], 1);
	gpio48 << BinaryUtils::getBit(data[0], 2);
	gpio60 << BinaryUtils::getBit(data[0], 3);

	// pwm data
	bool cf = BinaryUtils::getBit(data[0], 4),
		 zf = BinaryUtils::getBit(data[0], 5);
	float xd, yd;

	if(zf && !cf) // reset duty cycles (stop / 0)
	{
		xd = *reinterpret_cast<float*>(&data[1]);
		yd = *reinterpret_cast<float*>(&data[5]);

		pwm13 << PERIOD - (int)(PERIOD * std::min(1.0f, std::abs(xd)));
		pwm14 << PERIOD - (int)(PERIOD * std::min(1.0f, std::abs(yd)));
	}
	else if(zf && cf) // set respective duty cycles
	{
		xd = 0.0F;
		yd = 0.0F;

		pwm13 << PERIOD;
		pwm14 << PERIOD;
	}

	gpio30.flush();
	gpio31.flush();
	gpio48.flush();
	gpio60.flush();
	pwm13.flush();
	pwm14.flush();
}

void ThrusterControl::arm()
{
	// ToDo: Arm ESC to provide official control to thrusters
}
