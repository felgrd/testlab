#!/bin/bash

## @package upload
## Wait for start of router.
##
## @param $1: ID of router.
## @param $2: ID of release.

# Kontrola parametru ID testovaneho routeru
if [ -z $1 ]; then
	echo "Missing parameter ID of router" 1>&2
	exit 1
fi

#ID prvniho routeru
ROUTER1=$1

# Cekani na reboot routeru po ukonceni programovani
sleep 1

# Cekani na nabehnuti routeru
tl_routerready $ROUTER1
if [ $? -ne 0 ]; then
	echo "Router is not running after upload new firmware." 1>&2
	exit 1
fi

# Ukonceni scriptu
exit 0
