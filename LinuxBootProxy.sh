#
# In the unfortunate event Linux bootup fails and GRUB cannot be accessed,
# this approach is a reasonable alternate solution to repair things before
# resorting to a full wipe.
#
#
# Boot into a live USB of Linux
# Next, run the following commands:
# (Assuming the boot partition is on /dev/sda1 and root is /dev/sda2)
#

# perform ops as super user
sudo su # or sudo -i

# mount origin system partitions w/ current running kernel's interface
mount /dev/sda2 /mnt
mount /dev/sda1 /mnt/boot
mount -t proc none /mnt/proc
mount --rbind /sys /mnt/sys
mount --rbind /dev /mnt/dev

# switch root directory from live USB to original system's drive
/usr/sbin/chroot /mnt /bin/bash

# set networking and sync drive profile
echo "nameserver 8.8.8.8" >> /etc/resolv.conf
source /etc/profile

# set terminal's prefix to remind us we are
# no longer running from our live USB drive
# (for the current terminal session only)
export PS1="\[\033[1;32m\]chroot to ->\[\033[1;35m\](system) #\[\e[0m\] "

#
# The terminal should now be setup to perform actions such as "apt --reinstall"
# - what ever actions are needed to repair boot, startup scripts, etc.
#
