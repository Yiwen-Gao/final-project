#!/bin/bash

if [ $# -eq 0 ]
    then 
	echo "usage: ./setuppriv.sh <mail_system_name>"
	exit
fi

useradd cert-writer
useradd mail-writer
useradd pass-writer

chown -R cert-writer $1/server/ca
chown -R mail-writer $1/server/mail
chown -R pass-writer $1/server/password
chmod -R 700 $1/server/ca
chmod -R 700 $1/server/mail
chmod -R 700 $1/server/password

chown pass-writer $1/server/passwords.txt
chmod 600 $1/server/passwords.txt
