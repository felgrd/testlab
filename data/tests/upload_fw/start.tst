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

# Zjisteni IP adresy routeru
IP=$(tl_remoteinfo -i $ROUTER1)

# Zacatek mereni casu
START=$(date +%s)

# Pokus o ping na router
for (( a=1; $a-100; a=$a+1 ))
do
	# Ping na router
	ping -c 1 $IP

	# Ulozeni vysledku pingu
	RESULT=$?

	# Kontrola uspesnosti pingu
	if [ $RESULT -eq 0 ]; then
		break
	fi
done

# Pokus o pripojeni se do routeru
for (( a=1; $a-10; a=$a+1 ))
do
	# Dotaz na router
	tl_status $ROUTER1 sys

	# Ulozeni vysledku pingu
	RESULT=$?

	# Kontrola uspesnosti pingu
	if [ $RESULT -eq 0 ]; then
		break
	fi
done

# Ukonceni mereni casu priojeni
STOP=$(date +%s)

# Vypocet trvani startu routeru
TIME=$(($STOP - $START))

# Vypis hodnot testu
echo $TIME

# Ukonceni scriptu
exit $RESULT
