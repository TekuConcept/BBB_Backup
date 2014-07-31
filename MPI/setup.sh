# http://techtinkering.com/2009/12/02/setting-up-a-beowulf-cluster-using-open-mpi-on-linux/
# This scripts sets up the MPI interface for 'Beowulf' Cluster computing.
# The use of this supercomputer is to run a program from the master  node
# while utilizing the processing power from all  registered  slave  nodes
#
# NOTE:  This  script  assumes  the  respective  Client  and  Server  IPs
# It was only built for a single  client  to  server  setup, nevertheless
# MPI can be setup over numerous machines in a network. NFS  is  required
# for MPI file sharing and program execution over multiple  slave  nodes.
#
# This script was successfully tested on:
# Linux Distro: Debian Wheezy
# Platform:     Beaglebone Black vA5A
# Date:         7/30/2014

SERVER=10.0.0.20
CLIENT=10.0.0.22

echo "Installing required packages..."
# Be sure to do the same for each slave
#apt-get update # sometimes necessary
apt-get install openmpi-bin
#apt-get install ompenmpi-common
apt-get install libopenmpi-dev
#apt-get install libopenmpi1.3 # see if libopenmi1.6 is available first
#apt-get install openssh-server openssh-client


echo "Generating DSA Key..."
ssh-keygen -t dsa
# hit ENTER to create/write to directory ~/.ssh/
# enter password
# confirm password

# make copy of public key for client and server
# Server holds onto private key as it is the head of MPI
chmod 755 ~/.shh
ssh root@$CLIENT 'mkdir ~/.ssh'
scp ~/.ssh/id_dsa.pub root@$CLIENT:.ssh/authorized_keys
ssh root@$CLIENT 'chmod 600 ~/.ssh/authorized_keys'

# append to authorized_keys if the file already exists or back it up
if [ -e "$HOME/.ssh/authorized_keys" ]; then
	cp ~/.ssh/authorized_keys ~/.ssh/authorized_keys.bak
	cat ~/.ssh/id_dsa.pub >> ~/.ssh/authorized_keys
else
	mv ~/.ssh/id_dsa.pub ~/.ssh/authorized_keys
fi
chmod 600 ~/.ssh/authorized_keys
# ssh 10.0.0.20 -2 root -i $PATH # PATH=~/.ssh/id_dsa
echo "Disabling Empty Passwords for SSH..."
echo "(To re-enable, set PermitEmptyPasswords to yes in /etc/ssh/sshd_config \
	Your current session will require the private key created ealier for all \
	future connections or you may continue using the user password if applicable)"
sed -n "s/PermitEmptyPasswords yes/PermitEmptyPasswords no/p" /etc/ssh/sshd_config
/etc/init.d/ssh restart

ssh-add ~/.ssh/id_dsa # use for mpi-passwordless logins

echo "Configuring OpenMPI..."
touch ~/.mpi_hostfile
echo "# The Hostfile for OpenMPI
	
# The master node
localhost #slots=2 - use if machine contains more than one processor

# The following slave node(s) are single processor machines:
slave1" > ~/.mpi_hostfile

echo "To compile a C program for MPI, use: mpicc program.c
To discover the arguments passed for your normal C-compiler, use: mpicc -shome program.c

To run the program on n processes use the command:
mpirun -np n --hostfile ~/.mpi_hostfile ./program"

