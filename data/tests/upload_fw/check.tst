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

# Firmware v routeru
ROUTER_FW=$(tl_status $ROUTER1 sys "Firmware Version")

# Porovnani firmwaru
if [ "$NEW_FW" == "$ROUTER_FW" ]; then
	RESULT=0
else
	RESULT=1
	echo "Bad Firmware version. Required: $NEW_FW. Actual: $ROUTER_FW."	1>&2
fi

# Ukonceni scriptu
exit $RESULT
