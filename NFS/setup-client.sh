# http://howtoforge.com/install_nfs_server_and_client_on_debian_wheezy
# NFS  is   a   prerequisite   for   MPI   in   Super/Cluster   Computing
# It's primary [observed]  function  is  to  sync  files/folders  between
# client-server systems.
#
# NOTE:  This  script  assumes  the  respective  Client  and  Server  IPs
# It also sets up a unison between  systems  via  the  bone_client_1  and
# /var/www  folders  for  testing   the   available   options   of   NFS.
# Finally, this scripts was only built for  a  single  client  to  server
# setup, nevertheless NFS can be setup for numerous machines in a network
#
# This script was successfully tested on:
# Linux Distro: Debian Wheezy
# Platform:     Beaglebone Black vA5A
# Date:         7/30/2014

SERVER=10.0.0.20
CLIENT=10.0.0.22
NAME=bone_client_1

echo "Installing required packages..."
apt-get install nfs-common

echo "Creating directories..."
mkdir -p /home/debian/$NAME
if [ $(ls /var | grep www) != "www" ]; then
	mkdir -p /var/www
else
	/etc/init.d/apache2 stop
	mv /var/www /var/www_bak
	mkdir -p /var/www
fi

echo "Mounting files..."
mount $SERVER:/home/debian/$NAME /home/debian/$NAME
mount $SERVER:/var/www /var/www
