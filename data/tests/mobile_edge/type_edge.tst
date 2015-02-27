#!/bin/bash

## @file type_edge.tst
## @brief Test type_edge.tst tests select network type EDGE.
##
## @author David Felgr
## @version 1.0.0
##
## @param $1: ID of router.
## @param $2: ID of release.
## @returns 0 - Router is connected to EDGE network. <br>
##          1 - Router is not connected to EDGE network.

# Ukonceni skriptu v pripade chyby
function edge_error {
  tl_changeparam -f ppp -p nettype $ROUTER1 0
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

# Zmena typu site na EDGE
tl_changeparam -f ppp -p nettype $ROUTER1 1
if [ $? -ne 0 ]; then
  edge_error "Router does not change parameter Network type."
fi

# Restart ppp
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  edge_error "Router does not restart service ppp."
fi

# Cekani na sestaveni spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  edge_error "Router does not connect to EDGE mobile network."
fi

# Zjisteni nastaveneho typu site
NETTYPE=$(tl_status $ROUTER1 mobile Technology)
if [ $? -ne 0 ] || [ -z $NETTYPE ]; then
  edge_error "Router has not selected network type."
fi

# Kontrola nastavene typu site
if [ $NETTYPE != "EDGE" ]; then
  edge_error "Selected network type is not EDGE, but network type is $NETTYPE."
fi

# Vraceni nastaveni
tl_changeparam -f ppp -p nettype $ROUTER1 0
if [ $? -ne 0 ]; then
  edge_error "Router does not change parameter Network type."
fi

# Uspesne ukonceni testu
exit 0
