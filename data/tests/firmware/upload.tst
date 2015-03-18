#!/bin/bash

## @package upload
## Test upload firmware to the router.
##
## @param $1: ID of router.
## @param $2: ID of release.

# Kontrola parametru ID testovaneho routeru
if [ -z $1 ] || [ -z $2 ];then
	echo "Missing parameter router or release" 1>&2
	exit 1
fi

# ID prvniho routeru
ROUTER=$1

# ID testovaneho releasu
RELEASE=$2

# Firmware testovaneho routeru
FIRMWARE=$(tl_checkproduct $ROUTER)
if [ $? -eq 0 ] && [ "$REPLY" ]; then
	echo "Do not find product name in database." 1>&2
	exit 1
fi

# Nahrani firmwaru do routeru
tl_updatefw -r $RELEASE -f $FIRMWARE $ROUTER
if [ $? -eq 0 ]; then
	echo "New firmware is not uploaded to the router." 1>&2
	exit 1
fi

# Ukonceni scriptu
exit 0
