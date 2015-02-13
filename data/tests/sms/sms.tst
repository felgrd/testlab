#!/bin/bash

#  Test sms
#
#  @param $1: ID of router.
#  @param $2: ID of release.

# Pro tento test je nutne ulozit do SIM karty telefoni cislo
# ulozeni cisla:
# gsmat AT+CPBS="ON"
# gsmat AT+CPBW=1,"<tel.cislo SIM karty>"


# Kontrola parametru adresa testovaneho routeru
if [ -z $1 ];then
	echo "Missing parameter address of router" 1>&2
	exit 1
fi

#IP adresa prvniho routeru
ROUTER1=$1

#Vycteni tel.cisla
CISLO=$(tl_remote $1 "gsmat AT+CNUM")
CISLO=${CISLO:12:13}

#Poslani nahodneho cisla pres SMS na testovany router
NAHODNA=$[ 1 + $[ RANDOM % 10000 ]]
tl_remote $1 "sms $CISLO $NAHODNA"
sleep 10
tl_remote $1 "grep -q $NAHODNA /var/log/messages; echo $?"
