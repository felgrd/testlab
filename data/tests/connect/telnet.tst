#!/bin/bash

## @package telnet
#  Test telnet connect to the router.
#
#  @param $1: ID of router.
#  @param $2: ID of release.


# Kontrola parametru adresa testovaneho routeru
if [ -z $1 ];then
	echo "Missing parameter address of router" 1>&2
	exit 1
fi

# IP adresa prvniho routeru
ROUTER1=$1

# Puvodni protokol
PROTOCOL=$(tl_remoteinfo -t $ROUTER1)

# Zmena komunikacniho protokolu
tl_remotechange -t telnet $ROUTER1

# Kontrola zmeny protokolu
NEW=$(tl_remoteinfo -t $ROUTER1)

if [ $NEW != "telnet" ]; then
	echo "Change protocol to telnet error." 1>&2
	exit 1
fi

# Zkouska komunikace
REPLY=$(tl_remote $ROUTER1 "echo connection")

# Kontrola odpovedi routeru
if [ $? -eq 0 ] && [ $REPLY == "connection" ]; then
	RESULT=0
else
	echo "Bad or no response from router." 1>&2
	RESULT=1
fi

# Zmena na puvodni komunikacni protokol
tl_remotechange -t $PROTOCOL $ROUTER1

exit $RESULT
