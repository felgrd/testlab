#!/bin/bash

#  Test sms
#  
#  @param $1: ID of router.
#  @param $2: ID of release.

function error {
  echo $1 1>&2
  exit 1
}

# Kontrola parametru adresa testovaneho routeru
if [ -z $1 ];then
	error "Missing parameter address of router"
fi

#IP adresa prvniho routeru
ROUTER1=$1

#Vycteni tel.cisla
CISLO=$(tl_sim -n $ROUTER1)

# Vyckani na navazani mobile spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? != 0 ];then
	error "PPP conection is not estabished"
fi

#Poslani nahodneho cisla pres SMS na testovany router
NAHODNA=$[ 1 + $[ RANDOM % 10000 ]]
tl_remote $ROUTER1 "sms $CISLO $NAHODNA" &> /dev/null
if [ $? != 0 ];then
	error "SMS wasn't sent"
fi

RESULT="1"
# Vyhledani prijate sms s nahodnou zpravou a ulozeni vysledku do promenne RESULT
for (( a=1; $a-20; a=$a+1 ))
do
	# Hledani sms zpravy v slog
	tl_remote $ROUTER1 "slog | grep -q $NAHODNA" &> /dev/null
	
	# Kontrola jestli byla zprava nalezena
	if [ $? -eq 0 ]; then
		RESULT="0"
		break
	fi
	sleep 1
done

if [ $RESULT != 0 ];then
	error "SMS sending doesn't work"
fi

exit 0






