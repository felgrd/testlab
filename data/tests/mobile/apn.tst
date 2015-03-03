#!/bin/bash

## @file apn.tst
## @brief Test ping.tst tests change APN to blank.
##
## @author David Felgr
## @version 1.0.0
##
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - APN is changed. <br>
##          1 - APN is not changed.

# Ukonceni skriptu v pripade chyby
function error {
  tl_paramchange -f ppp -p APN $ROUTER1 conel.agnep.cz
  echo $1 1>&2
  exit 1
}

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

#IP adresa prvniho routeru
ROUTER1=$1

# Zmena APN na conel.agnep.cz
tl_paramchange -f ppp -p APN $ROUTER1 ""
if [ $? -ne 0 ]; then
  error "Router does not change parameter APN."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Cekani na spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not connect to mobile network."
fi

# Zjisteni IP adresy prazdneho APN
IP_BLANK=$(tl_status $ROUTER1 ppp "IP Address")
if [ -z $IP_BLANK ] || [ $IP_BLANK == "Unassigned" ]; then
  error "Router has not assigned IP Address with empty APN."
fi

# Zmena APN na conel.agnep.cz
tl_paramchange -f ppp -p APN $ROUTER1 conel.agnep.cz
if [ $? -ne 0 ]; then
  error "Router does not change parameter APN."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Cekani na spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not connect to mobile network."
fi

# Zjisteni IP adresy prazdneho APN (internet)
IP_AGNEP=$(tl_status $ROUTER1 ppp "IP Address")
if [ -z $IP_AGNEP ] || [ $IP_AGNEP == "Unassigned" ]; then
  error "Router has not assigned IP Address with conel.agnep.cz APN."
fi

# Kontrola zmeny APN
if [ IP_BLANK == IP_AGNEP ]; then
  error "APN is not changed."
fi

# Uspesne ukonceni testu
exit 0
