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

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

#IP adresa prvniho routeru
ROUTER1=$1

# Zmena APN na conel.agnep.cz
tl_changeparam -c ppp -p APN $ROUTER1 conel.agnep.cz

if[ $? != 0 ]; then
  echo "Router does not change parameter APN." 1>&2
  exit 1
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null

if[ $? != 0 ]; then
  echo "Router does not restart service ppp." 1>&2
  exit 1
fi

# Cekani na spojeni
tl_mobileready $ROUTER1 >/dev/null

# Kontrola pripojeni do mobilni site
if[ $? != 0 ]; then
  echo "Router does not connect to mobile network." 1>&2
  exit 1
fi

# Ping na vychozi branu
tl_remote $ROUTER1 "ping -c 1 10.0.0.1" >/dev/null

# Kontrola pingu na vychozi branu
if[ $? != 0 ]; then
  echo "Router does not ping to 10.0.0.1." 1>&2
  exit 1
fi

# Ukonceni skriptu
exit 0
