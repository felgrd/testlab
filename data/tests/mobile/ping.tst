#!/bin/bash

## @file ping.tst
## @brief Test ping.tst tests ping to mobile wan.
##
## @author David Felgr
## @version 1.0.0
##
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - Packet received. <br>
##          1 - Packet loss.


# Ukonceni skriptu v pripade chyby
function ping_error {
  echo $1 1>&2
  exit 1
}

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  ping_error "Missing parameter address of router"
fi

#IP adresa prvniho routeru
ROUTER1=$1

# Zmena APN na conel.agnep.cz
tl_changeparam -f ppp -p APN $ROUTER1 conel.agnep.cz
if [ $? -ne 0 ]; then
  ping_error "Router does not change parameter APN."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  ping_error "Router does not restart service ppp."
fi

# Cekani na spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  ping_error "Router does not connect to mobile network."
fi

# Ping na vychozi branu
tl_remote $ROUTER1 "ping -c 5 10.0.0.1" >/dev/null
if [ $? -ne 0 ]; then
  ping_error "Router does not ping to 10.0.0.1."
fi

# Uspesne ukonceni skriptu
exit 0
