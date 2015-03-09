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
  tl_paramchange -f ppp -p mtu $ROUTER1 $B_MTU
  echo $1 1>&2
  exit 1
}

# Inicializace testovacich hodnot
TEST_MTU=500
TEST_SIZE=1000
B_MTU=1500

# Kontrola parametru adresy testovaneho routeru
if [ -z $1 ]; then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

#IP adresa prvniho routeru
ROUTER1=$1

# Zalohovani nastaveni ppp mtu
B_MTU=$(tl_paraminfo -f ppp -p mtu $ROUTER1)
if [ $? -ne 0 ]; then
  echo "Error with check value of parameter MTU." 1>&2
  exit 1
fi

# Nastaveni parametru ppp mtu
tl_paramchange -f ppp -p mtu $ROUTER1 "$TEST_MTU"
if [ $? -ne 0 ]; then
  error "Router does not change parameter MTU."
fi

# Restart mobilniho spojeni
tl_remote $ROUTER1 "service ppp restart" >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not restart service ppp."
fi

# Cekani na spojeni
tl_mobileready $ROUTER1 >/dev/null
if [ $? -ne 0 ]; then
  error "Router does not connect to mobile network."
fi

# Cekani na ustaleni spojeni pred pingem
sleep 2

# Zjisteni interfacu mobilniho spojeni
M_INT=$(tl_status $ROUTER1 ppp Interface)
if [ $? -ne 0 ] || [ -z $M_INT ] || [ $M_INT == "N/A" ]; then
  error "Error with check interface of mobile connection ($M_INTs)."
fi

# Spusteni pingu z routeru na do mobilni site
tl_pingb -c 5 -s $TEST_SIZE $ROUTER1 10.0.0.1
if [ $? -ne 0 ]; then
  error "Router does not start ping to mobile network."
fi

# Spravne nastaveni MTU v tcpdumpu
PATTERN="IP \(.*length $TEST_MTU\)"

# Monitorovani dat na mobilni siti
tl_tcpdump -i $M_INT -t 3 $ROUTER1 "icmp[icmptype]==8" | grep -q "$PATTERN"
if [ $? -ne 0 ]; then
  tl_tcpdump -r $ROUTER1 1>&2
  error "Required traffic is not found in tcpdump."
fi

# Nastaveni parametru ppp mtu
tl_paramchange -f ppp -p mtu $ROUTER1 "$B_MTU"
if [ $? -ne 0 ]; then
  error "Router does not change parameter MTU."
fi

exit 0
