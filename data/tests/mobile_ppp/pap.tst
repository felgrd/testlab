#!/bin/bash

## @file pap.tst
## @brief Test pap.tst tests authentication methods pap.
##
## @author David Felgr
## @version 1.0.0
##
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - Router is connected to mobile network. <br>
##          1 - Router is not connected to mobile network.

# Ukonceni skriptu v pripade chyby
function error {
  tl_paramchange -f ppp -p auth $ROUTER1 "$B_AUTH"
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
B_AUTH=$(tl_paraminfo -f ppp -p auth $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter auth." 1>&2
  exit 1
fi

# Zmena autentizace na PAP
tl_paramchange -f ppp -p auth $ROUTER1 1
if [ $? -ne 0 ]; then
  error "Router does not change parameter Authentication."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Cekani na sestaveni spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not connect to mobile network."
fi

# Kontrola hlasky CHAP autentizace v logu
tl_slog -p "PAP authentication succeeded" $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "PAP authentication is not succeeded."
fi

# Vraceni nastaveni
tl_paramchange -f ppp -p auth $ROUTER1 "$B_AUTH"
if [ $? -ne 0 ]; then
  error "Router does not change parameter Authentication."
fi

# Uspesne ukonceni testu
exit 0
