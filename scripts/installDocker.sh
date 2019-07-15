#!/bin/bash

#update repos
apt-get update

#upgrade
apt-get -y upgrade

#Install packages to allow apt to use a repository over HTTPS
apt-get -y install \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg2 \
    software-properties-common

#Add Docker’s official GPG key
curl -fsSL https://download.docker.com/linux/debian/gpg | sudo apt-key add -

#add docker ce repository
echo "deb [arch=armhf] https://download.docker.com/linux/debian stretch stable" >> /etc/apt/sources.list

#update repos
apt-get update

#install docker
apt-get -y install docker-ce docker-ce-cli containerd.io
