#!/bin/bash

if [ $EUID -ne 0 ]; then
        echo "[!] Need to be root ! "
        exit 1
fi



service network-manager stop
service dnsmasq stop
service hostapd stop 
service apache2 start

hostapd ./hostapd.conf &
ifconfig wlxa0f3c124fa3a 192.168.12.1
dnsmasq -C ./dnsmasq.conf &

