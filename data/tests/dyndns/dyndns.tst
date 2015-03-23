#!/bin/bash

#  Test DynDNS
#  
#  @param $1: ID of router.
#  @param $2: ID of release.
#
# Registrace novych routeru na www.dnsdynamic.org
# username: ctestdyndns@gmail.com
# password: C0n3LUst1

# Ukonceni skriptu v pripade chyby
function error {
  	tl_paramchange -f DYNDNS -p ENABLED $ROUTER1 "$ZALOHA1"
	tl_paramchange -f DYNDNS -p HOSTNAME $ROUTER1 "$ZALOHA2"
	tl_paramchange -f DYNDNS -p USERNAME $ROUTER1 "$ZALOHA3"
	tl_paramchange -f DYNDNS -p PASSWORD $ROUTER1 "$ZALOHA4"
	tl_paramchange -f DYNDNS -p SERVER $ROUTER1 "$ZALOHA5"
	tl_paramchange -f PPP -p APN $ROUTER1 "$ZALOHA6"
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

# zaloha konfigurace
ZALOHA1=$(tl_paraminfo -f DYNDNS -p ENABLED $ROUTER1)
ZALOHA2=$(tl_paraminfo -f DYNDNS -p HOSTNAME $ROUTER1)
ZALOHA3=$(tl_paraminfo -f DYNDNS -p USERNAME $ROUTER1)
ZALOHA4=$(tl_paraminfo -f DYNDNS -p PASSWORD $ROUTER1)
ZALOHA5=$(tl_paraminfo -f DYNDNS -p SERVER $ROUTER1)
ZALOHA6=$(tl_paraminfo -f PPP -p APN $ROUTER1)

# nahrani konfigurace s DynDNS
tl_paramchange -f DYNDNS -p ENABLED $ROUTER1 1
tl_paramchange -f DYNDNS -p HOSTNAME $ROUTER1 "conel-$ROUTER1.dnsd.info "
tl_paramchange -f DYNDNS -p USERNAME $ROUTER1 "ctestdyndns@gmail.com"
tl_paramchange -f DYNDNS -p PASSWORD $ROUTER1 "C0n3LUst1"
tl_paramchange -f DYNDNS -p SERVER $ROUTER1 "www.dnsdynamic.org "
tl_paramchange -f PPP -p APN $ROUTER1 ""
if [ $? != 0 ];then
	error "Update of configuration has failed"
fi

# Vyckani na navazani mobile spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? != 0 ];then
	error "PPP conection is not estabished"
fi

# restart PPP
tl_remote $1 "service ppp restart" &> /dev/null
if [ $? != 0 ];then
	error "Restart ppp has failed"
fi

# restart dyndns
tl_remote $1 "service dyndns restart" &> /dev/null
if [ $? != 0 ];then
	error "Restart dyndns has failed"
fi

# Vyckani na navazani mobile spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? != 0 ];then
	error "PPP conection is not estabished"
fi

# Pokus o ping na router
for (( a=1; $a-30; a=$a+1 ))
do
	# Ping na domenu routeru
	tl_remote $1 "ping -c 1 conel-$ROUTER1.dnsd.info" &> /dev/null
	
	# Ulozeni vysledku pingu
	RESULT=$?

	# Kontrola uspesnosti pingu
	if [ $RESULT -eq 0 ]; then
		break
	fi
	sleep 1
done

# nahrani puvodni konfigurace
tl_paramchange -f DYNDNS -p ENABLED $ROUTER1 "$ZALOHA1"
tl_paramchange -f DYNDNS -p HOSTNAME $ROUTER1 "$ZALOHA2"
tl_paramchange -f DYNDNS -p USERNAME $ROUTER1 "$ZALOHA3"
tl_paramchange -f DYNDNS -p PASSWORD $ROUTER1 "$ZALOHA4"
tl_paramchange -f DYNDNS -p SERVER $ROUTER1 "$ZALOHA5"
tl_paramchange -f PPP -p APN $ROUTER1 "$ZALOHA6"

# Kontrola vysledku
if [ $RESULT -eq "0" ];then
	exit 0
else	
	
	echo "DynDNS doesn't work" 1>&2
	exit 1
fi
