# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
# Commands were specifically used to build a custom debian-based linux system
# for the HiSilicon Hi3516CV500 IP-Camera chipset. Nevertheless, many of the
# commands and other techniques may be applied to other systems as well.
#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



sudo apt install g++-8-arm-linux-gnueabihf
sudo ln -s arm-linux-gnueabihf-cpp-8 arm-linux-gnueabihf-cpp
sudo ln -s arm-linux-gnueabihf-g++-8 arm-linux-gnueabihf-g++
sudo ln -s arm-linux-gnueabihf-gcc-8 arm-linux-gnueabihf-gcc
sudo ln -s arm-linux-gnueabihf-gcc-ar-8 arm-linux-gnueabihf-gcc-ar
sudo ln -s arm-linux-gnueabihf-gcc-nm-8 arm-linux-gnueabihf-gcc-nm
sudo ln -s arm-linux-gnueabihf-gcc-ranlib-8 arm-linux-gnueabihf-gcc-ranlib
sudo ln -s arm-linux-gnueabihf-gcov-8 arm-linux-gnueabihf-gcov
sudo ln -s arm-linux-gnueabihf-gcov-dump-8 arm-linux-gnueabihf-gcov-dump
sudo ln -s arm-linux-gnueabihf-gcov-tool-8 arm-linux-gnueabihf-gcov-tool



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# build uboot
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# get uboot source for HiSilicon chipset
cd u-boot-2016.11
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- hi3516cv500_defconfig

# in ./board/hisilicon/hi3516cv500/hi3516cv500.c
#
# wrap existing code in function board_mmc_init() with
# "#ifdef CONFIG_SUPPORT_EMMC_BOOT" then add the following snippet
#
#    #elif defined(CONFIG_SD_BOOT)
#	dev_num = 1;
#	extern int himci_add_port(int index, u32 reg_base, u32 freq);
#	ret = himci_add_port(dev_num, SDIO0_REG_BASE, CONFIG_SDIO0_FREQ);
#	if (!ret) {
#		ret = himci_probe(dev_num);
#		if (ret) printf("No SD device found !\n");
#	}
#	#endif /* CONFIG_SUPPORT_EMMC_BOOT */
#

# in ./common/bootm.c
#
# add "#define CONFIG_SYS_BOOTM_LEN 0x1000000 /* 16 MB */"
#

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
# boot media -->
# Support for booting from NAND flash = enabled
# Support for booting from SD/EMMC = enabled
# Support for booting from SPI flash = enabled
# -->
# Set kernel load address = 0x80008000 # 0x82080000 ?
# Command line interface -->
# Autoboot options -->
# Autoboot --> disabled
# Stop autobooting via specific input key / string = enabled
# Enable Ctrl+C autoboot interruption = enabled
# Boot commands -->
# bootz = enabled
# Device access commands -->
# mmc = enabled
# Filesystem commands -->
# ext4 command support = enabled
#
# add as needed to ./config
# CONFIG_MMC=y
# CONFIG_SD_BOOT=y
# CONFIG_SYS_TEXT_BASE
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all

# extra steps
cp hi_gzip uboot/arch/arm/cpu/armv7/hi3516cv500/hw_compressed/
cp reg_info.bin uboot/reg_info.reg
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- u-boot-z.bin

# modify arch/arm/cpu/armv7/hi3516cv500/hw_compressed/startup.c -> start_armboot()
# to not use the hardware gzip interface and instead to copy the uboot image from
# flash to ram@0x80800000, that is, comment out everything and add the following
#> for (i = 0; i < image_data_len; i++)
#>     *(unsigned char*)(0x80800000 + i) = *(unsigned char*)(input_data + i);
#> invalidate_icache_all();
#> ((fn)0x80800000)();


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# burn uboot to flash from scratch
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# in host terminal:
sudo ./flasher_pause /dev/ttyUSB0
# power-cycle camera
# cancel flasher_pause
sudo ./flasher /dev/ttyUSB0 fastboot.bin
sudo ./flasher -a 0x80800000 /dev/ttyUSB0 u-boot.bin
# u-boot should now be running in RAM
# - - - - - - - - - - -
# in u-boot hush shell:
mw.b 0x82000000 ff 0x100000
# fatload mmc 0:1 0x82000000 u-boot.bin
# OR
# tftp 0x82000000 u-boot.bin
nand erase 0 0x100000 # clear 0x100000 bytes starting at 0x0
nand write 0x82000000 0 0x100000 # write 0x100000 bytes from 0x82000000 RAM to 0x0 NAND

# for tftp, on host install tftpd-hpa
# put files in /var/lib/tftpboot
# $ sudo ip addr add 192.168.0.2/24 dev eno1 # host
# # setenv serverip 192.168.0.2
# # setenv ipaddr 192.168.0.3
# # tftp 0x82000000 u-boot-smd.bin

# for spl adaptations
# # setenv serverip 192.168.0.2
# # setenv ipaddr 192.168.0.3
# # mw.b 0x82000000 ff 0x100000
# # tftp 0x82000000 spl.bin
# # tftp 0x82006000 blinker.bin

setenv serverip 192.168.0.2
setenv ipaddr 192.168.0.3
mw.b 0x82000000 ff 0x100000
tftp 0x82000000 u-boot-hi3516cv500.bin
nand erase 0 0x100000
nand write 0x82000000 0 0x100000
reset

#
# update uboot environment variables for booting
#

setenv bootenv /boot/uEnv.txt
setenv bootenv_loadaddr 0x81000000
setenv uname_r none
setenv kernel_load_address 0x82000000
setenv kernel_image /boot/uImage
setenv sd_uenv_exists test -e mmc 0:1 \${bootenv}
setenv loadbootenv ext4load mmc 0:1 \${bootenv_loadaddr} \${bootenv}
setenv importbootenv echo Importing environment from SD ...\; env import -t \${bootenv_loadaddr} \${filesize}
setenv uenvboot if run sd_uenv_exists\; then run loadbootenv\; echo Loaded environment from \${bootenv}\; run importbootenv\; fi\; if test -n \${uenvcmd}\; then echo Running uenvcmd ...\; run uenvcmd\; fi
setenv sdboot if mmcinfo\; then run uenvboot\; echo Copying Linux from SD to RAM... \&\& ext4load mmc 0:1 \${kernel_load_address} \${kernel_image}-\${uname_r} \&\& bootm \${kernel_load_address}\; fi
setenv bootcmd run sdboot


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# build linux kernel
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
cd linux-stable
git tag -l | grep 4.9.37 # check version tag exists
git checkout -b dev/TekuConcept v4.9.37
git apply --check linux-4.9.37.patch # test patch
git apply --3way linux-4.9.37.patch # apply patch

mkdir Build
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build hi3516cv500_smp_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build menuconfig
#
# or edit .config with the following:
# CONFIG_CROSS_COMPILE="arm-linux-gnueabihf-"
# CONFIG_LOCALVERSION="sb01"
# CONFIG_DEFAULT_HOSTNAME="tekuconcept"
# CONFIG_SWAP=y
# CONFIG_IKCONFIG=y
# CONFIG_IKCONFIG_PROC=y
# CONFIG_CC_OPTIMIZE_FOR_SIZE=y
# CONFIG_BLK_DEV_INITRD=y
# CONFIG_INITRAMFS_SOURCE=""
# CONFIG_ARCH_HI3516CV500=y # system type > hisilicon bvt soc
# CONFIG_ARCH_HISI_BVT=y
# CONFIG_ARCH_MULTI_V7=y
# CONFIG_OF=y
# CONFIG_OF_OVERLAY=y
# CONFIG_CFG80211=m
# CONFIG_RFKILL=y
# CONFIG_PWM=y
# # CONFIG_DEBUG_LL=y
# # CONFIG_DEBUG_HI3516CV500_UART=y
#
# Note: one source file has a naughty include line
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build -j4 uImage
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build -j4 modules

mkdir Install
cp ./Build/arch/arm/boot/*(u)*(z)Image ./Install/
cp ./Build/arch/arm/boot/dts/*.dtb ./Install/
make ARCH=arm O=$(pwd)/Build modules_install INSTALL_MOD_PATH=$(pwd)/Install/modules
make ARCH=arm O=$(pwd)/Build firmware_install INSTALL_FW_PATH=$(pwd)/Install/firmware
make ARCH=arm O=$(pwd)/Build headers_install INSTALL_HDR_PATH=$(pwd)/Install/headers

cp Build/arch/arm/boot/uImage //rootfs/boot/
cp -R -L Install/modules/* //rootfs/
cp -R -L Install/firmware/* //rootfs/lib/firmware/

# mannually making an image
uboot/tools/mkimage \
	-A arm \
	-O linux \
	-T kernel \
	-C none \
	-a 0x80008000 \
	-e 0x80008000 \
	-n "Linux Kernel" \
	-d Build/arch/arm/boot/zImage \
	Install/uImage

cat Build/arch/arm/boot/Image | gzip > Install/Image.gz
cd Install
uboot/tools/mkimage \
	-A arm \
	-O linux \
	-T kernel \
	-C gzip \
	-a 0x80008000 \
	-e 0x80008000 \
	-n "Linux Kernel" \
	-d Install/Image.gz \
	uImage
cd ../..



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# creating an initramfs
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#!/bin/bash

ARCH="armv7l"
BB_VER="1.31.0"

# Dirs
mkdir -p root
cd root
mkdir -p bin dev etc lib mnt proc sbin sys tmp var
cd -

# Utils
if [ ! -f "root/bin/busybox" ]; then
    curl -L "https://www.busybox.net/downloads/binaries/${BB_VER}-defconfig-multiarch-musl/busybox-${ARCH}" >root/bin/busybox
fi

cd root/bin
chmod +x busybox
ln -s busybox mount
ln -s busybox sh
cd -

# Init process

cat >>root/init << EOF
#!/bin/busybox sh
/bin/busybox --install -s /bin
mount -t devtmpfs  devtmpfs  /dev
mount -t proc      proc      /proc
mount -t sysfs     sysfs     /sys
mount -t tmpfs     tmpfs     /tmp
setsid cttyhack sh
exec /bin/sh
EOF
chmod +x root/init

# initramfs creation

cd root
find . | cpio -ov --format=newc | gzip -9 >../initramfs
cd -




# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# setting up linux
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

git clone https://github.com/mirror/busybox.git
cd busybox && mkdir Build
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build menuconfig
# Settings -->
# Enable compatibility for full-blown desktop systems = disabled
# Enable obsolete features removed before SUSv3 = disabled
# Build static binary (no shared libs) = enabled
# Cross compiler prefix = arm-linux-gnueabihf-
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build install
cd ..

# https://jootamam.net/howto-initramfs-image.htm
mkdir -p armhf-linux/initramfs/{bin,sbin,etc,proc,sys,newroot}
cd armhf-linux/initramfs
touch etc/mdev.conf
cp ../../busybox/Build/busybox bin/
cd bin && ln -s busybox sh && cd ..

printf "\
#!/bin/sh
# Mount things needed by this script
mount -t proc proc /proc
mount -t sysfs sysfs /sys
# Disable kernel messages from popping onto the screen
echo 0 > /proc/sys/kernel/printk
clear # Clear the screen
# Create all the symlinks to /bin/busybox
busybox --install -s
# Create device nodes
mknod /dev/null c 1 3
mknod /dev/tty c 5 0
mdev -s
# Function for parsing command line options with "=" in them
# get_opt("init=/sbin/init") will return "/sbin/init"
get_opt() {
	echo "$@" | cut -d "=" -f 2
}
# Defaults
init="/sbin/init"
root="/dev/mmcblk0p2" # new root
# Process command line options
for i in \$(cat /proc/cmdline); do
	case \$i in
		root\\=*)
			root=\$(get_opt $i)
			;;
		init\\=*)
			init=\$(get_opt $i)
			;;
	esac
done
# Mount the root device
mount "\${root}" /newroot
# Check if $init exists and is executable
if [[ -x "/newroot/${init}" ]] ; then
	# Unmount all other mounts so that the ram used by
	# the initramfs can be cleared after switch_root
	umount /sys /proc
	# Switch to the new root and execute init
	exec switch_root /newroot "\${init}"
fi
# This will only be run if the exec above failed
echo "Failed to switch_root, dropping to a shell"
exec sh
" > init && chmod +x init

find . | cpio -o -Hnewc | gzip > ../initramfs.gz

uboot/tools/mkimage \
	-A arm \
	-O linux \
	-T ramdisk \
	-C gzip \
	-a 0x85A00000 \
	-e 0x85A00000 \
	-n "Initial RAM Disk" \
	-d ../initramfs.gz \
	../initramfs.img



# other notes

setenv bootargs mem=128M console=ttyAMA0,115200 root=/dev/mmcblk0p2 rw rootwait
setenv bootargs mem=128M console=ttyAMA0,115200 initrd=0x85A00000
setenv bootargs earlyprintk=serial mem=128M console=ttyAMA0,115200 noinitrd

bootargs=mem=128M console=ttyAMA0,115200 ubi.mtd=2 root=ubi0:ubifs rootfstype=ubifs rw mtdparts=hinand:1M(boot),4M(kernel),8M(rootfs),7M(appfs),128k(hwconfig),896K(noconfig),12M(models),10M(snapdata),2M(config),-(user)

bootargs=mem=128M console=ttyAMA0,115200 ubi.mtd=2 root=ubi0:ubifs rootfstype=ubifs rw mtdparts=hinand:1M(boot),4M(kernel),8M(rootfs),7M(appfs),128k(hwconfig),896K(noconfig),12M(models),10M(snapdata),2M(config),128k(sdboot)ro,-(user)

# uboot examples
include/configs/hi3516cv500.h
#define CONFIG_STANDALONE_LOAD_ADDR 0x82000000
uboot/tools/mkimage \
	-A arm \
	-O linux \
	-T standalone \
	-C none \
	-a 0x82000000 \
	-e 0x82000000 \
	-n "Hello World" \
	-d hello_world.bin \
	hello_world.img

Note that the Device Tree At Fixed Address kernel configuration option is only available in the menu when device tree has been enabled in the kernel configuration.

https://www.kernel.org/doc/Documentation/arm/Booting
CPU register settings
  r0 = 0,
  r1 = machine type number discovered in (3) above.
  r2 = physical address of tagged list in system RAM, or
       physical address of device tree block (dtb) in system RAM

bootm <zImage> - <dtb>


uboot/tools/mkimage \
	-A arm \
	-O linux \
	-T flat_dt \
	-C none \
	-a 0x84F00000 \
	-e 0x84F00000 \
	-n "Flat Device Tree" \
	-d Install/hi3516cv500-demb.dtb \
	Install/dtb.img


# -----------
mkdir Build
cp arch/arm/configs/hi3516cv500_smp_defconfig Build/.config
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build uImage
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build modules
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build prepare
make ARCH=arm O=$(pwd)/Build modules_install INSTALL_MOD_PATH=$(pwd)/Install
make ARCH=arm O=$(pwd)/Build firmware_install INSTALL_FW_PATH=$(pwd)/Install
make ARCH=arm O=$(pwd)/Build headers_install INSTALL_HDR_PATH=$(pwd)/Install




# ----------------
# hush boot script
# ----------------

uboot/tools/mkimage -A arm -O linux -T script -C none -a 0x82080000 -e 0x82080000 -n "sd bootstrap" -d bootstrap.hush bootstrap

# PREDEFINED:
# setenv bootargs mem=128M console=ttyAMA0,115200 ubi.mtd=2 root=ubi0:ubifs rootfstype=ubifs rw mtdparts=hinand:1M(boot),4M(kernel),8M(rootfs),7M(appfs),128k(hwconfig),896K(noconfig),12M(models),10M(snapdata),2M(config),128k(sdboot)ro,-(user)
# setenv native_boot nand read 0x84000000 0x100000 0x400000;bootm 0x84000000
# setenv load_sdreader nand read 0x82000000 0x2D00000 0x20000
# setenv bootcmd ${UPDATE};${load_sdreader};go 0x82000000 "bootstrap" 0x82080000 && source 0x82080000 || ${native_boot}

# add bootarg panic=x to reboot on panic (x seconds)
# as needed add init=/sbin/init to run specific binary on boot

# Burn sd-reader to flash
mw.b 0x82000000 ff 0x20000 # set 0x20000 bytes to 0xFF starting at 0x82000000
tftp 0x82000000 sdio.bin # copy file sdio.bin from pre-conf server to address 0x82000000
nand erase 2D00000 0x20000 # clear 0x20000 bytes starting at 0x2D00000
nand write 0x82000000 2D00000 0x20000 # write 0x20000 bytes from 0x82000000 RAM to 0x2D00000 NAND



1M 		100000
4M 		400000 500000
8M 		800000
7M 		700000 1400000
128k 	 20000
896k 	 E0000 1500000
12M 	C00000
10M 	A00000 2B00000
2M 		200000 2D00000



setenv bootargs 'mem=128M console=ttyAMA0,115200 ubi.mtd=2 root=ubi0:ubifs rootfstype=ubifs rw mtdparts=hinand:1M(boot),4M(kernel),8M(rootfs),7M(appfs),128k(hwconfig),896K(noconfig),12M(models),10M(snapdata),2M(config),128k(sdboot)ro,-(user)'
setenv native_boot 'nand read 0x84000000 0x100000 0x400000'
setenv load_sdreader nand read 0x82000000 0x2D00000 0x20000

setenv bootcmd 'tftp 0x82000000 sdio.bin;go 0x82000000 "bootstrap" 0x82080000 && source 0x82080000 || ${native_boot};bootm 0x84000000'

setenv bootcmd '${load_sdreader};go 0x82000000 "bootstrap" 0x82080000 && source 0x82080000 || ${native_boot};bootm 0x84000000'


nand read 0x82000000 0x2D00000 0x20000




# ---------------
# building rootfs
# ---------------

mkdir /bin /boot /dev /etc /home /lib /media /mnt /opt /proc /sbin /sys /usr /var /tmp
mkdir /etc/init.d
echo "mount -t proc proc /proc" > /etc/init.d/rcS
echo "mount -t sysfs sysfs /sys" >> /etc/init.d/rcS
echo "mount -t configfs none /config" >> /etc/init.d/rcS
echo "echo 0 > /proc/sys/kernel/printk" >> /etc/init.d/rcS

# unpacked the following packages using `sudo dpkg -x <package> <rootfs>`
# -- gcc-8-base
# -- libgcc1
# -- libc6
# -- libstdc++6
# -- libc-l10n
# -- libbz2
# -- liblzma
# -- libpcre3
# -- libselinux
# -- libattr
# -- libacl1
# -- libuuid
# -- libblkid
# -- libgpg-error
# -- libgcrypt
# -- liblz
# -- libdb
# -- libcap
# -- libcap2
# -- libcap2-bin
# -- libsepol
# -- libaudit-common
# -- libaudit
# -- libsemanage-common
# -- libsemanage
# -- libpam
# -- libpam-modules-bin
# -- libpam-modules
# -- libpam-runtime
# -- libapparmor
# -- libargon
# -- libjson
# -- libssl
# -- libssl-dev
# -- libudev
# -- libdevmapper
# -- libcryptsetup
# -- libgmp
# -- libnettle
# -- libhogweed
# -- libunistring
# -- libidn11
# -- libidn2
# -- libffi
# -- libp
# -- libtasn
# -- libgnutls
# -- libip4tc
# -- libkmod
# -- libmount
# -- libseccomp
# -- libsystemd
# -- libsmartcols
# -- libtinfo
# -- libfdisk
# -- libncursesw
# -- libzstd
# -- libapt-pkg
# -- libc-bin
# -- libbsd
# -- libedit2
# -- libopts
# -- libsigsegv
# -- libreadline
# -- libmpfr
# -- libelf
# -- libmnl
# -- libxtables
# -- libisc-export
# -- libdns-export
# -- libatomic
# -- libassuan
# -- libksba
# -- libsasl2-modules-db
# -- libsasl2
# -- libldap-common
# -- libldap
# -- libnpth
# -- libsqlite
# -- login
# -- fdisk
# -- dmsetup
# -- passwd
# -- adduser
# -- zlib
# -- tar
# -- gpgv
# -- debian-archive-keyring
# -- openssl
# -- dpkg
# -- lsb-base
# -- netbase
# -- readline-common
# -- gawk
# -- debianutils
# -- isc-dhcp-client
# -- gpgconf
# -- dirmngr
# -- pinentry-curses
# -- gpg-agent
# -- gpgsm
# -- gpg
# -- gpg-wks-client
# -- gpg-wks-server
# -- gnupg-utils
# -- gnupg-l10n
# -- gnupg
# -- coreutils
# -- mktemp
# -- diffutils
# -- debconf
# -- perl-base
# -- dialog
# -- init-system-helpers
# -- util-linux
# -- mount
# -- apt
# -- systemd
# -- systemd-sysv
# -- base-files
# -- locales
# -- iproute

# installed with the new apt program
# - libncurses-dev
# - ncurses-base
# - ncurses-bin
# - ntp
# - curl
# - bash
# - dhcpcd5
# - isc-dhcp-server
# - lshw
# - iw
# - wpasupplicant
# - apt-utils
# - kmod (for kernel module management; ln -s /usr/local/bin/kmod /sbin/depmod)

# edit /etc/default/isc-dhcp-server
# - INTERFACESv4="eth0"
# - INTERFACESv6="eth0"

# automated networking
systemctl enable --now dhcpcd

# for missing file /var/lib/dpkg/available
dpkg --clear-avail && apt update

# for "perl: warning: Setting locale failed."
export LANGUAGE=en_US.UTF-8
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
locale-gen en_US.UTF-8
dpkg-reconfigure locales
# Locales to be generated: en_US.UTF-8
# Default locale for the system environment: en_US.UTF-8


sudo echo "include /etc/ld.so.conf.d/*.conf" > /etc/ld.so.conf
# run ldconfig before switching init from /bin/busybox to /lib/systemd/systemd
# append "systemd.unit=multi-user.target" to bootargs
# created file /etc/systemd/network/eth0.network
# renew IP address lease with `dhclient -v`

touch /var/lib/dpkg/status
touch /etc/apt/sources.list


# -- runtime --
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devpts devpts /dev/pts
mount -t configfs none /config
echo 0 > /proc/sys/kernel/printk
dhclient -v
export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
date -s "27 DEC 2019 21:11:00"


# -- swap --

dd if=/dev/zero of=/var/swapfile bs=1024 count=1048576 status=progress
chmod 600 /var/swapfile
mkswap /var/swapfile
swapon /var/swapfile

# -- fstab --

touch /etc/fstab
cat > /etc/fstab << "EOF"
# Begin /etc/fstab

# file system  mount-point  type     options             dump  fsck
#                                                              order

/dev/mmcblk0p2 /            ext4     defaults            1     1
/var/swapfile  swap         swap     defaults            0     0
proc           /proc        proc     nosuid,noexec,nodev 0     0
sysfs          /sys         sysfs    nosuid,noexec,nodev 0     0
devpts         /dev/pts     devpts   gid=5,mode=620      0     0
tmpfs          /run         tmpfs    defaults            0     0
devtmpfs       /dev         devtmpfs mode=0755,nosuid    0     0

# End /etc/fstab
EOF



# --------------------
# Dynamic Device Trees
# --------------------

git clone https://github.com/ikwzm/dtbocfg.git
cd dtbocfg
make ARCH=arm KERNEL_SRC_DIR=/home/amanda/Documents/GitHub/linux-stable/Install/lib/modules/4.9.37/build

insmod dtbocfg.ko
mkdir -p /config
mount -t configfs none /config

dtc -I dts -O dtb -o myoverlay.dtb myoverlay.dts
mkdir /config/device-tree/overlays/myoverlay
cp myoverlay.dtb /config/device-tree/overlays/myoverlay/dtbo
echo 1 > /config/device-tree/overlays/myoverlay/status

# - example -
insmod dtbocfg.ko
mount -t configfs none /config/
mkdir /config/device-tree/overlays/demo
STATUS="cat /config/device-tree/overlays/demo/status"
ENABLE="echo 1 > /config/device-tree/overlays/demo/status"
DISABLE="echo 0 > /config/device-tree/overlays/demo/status"
LOAD="cp demo.dtb /config/device-tree/overlays/demo/dtbo"
eval $DISABLE && eval $LOAD && eval $ENABLE
eval $STATUS




# --------------------
# WiFi
# --------------------

LINUX_BUILD=/home/amanda/Documents/GitHub/linux-stable/Install/modules/lib/modules/4.9.37/build
# LINUX_BUILD=/home/amanda/Documents/GitHub/linux-stable/Install/lib/modules/4.9.37/build

# - Part 1 -
cd /path/to/linux-kernel
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build menuconfig
# --> Networking support
# --> Wireless
# <M> for cfg80211
# <save>, <exit>
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$(pwd)/Build modules
make ARCH=arm O=$(pwd)/Build modules_install INSTALL_MOD_PATH=$(pwd)/Install

# - Part 2A (RTL8192EU driver) -
git clone https://github.com/Mange/rtl8192eu-linux-driver
cd rtl8192eu-linux-driver
# Edit Makefile
# > CONFIG_PLATFORM_I386_PC = n
# > CONFIG_PLATFORM_HISILICON = y
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- KSRC=$LINUX_BUILD

# - Part 2B (RTL8188FU driver) -
https://github.com/kelebek333/rtl8188fu
cd rtl8188fu
# Edit Makefile
# > CONFIG_WIFI_MONITOR = y
# CONFIG_PLATFORM_I386_PC = n
# CONFIG_PLATFORM_HISILICON = y
# # Insert The Following Snippet:
# ifeq ($(CONFIG_PLATFORM_HISILICON), y)
# EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_HISILICON
# ifeq ($(SUPPORT_CONCURRENT),y)
# EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
# endif
# EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
# ARCH := arm
# endif
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- KSRC=$LINUX_BUILD

# - Part 2C (BL-RTL8188FU3A driver) -
# aquire sources from manufacturer
# CONFIG_WIFI_MONITOR = y
# CONFIG_PLATFORM_I386_PC = n
# CONFIG_PLATFORM_HISILICON = y
cd rtl8188fu
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- KSRC=$LINUX_BUILD

# - Part 3 -
# mkdir -p /usr/lib/modules/$(uname -r)
mkdir -p /lib/modules/$(uname -r)/kernel/net/wireless
cp cfg80211.ko /lib/modules/$(uname -r)/kernel/net/wireless
modprobe cfg80211.ko

# Option 2A
cp rtl8192eu.ko /lib/modules/$(uname -r)/kernel/net/wireless
insmod rtl8192eu.ko

# Option 2B
mkdir -p /lib/firmware/rtlwifi
mkdir -p /etc/modprobe.d/
echo "options rtl8188fu rtw_power_mgnt=0 rtw_enusbss=0" > /etc/modprobe.d/rtl8188fu.conf
cp rtl8188fufw.bin /lib/firmware/rtlwifi/
cp rtl8188fu.ko /lib/modules/$(uname -r)/kernel/net/wireless
insmod rtl8188fu.ko # rtw_power_mgnt=0

# - - - - -

depmod -a $(uname -r)
# note: make sure USB bus power is on

# - - - - -

#
# Setting up a wifi connection
#

# hwaddress ether 00:11:22:33:44:55

echo -e "\
iface wlan0 inet dhcp\n\
  wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf\n\
" >> /etc/network/interfaces

echo -e "\
network={\n\
	ssid="${WIFI_NAME}"\n\
	psk="${WIFI_PASSWORD}"\n\
}\n\
\n\
update_config=1\n\
ctrl_interface=DIR=/run/wpa_supplicant\n\
" > /etc/wpa_supplicant/wpa_supplicant.conf
# add optional 'priority=${PRIORITY_NUMBER}'
# add optional 'key_mgmt=[NONE,WPA-PSK,...]'
# add optional 'mode=${MODE_N}'
# add optional 'frequency=${RADIO_FREQUENCY}'

# test with `wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant/wpa_supplicant.conf`
# command-line-interface: run ^^^ and then `wpa_cli -i wlan0`
add_network # creates new network id eg '0';
set_network $NETID ssid "<AP-name>"
set_network $NETID psk "<AP-password>"
enable_network $NETID
save config
disable_network $NETID # to disable
remove_network $NETID # to remove
quit

# ----------------------------------------------
cat <<EOF> /lib/systemd/system/wifi-up.service
[Unit]
Description=Delayed WiFi Setup (because network is failing to do this for us)
Before=network-pre.target
Wants=network-pre.target

[Service]
Type=oneshot
ExecStart=/sbin/ifup wlan0
ExecStop=/sbin/ifdown wlan0
RemainAfterExit=yes
Restart=no

[Install]
WantedBy=default.target
EOF
# ----------------------------------------------

systemctl enable wifi-up.service
# this is a work-around solution to starting wifi on boot
# technically "auto" and "allow-hotplug" in
# /etc/network/interfaces is supposed to do this for us

# Reboot machine or restart network interface with one of:
> /etc/init.d/networking [start,restart,stop]
> systemctl [start,restart,stop,enable,disable] networking
> systemctl restart systemd-networkd




# --------------------
# Load KO at Boot
# --------------------

mkdir -p /etc/modules-load.d
echo module-name > /etc/modules-load.d/module-name.conf
cp module-name.ko /lib/modules/<kernel_release>/kernel/drivers/
echo "options module-name option1 option2 ..." > /etc/modprobe.d/module-name.conf

# alt
echo module-name >> /etc/modules




# --------------------
# SystemD Troubleshooting
# --------------------

systemctl --failed
systemctl status <module>
systemctl restart <module>
journalctl _PID=<pid>




# --------------------
# MISCELLANEOUS
# --------------------

# pin a package to prevent it from being upgraded
sudo apt-mark hold <package>
sudo apt-mark unhold <package>


