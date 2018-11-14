# Building Debian Packages (plain and simple)
# Specifically, how to build a debian binary package using
# files generated from say a cmake project. Note: debhelper
# tools can build your source for you, but I find this way
# more comprehensible and adaptable.

# Project tree should look as follows:
#
# ./My_Project_Root
#     My_Project_Name_Source
#         Include
#         Source
#         CMakeLists.txt
#     My_Project_Name-0.0.1
#

# Prerequisites
sudo apt-get install --no-install-recommends devscripts fakeroot
sudo apt-get install build-essential debhelper lintian dh-make cmake

# Configure your environment ...otherwise you'll be
# manually entering these in the generated files
echo export EMAIL="john.doe@example.com" >> ~/.bashrc
echo export DEBFULLNAME="John Doe" >> ~/.bashrc

# Build the source project
cd ./My_Project_Root/My_Project_Name_Source
mkdir Build && cd Build && cmake .. && make
# add "-j $(grep -c ^processor /proc/cpuinfo)" to the make command
# to take full advantage of the CPU when building.
# For this example, lets assume the cmake project built a program
# called "example.run", which could be executed as "./example.run"

# Configure the debian package
cd ./My_Project_Root/My_Project_Name-0.0.1
# Note: the version would be the version of your project
dh_make \
--packagename My_Package_Name \
--copyright MIT \
--native \
--single
# Edit My_Project_Name-0.0.1>/debian/* files to fine tune the project
# --single is for a single binary project (platform dependent)
# --native is for debian native projects; You will need to either
# include a ".orig.tar.gz" file in ./My_Project_Root or include a
# --createorig tag to automatically create one for you.

# Add project files to package
cd ./My_Project_Root
mkdir -p My_Project_Name-0.0.1/bin
cp \
My_Project_Name_Source/Build/example.run \
My_Project_Name-0.0.1/bin/example.run
# Now we need to tell the package where to put which files.
echo "bin/* usr/bin" >> \
My_Project_Name-0.0.1/debian/My_Package_name.install

# Build the debian package
cd ./My_Project_Root/My_Project_Name-0.0.1
dpkg-buildpackage
# This will generate a file
# ./My_Project_Root/My_Project_Name-0.0.1-<arch>.deb

# You can install it with
# sudo dpkg -i <package-name>.deb

# and uninstall it with
# sudo dpkg -r <package-name>

