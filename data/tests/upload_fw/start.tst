#!/bin/bash

## @package upload
#  Wait for start of router.
#
#  @param $1: ID of router.
#  @param $2: ID of release.

# Kontrola parametru ID testovaneho routeru
if [ -z $1 ];then
	echo "Missing parameter ID of router" 1>&2
	exit 1
fi

#ID prvniho routeru
ROUTER1=$1

# Cekani na ukonceni programovani
sleep 1

# Zacatek mereni casu
START=$(date +%s)

# Cekani na nabehnuti routeru
tl_routerready -t 60 $ROUTER1

# Ukonceni scriptu
exit $?
