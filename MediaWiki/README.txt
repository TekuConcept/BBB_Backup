git clone https://gerrit.wikimedia.org/r/mediawiki/core.git mediawiki
sudo apt install php-cli php-intl php-xml php-mysql php-apcu
sudo apt install composer
sudo apt install mariadb-server
sudo apt install imagemagick
cd mediawiki
composer update --no-dev
sudo /etc/init.d/mysql start
sudo mysql_secure_installation
php -S localhost:8080
> setup the wiki


# edit database
$ mysql
> CREATE USER tekuconcept@localhost IDENTIFIED BY '985606';
> SELECT User FROM mysql.user;
> GRANT ALL PRIVILEGES ON * . * TO tekuconcept@localhost;
