#!/bin/bash

## @file address.tst
## @brief Test address.tst tests select IP address for mobile network.
##
## @author David Felgr
## @version 1.0.0
##
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - Router is connected to UMTS network. <br>
##          1 - Router is not connected to UMTS network.

# Ukonceni skriptu v pripade chyby
function error {
  tl_paramchange -f ppp -p ipaddr $ROUTER1 "$B_IP"
  tl_paramchange -f ppp -p apn $ROUTER1 "$B_APN"
  echo $1 1>&2
  exit 1
}

# Inicializace defaultnich hodnot
B_IP=""
B_APN="conel.agnep.cz"

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

# IP adresa prvniho routeru
ROUTER1=$1

# Zalohovani konfigurace IP adresy
B_IP=$(tl_paraminfo -f ppp -p ipaddr $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter ipaddr." 1>&2
  exit 1
fi

# Zalohovani konfigurace APN
B_APN=$(tl_paraminfo -f ppp -p apn $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter apn." 1>&2
  exit 1
fi

# Zjisteni adresy SIM karty
SIM_IP=$(tl_sim -i $ROUTER1)
if [ $? -ne 0 ] || [ -z $SIM_IP ]; then
  echo "Error with check IP address of SIM card ($SIM_IP)." 1>&2
  exit 1
fi

# Zmena IP adresy mobilniho spojeni
tl_paramchange -f ppp -p ipaddr $ROUTER1 "$SIM_IP"
if [ $? -ne 0 ]; then
  error "Router does not change parameter ipaddr."
fi

# Zmena APN
tl_paramchange -f ppp -p apn $ROUTER1 conel.agnep.cz
if [ $? -ne 0 ]; then
  error "Router does not change parameter APN."
fi

# Restart mobilniho spojeni
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Cekani na sestaveni mobilniho spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not connect to mobile network."
fi

# Vraceni hodnot parametru IP adresa
tl_paramchange -f ppp -p ipaddr $ROUTER1 "$B_IP"
if [ $? -ne 0 ]; then
  error "Router does not change parameter ipaddr."
fi

# Vraceni Hodnot parametru APN
tl_paramchange -f ppp -p apn $ROUTER1 "$B_APN"
if [ $? -ne 0 ]; then
  error "Router does not change parameter APN."
fi

# Zjisteni prirazene IP adresy mobilniho spojeni
M_IP=$(tl_status $ROUTER1 ppp "IP Address")
if [ $? -ne 0 ] || [ -z $SIM_IP ]; then
  error "Error with check IP address of mobile wan connection"
fi

# Kontrola prirazene adresy adresy
if [ $SIM_IP != $M_IP ]; then
  error "Assigned IP address is not IP address of SIM card."
fi

# Ukonceni skriptu
exit 0
