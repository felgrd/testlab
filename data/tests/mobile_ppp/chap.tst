#!/bin/bash

## @file chap.tst
## @brief Test chap.tst tests authentication methods chap
##
## @author David Felgr
## @version 1.0.0
##
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - Router is connected to mobile network. <br>
##          1 - Router is not connected to mobile network.

# Ukonceni skriptu v pripade chyby
function chap_error {
  tl_paramchange -f ppp -p auth $ROUTER1 0
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

# Zmena autentizace na CHAP
tl_paramchange -f ppp -p auth $ROUTER1 2
if [ $? -ne 0 ]; then
  chap_error "Router does not change parameter Authentication."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  chap_error "Router does not restart service ppp."
fi

# Cekani na sestaveni spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  chap_error "Router does not connect to mobile network."
fi

# Kontrola hlasky CHAP autentizace v logu
tl_slog -p "CHAP authentication succeeded" $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  chap_error "Router does not connect to mobile network."
fi

# Vraceni nastaveni
tl_paramchange -f ppp -p auth $ROUTER1 0
if [ $? -ne 0 ]; then
  chap_error "Router does not change parameter Authentication."
fi

# Uspesne ukonceni testu
exit 0
