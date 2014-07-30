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
apt-get install nfs-kernel-server nfs-common

echo "Creating directories..."
cd /home/debian
mkdir $NAME
chown nobody:nogroup /home/debian/$NAME
chmod 755 /home/debian/$NAME

if [ $(ls /var | grep www) != "www" ]; then
	mkdir -p /var/www
else
	/etc/init.d/apache2 stop
	mv /var/www /var/www_bak
	mkdir -p /var/www
fi
chown root:root /var/www
chmod 755 /var/www

echo "Updating /etc/exports..."
echo "/home/debian/$NAME $CLIENT(rw,sync,no_subtree_check)" >> /etc/exports
echo "/var/www $CLIENT(rw,sync,fsid=0,crossmnt,no_subtree_check,no_root_squash)" >> /etc/exports

echo "Adding security rules..."
# whitelist schema
echo "portmap:ALL" >> /etc/hosts.deny
echo "lockd:ALL"   >> /etc/hosts.deny
echo "mountd:ALL"  >> /etc/hosts.deny
echo "rquotad:ALL" >> /etc/hosts.deny
echo "statd:ALL"   >> /etc/hosts.deny
# allow local client
echo "ALL: $CLIENT" >> /etc/hosts.allow # add more IPs with ' , '

echo "Restarting NFS..."
/etc/init.d/nfs-kernel-server restart
