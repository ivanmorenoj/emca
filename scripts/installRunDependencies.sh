#!/bin/bash

# db user, pass and dbname
DB_USER="insert_user"
DB_PASS="inserp@ss"
DB_NAME="EMCA"

# load sql scripts
SQL_SCRIPTS=$(find ../ -name "*.sql")

#update repo
apt-get update

#upgrade repos
apt-get -y upgrade

#install packages to run 
apt-get install -y \
    libconfig++-dev \
    libmysqlcppconn-dev \
    wiringpi \
    mariadb-server

echo "Creating database $DB_NAME ."
mysql -e "CREATE DATABASE $DB_NAME /*!40100 DEFAULT CHARACTER SET utf8 */;"
echo "Creating user $DB_USER .."
mysql -e "CREATE USER '$DB_USER'@'localhost' IDENTIFIED BY '$DB_PASS';"
mysql -e "GRANT ALL PRIVILEGES ON $DB_NAME.* TO '$DB_USER'@'localhost';"
mysql -e "FLUSH PRIVILEGES;"

#creating tables in $DB_NAME
for script in $SQL_SCRIPTS; do
    echo ">>Executing script '$script' ..."
    mysql $DB_NAME < $script
done

#clean
apt-get clean

echo 'Delete /var/lib/apt/lists/* /tmp/* /var/tmp/*' 
rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*