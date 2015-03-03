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
  tl_paramchange -f ppp -p phone $ROUTER1 "$B_NUMBER"
  echo $1 1>&2
  exit 1
}

# Inicializace defaultnich hodnot
B_NUMBER=""
RESULT=1

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

# IP adresa prvniho routeru
ROUTER1=$1

# Zalohovani konfigurace telefoniho cisla
B_NUMBER=$(tl_paraminfo -f ppp -p phone $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter phone." 1>&2
  exit 1
fi

# Zjisteni telefoniho cisla SIM karty
SIM_NUMBER=$(tl_sim -n $ROUTER1)
if [ $? -ne 0 ] || [ -z $SIM_NUMBER ]; then
  echo "Error with check IP address of SIM card ($SIM_IP)." 1>&2
  exit 1
fi

# Zmena telefoniho cisla mobilniho spojeni
tl_paramchange -f ppp -p phone $ROUTER1 "$SIM_NUMBER"
if [ $? -ne 0 ]; then
  error "Router does not change parameter ipaddr."
fi

# Restart mobilniho spojeni
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Vraceni hodnot parametru IP adresa
tl_paramchange -f ppp -p phone $ROUTER1 "$B_NUMBER"
if [ $? -ne 0 ]; then
  error "Router does not change parameter phone."
fi

# Kontrola zadani cisla pro vytaceni spojeni
for a in $( seq 5 ); do
  # Vycteni systemoveho logu
  tl_slog -p "send (\dATD$SIM_NUMBER^M)" $ROUTER1 >/dev/null
  if [ $? -eq 0 ]; then
    RESULT=0
    break
  fi

  # Cekani na odeslani cisla
  sleep 5
done

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


# Ukonceni skriptu
exit $RESULT
