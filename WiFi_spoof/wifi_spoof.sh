#!/bin/bash
#depends on macchanger

if [ $# -lt 5 -o $EUID -ne 0 ]; then
        echo "[!] Usage: "$0" iface_name wifi_name target_ip target_mac route_ip"
        echo "[!] Need to be root ! "
        exit 1
fi

ip=$3
ap=$2
iface=$1
mac=$4
route_ip=$5
echo "[+] lets spoof $mac,$ip"

service network-manager stop
service dnsmasq stop
service wpasupplicant stop

sleep 1

ifconfig $iface down

macchanger $iface -m $mac
iwconfig $iface essid $ap
ifconfig $iface $ip

ifconfig $iface up

route add defautl gw $route_ip

echo '[+] tryin to ping 8.8.8.8 ...'
ping 8.8.8.8 -c 4 -w 3
echo '[!] Bless..'
exit 0
