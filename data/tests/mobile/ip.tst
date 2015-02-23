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

# IP adresa prvniho routeru
ROUTER1=$1

# Cekani na pripojeni routeru do mobilni site
tl_mobileready $ROUTER1

#
exit $?
