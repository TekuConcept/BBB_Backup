# Up-To-Date script as of April 18, 2016
# Tested on: "Linux Mint 17.2 Cinnamon 32-bit"
#            Oracle VirtualBox v5.0.16 r105871


# [-                        Prepare System                         -]
sudo apt-get update
sudo apt-get install -y python-software-properties
sudo apt-get install -y python make build-essential g++ curl
sudo apt-get install -y libssl-dev apache2-utils git libxml2-dev
sudo apt-get upgrade  # I feel like this line may be optional


# [-                    Create Central Git Repo                    -]
# (or use your own location)
mkdir ~/GitHub
cd ~/GitHub
git clone https://github.com/c9/core                   # clone Cloud9
git clone https://github.com/creationix/nvm.git ~/.nvm # clone nvm
# Cloud 9 v3 is currently developed with Node.js v0.10 and v0.12
source ~/.nvm/nvm.sh
nvm install 0.10


# [-                         Setup Cloud9                          -]
cd core
# sudo is omitted because npm is owned by <user>
npm install packager
npm install
./scripts/install-sdk.sh

echo "- END OF LINE -"


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
# In .bashrc, add the following for convinience:
#
# function node_init() {
#     source ~/.nvm/nvm.sh
#     nvm use 0
# }
#
# function cloud9_start() {
#     node server.js -p 3000 -a : -w '/home/<user>/'
# }      
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
