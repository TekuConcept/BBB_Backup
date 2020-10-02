x86_64-efi#
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
mount /dev/sda1 /mnt/boot/efi
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

# one possible fault for boot failure: /var/run gets separated from /run
# to fix, rejoin them as follows: (tested on Linux Mint 19)
mv -f /var/run/* /run/
rm -rf /var/run
ln -s /run /var/run

# Grub related repair commands
grub-install                   \
    --boot-directory=/boot     \
    --bootloader-id=ubuntu     \
    --target=x86_64-efi        \
    --efi-directory=/boot/efi
grub-install                   \
    --uefi-secure-boot         \
    --target=x86_64-efi        \
    --efi-directory=/boot      \
    /dev/sda
grub-install /dev/sda
grub-install --recheck /dev/sda

# edit grub file
nano /etc/default/grub
# comment out GRUB_TIMEOUT_STYLE=hidden
# comment out GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
# set GRUB_TIMEOUT=10
update-grub
# tested on Linux Mint 19
# this will allow kernel debug messages to be displayed
# on boot for troublshooting purposes

# determine the partition holding EFI
lsblk -o NAME,PARTTYPE,MOUNTPOINT | grep -i "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"

# Reinstalling Cinnamon
apt install --reinstall cinnamon
