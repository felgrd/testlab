#!/bin/bash

## @file operator.tst
## @brief Test operator.tst tests select operator for mobile network.
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
  tl_paramchange -f ppp -p operator $ROUTER1 "$B_OPERATOR"
  echo $1 1>&2
  exit 1
}

# Inicializace defaultnich hodnot
B_OPERATOR=""

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

# Identifikator prvniho routeru
ROUTER1=$1

# Zalohovani konfigurace operator mobilniho spojeni
B_OPERATOR=$(tl_paraminfo -f ppp -p operator $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter operator." 1>&2
  exit 1
fi

# Zjisteni operatora SIM karty
SIM_OPERATOR=$(tl_sim -o $ROUTER1)
if [ $? -ne 0 ] || [ -z $SIM_OPERATOR ]; then
  echo "Error with check operator of SIM card ($SIM_OPERATOR)." 1>&2
  exit 1
fi

# Zmena operatora mobilniho spojeni
tl_paramchange -f ppp -p operator $ROUTER1 "$SIM_OPERATOR"
if [ $? -ne 0 ]; then
  error "Router does not change parameter operator."
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

# Vraceni hodnot parametru operator mobilniho spojeni
tl_paramchange -f ppp -p operator $ROUTER1 "$B_OPERATOR"
if [ $? -ne 0 ]; then
  error "Router does not change parameter operator."
fi

# Zjisteni prirazeneho operatora mobilniho spojeni
M_OPERATOR=$(tl_status $ROUTER1 mobile PLMN)
if [ $? -ne 0 ] || [ -z $M_OPERATOR ]; then
  error "Error with check operator of mobile wan connection"
fi

# Kontrola prirazene adresy adresy
if [ $SIM_OPERATOR != $M_OPERATOR ]; then
  error "Assigned operator address is not operator of SIM card."
fi

# Ukonceni skriptu
exit 0
