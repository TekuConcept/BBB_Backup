function edit_bash {
  nano ~/.bashrc
}

function edit_network {
  nano /etc/network/interfaces
}

SLOTS=/sys/devices/bone_capemgr.*/slots
PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins
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

