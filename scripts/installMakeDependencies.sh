#!/bin/bash

#update repo
apt-get update

#upgrade repos
apt-get -y upgrade

#install libs to compile 
apt-get install -y \
    libconfig++-dev \
    libmysqlcppconn-dev \
    wiringpi \
    i2c-tools \
    libi2c-dev \
    build-essential \
    git-core

#clean
apt-get clean

echo 'Delete /var/lib/apt/lists/* /tmp/* /var/tmp/*' 
rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*