#!/bin/bash

#installing runing dependencies
echo "Installing dependencies"
sh installRunDependencies.sh

echo "Installing on /usr/bin/emca" 
cp bin/emca /usr/bin/

echo "Copy config.cfg on /etc/emca/config.cfg" 
mkdir -p /etc/emca/
cp config.cfg /etc/emca/config.cfg 

echo "Creating /var/log/emca/ where is saved logfile"
mkdir -p /var/log/emca/

echo "Eopy emca.service on /etc/systemd/system/"
cp emca.service /etc/systemd/system/

echo "Enable emca service" 
systemctl daemon-reload
systemctl enable --now emca
