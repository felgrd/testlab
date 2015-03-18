#!/bin/bash

## @package upload
## Test firmware version.
##
## @param $1: ID of router.
## @param $2: ID of release.

# Ukonceni skriptu v pripade chyby
function error {
	echo $1 1>&2
	exit 1
}

# Kontrola parametru ID testovaneho routeru
if [ -z $1 ] || [ -z $2 ]; then
	error "Missing parameter router or release"
fi

# ID prvniho routeru
ROUTER1=$1

# ID testovaneho releasu
RELEASE=$2

# Firmware routeru
NAME=$(tl_checkproduct $ROUTER1)
if [ $? -ne 0 ]; then
	error "Firmware name is not found in database."
fi

# Pozadovany firmware routeru
NEW_FW=$(cat /var/testlab/firmware/$RELEASE/$NAME.ver)
if [ $? -ne 0 ] || [ -z "$NEW_FW" ]; then
	error "Compile firmware is not found."
fi

# Firmware v routeru
ROUTER_FW=$(tl_status $ROUTER1 sys "Firmware Version")
if [ $? -ne 0 ] || [ -z "$ROUTER_FW" ]; then
	error "No response from router"
fi

# Porovnani pozadovaneho a nahraneho firmwaru
if [ "$NEW_FW" != "$ROUTER_FW" ]; then
	error "Bad Firmware version. Required: $NEW_FW. Actual: $ROUTER_FW."
fi

# Ukonceni scriptu
exit 0
