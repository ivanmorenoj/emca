#!/bin/bash

#need root privilege

DB_USER="insert_user"
DB_PASS="inserp@ss"
DB_NAME="EMCA"

#update repo
apt-get update

#upgrade
apt-get -y upgrade

#install mariadb server
apt-get install -y mariadb-server

SQL_SCRIPTS=$(find ../ -name "*.sql")

echo "Creating database $DB_NAME ."
mysql -e "CREATE DATABASE $DB_NAME /*!40100 DEFAULT CHARACTER SET utf8 */;"
echo "Creating user $DB_USER .."
mysql -e "CREATE USER '$DB_USER'@'localhost' IDENTIFIED BY '$DB_PASS';"
mysql -e "GRANT ALL PRIVILEGES ON $DB_NAME.* TO '$DB_USER'@'localhost';"
mysql -e "FLUSH PRIVILEGES;"

#creating tables in $DB_NAME
for script in $SQL_SCRIPTS; do
    echo "Executing script '$script' ..."
    mysql $DB_NAME < $script
done
