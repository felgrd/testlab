#!/bin/bash

##  Test ntp
##
##  @param $1: ID of router.
##  @param $2: ID of release.
##
## Test vyzaduje APN conel.agnep.cz s NTP serverem 10.0.0.1
## Test vyzaduje refreshnuty syslog pri opakovanem pouziti

# Ukonceni skriptu v pripade chyby
function error {
  tl_paramchange -f NTP -p SERVICE $ROUTER1 1
  tl_paramchange -f NTP -p ENABLED $ROUTER1 1
  tl_paramchange -f NTP -p PRIMARY_SERVER $ROUTER1 10.0.0.1
  echo $1 1>&2
  exit 1
}

# Kontrola parametru adresa testovaneho routeru
if [ -z $1 ];then
	echo "Missing parameter address of router" 1>&2
	exit 1
fi

#IP adresa prvniho routeru
ROUTER1=$1

# Vyckani na navazani mobile spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? != 0 ];then
	error "PPP conection is not estabished"
fi

#nastaveni stareho datumu
tl_remote $ROUTER1 "date -s 2010.01.01-01:00" &> /dev/null

# kontrola, jestli se datum zmenil na 2010
tl_remote $ROUTER1 "date" | grep -q "2010"

if [ $? != 0 ];then
	error "Date can't be changed"
fi

# zaloha konfigurace
ZALOHA1=$(tl_paraminfo -f NTP -p SERVICE $ROUTER1)
ZALOHA2=$(tl_paraminfo -f NTP -p ENABLED $ROUTER1)
ZALOHA3=$(tl_paraminfo -f NTP -p PRIMARY_SERVER $ROUTER1)

# nahrani konfigurace
tl_paramchange -f NTP -p SERVICE $ROUTER1 1
tl_paramchange -f NTP -p ENABLED $ROUTER1 1
tl_paramchange -f NTP -p PRIMARY_SERVER $ROUTER1 10.0.0.1
if [ $? != 0 ];then
	error "Update of configuration has failed"
fi

# restart NTP
tl_remote $1 "service ntp restart" &> /dev/null
if [ $? != 0 ];then
	error "NTP can't be restarted"
fi

RESULT=1
for (( a=1; $a-60; a=$a+1 ))
do
	# Hledani zpravy v slog
	tl_remote $ROUTER1 "slog | grep -q \"system clock updated\"" &> /dev/null

	# Kontrola jestli byla zprava nalezena
	if [ $? -eq 0 ]; then
		RESULT=0
		break
	fi
	sleep 1
done

if [ $RESULT != 0 ];then
	error "NTP doesn't work"
fi

sleep 5

# kontrola jestli se po updatu zmenil rok na jiny nez 2010
tl_remote $ROUTER1 "date" | grep -q "2010"

if [ $? != 1 ];then
	error "Date isn't updated"
fi

# Obnoveni konfigurace
tl_paramchange -f NTP -p SERVICE $ROUTER1 $ZALOHA1
tl_paramchange -f NTP -p ENABLED $ROUTER1 $ZALOHA2
tl_paramchange -f NTP -p PRIMARY_SERVER $ROUTER1 "$ZALOHA3"

exit 0
