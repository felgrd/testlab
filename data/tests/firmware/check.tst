#!/bin/bash

## @package upload
#  Test firmware version.
#
#  @param $1: ID of router.
#  @param $2: ID of release.

# Kontrola parametru ID testovaneho routeru
if [ -z $1 ] || [ -z $2 ]; then
	echo "Missing parameter router or release" 1>&2
	exit 1
fi

# ID prvniho routeru
ROUTER1=$1

# ID testovaneho releasu
RELEASE=$2

# Firmware routeru
NAME=$(tl_checkproduct $ROUTER1)

# Pozadovany firmware routeru
NEW_FW=$(cat /var/testlab/firmware/$RELEASE/$NAME.ver)

# Kontrola pozadovaneho firmwaru
if [ -z "$NEW_FW" ];then
	echo "Compile firmware is not found." 1>&2
	exit 1
fi

# Firmware v routeru
ROUTER_FW=$(tl_status $ROUTER1 sys "Firmware Version")

# Kontrola odpovedi od routeru
if [ $? -ne 0 ] || [ -z "$ROUTER_FW" ]; then
	echo "No response from router" 1>&2
fi

# Porovnani pozadovaneho a nahraneho firmwaru
if [ "$NEW_FW" != "$ROUTER_FW" ]; then
	echo "Bad Firmware version. Required: $NEW_FW. Actual: $ROUTER_FW."	1>&2
	exit 1
fi

# Ukonceni scriptu
exit 0
