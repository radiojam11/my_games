#!/bin/bash

# wp_options, wp_posts, wp_postmeta. 

echo "[!] This script takes user,pasw,db,table,old and new"
echo "[!] it replaces all the occurrences of old to new in every column"
echo "[!!!] DANGEROUS."
echo 
echo "[+] Hi. please supply mysql credentials"
printf "[+] user: "
read -r user

echo 
printf "[+] password: "
read -r -s passw

echo
printf "[+] db: "
read -r db

echo 
printf "[+] table: "
read -r table

echo
printf "[+] old: "
read -r old

echo
printf "[+] new: "
read -r new

echo "[+] getting table's columns from mysql table. "

# Declaring substitution function
do_substitution() {
 echo "[+] Doing substitution."
 for i in "$@"; do
  echo "[+] processing: "$i
  mysql -v --user="$user" --password="$passw" --database="$db" --execute="update $table set $i=REPLACE($i,'$old','$new')" ;
 done
}

do_substitution $( mysql -s -s -r --user="$user" --password="$passw" --database="$db" --execute="describe $table;" | awk '{ if ( NR>1) print $1 }')

