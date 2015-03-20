#!/bin/bash

## @file type_umts.tst
## @brief Test type_umts.tst tests select network type UMTS.
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
  tl_paramchange -f ppp -p nettype $ROUTER1 "$B_NETTYPE"
  echo $1 1>&2
  exit 1
}

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

# IP adresa prvniho routeru
ROUTER1=$1

# Zalohovani nastaveni ppp nettype
B_NETTYPE=$(tl_paraminfo -f ppp -p nettype $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter nettype." 1>&2
  exit 1
fi

# Zmena typu site na UMTS
tl_paramchange -f ppp -p nettype $ROUTER1 2
if [ $? -ne 0 ]; then
  error "Router does not change parameter Network type."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Cekani na sestaveni spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not connect to UMTS mobile network."
fi

# Zjisteni nastaveneho typu site
NETTYPE=$(tl_status $ROUTER1 mobile Technology)
if [ $? -ne 0 ] || [ -z $NETTYPE ]; then
  error "Router has not selected network type."
fi

# Kontrola nastavene typu site
if [ $NETTYPE != "UMTS" ]; then
  error "Selected network type is not UMTS, but network type is $NETTYPE."
fi

# Vraceni nastaveni
tl_paramchange -f ppp -p nettype $ROUTER1 "$B_NETTYPE"
if [ $? -ne 0 ]; then
  error "Router does not change parameter Network type."
fi

# Uspesne ukonceni testu
exit 0
