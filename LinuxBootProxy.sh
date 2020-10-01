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
mount --rbind /dev/pts /mnt/dev/pts

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

# Grub related repair commands
grub-install /dev/sda
grub-install --recheck /dev/sda
update-grub

# if the error appears:
# "grub-install: error: cannot find EFI directory."
# run the following commands:
# determine the partition holding EFI
lsblk -o NAME,PARTTYPE,MOUNTPOINT | grep -i "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"
# device is likely /dev/sda1
# EFI mount-point / folder is likely /boot/efi or /boot/EFI
# manually specify efi directory
EFI_DIR=/boot # example
grub-install --efi-directory=$EFI_DIR
# https://unix.stackexchange.com/questions/405472/

# Reinstalling Cinnamon
apt install --reinstall cinnamon
