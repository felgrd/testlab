#!/bin/bash

## @package telnet
## Test telnet connect to the router.
##
## @param $1: ID of router.
## @param $2: ID of release.

# Ukonceni skriptu v pripade chyby
function error {
	tl_remotechange -t $PROTOCOL $ROUTER1
	echo $1 1>&2
	exit 1
}

# Kontrola parametru adresa testovaneho routeru
if [ -z $1 ]; then
	echo "Missing parameter address of router" 1>&2
	exit 1
fi

# IP adresa prvniho routeru
ROUTER1=$1

# Puvodni protokol
PROTOCOL=$(tl_remoteinfo -t $ROUTER1)
if [ $? -ne 0 ] || [ -z "$PROTOCOL" ]; then
	echo "Check actual remote protocol error." 1>&2
	exit 1
fi

# Zmena komunikacniho protokolu
tl_remotechange -t telnet $ROUTER1
if [ $? -ne 0 ]; then
	error "Change remote protocol error."
fi

# Kontrola zmeny protokolu
NEW=$(tl_remoteinfo -t $ROUTER1)
if [ $? -ne 0 ] || [ $NEW != "telnet" ]; then
	error "Change remote protocol to telnet error."
fi

# Zkouska komunikace
REPLY=$(tl_remote $ROUTER1 "echo connection")
if [ $? -ne 0 ] || [ $REPLY != "connection" ]; then
	error "Bad or no response from router."
fi

# Zmena na puvodni komunikacni protokol
tl_remotechange -t $PROTOCOL $ROUTER1

exit 0
