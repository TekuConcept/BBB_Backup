#!/bin/bash

# Add repository
sudo sh -c '. /etc/lsb-release && echo "deb http://mirror.umd.edu/packages.ros.org/ros/ubuntu trusty main" > /etc/apt/sources.list.d/ros-latest.list'

# Add keys
wget https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -O - | sudo apt-key add -

# Update and Install
sudo apt-get update
sudo apt-get install ros-indigo-desktop-full
#sudo apt-get install ros-jade-desktop-full
