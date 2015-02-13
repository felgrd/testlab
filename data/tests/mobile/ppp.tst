#!/bin/bash

## @file ppp.tst
## @brief Test ppp connection to mobile wan.
##	
## @author David Felgr
## @version 1.0.0
##   
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - Connection established. 1 - Connection is not established.

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
	echo "Missing parameter address of router" 1>&2
	exit 1
fi

#IP adresa prvniho routeru
ROUTER1=$1

# 10 pokusu o provedeni testu
for (( a=1; $a-11; a=$a+1 ))
do
	# Zjisteni IP adresy mobilniho pripojeni
	REPLY=$(tl_status 1 ppp "IP Address") &> /dev/null
	
	# Kontrola odpovedi 
	if [ -z "$REPLY" ]; then
		echo "No answer from router." 1>&2
		exit 1
	fi
	
	# Kontrola pripojeni
	if [ "$REPLY" != "Unassigned" ]; then
		break
	fi	
	
	# Pauza pro moznonst navazani spojeni
	sleep 10
done

# Kontrola pripojeni 
if [ "$REPLY" == "Unassigned" ]; then
	echo "PPP connection is not established. IP: $REPLY." 1>&2
	exit 1
fi	

exit 0

