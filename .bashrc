function edit_bash {
  nano ~/.bashrc
}

function edit_network {
  nano /etc/network/interfaces
}

SLOTS=/sys/devices/bone_capemgr.*/slots
PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins
GROUPS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pingroups
FIRM=/lib/firmware


function search_desk {
  cd /home/debian/Desktop
}

function comp_c {
  # g++ [fileA.c] [fileB.c] [etc] -o [Program]
  # g++ -c [file.c] => file.o
  # g++ [fileA.o] [fileB.o] [etc] -o [Program]
  g++ $1.c -o $1
}

# use to read register address (once)
# "> readm 0x48304000 10"
function readm {
  /home/debian/Desktop/CODE/memory/./main -r $1 $2 0 1
}

# use to write register address (once)
# "> writem 0x48304000 214 61A8"
function writem {
  /home/debian/Desktop/CODE/memory/./main -w $1 $2 $3 1
}

# use to quickly load i2c-2 then access registers through bash
# (script.sh uses functions readm and writem)
function load_i2c {
        echo BB-I2C1 > $SLOTS
        source /home/debian/Desktop/CODE/i2c/script.sh
        i2c2_enable
}

# use to quickly start and stop LED blink state
function LED {
        case $1 in
                '0') echo none > /sys/devices/ocp.3/gpio-leds.8/leds/beaglebone\:green\:usr0/trigger; ;;
                '1') echo heartbeat > /sys/devices/ocp.3/gpio-leds.8/leds/beaglebone\:green\:usr0/trigger; ;;
                *) echo "Unsupported Option"; ;;
        esac
}

