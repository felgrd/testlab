#!/bin/bash

#  Test SMS3
#
#  @param $1: ID of router.
#  @param $2: ID of release.

# Pro tento test je nutne ulozit do SIM karty telefoni cislo
# ulozeni cisla:
# gsmat AT+CPBS="ON"
# gsmat AT+CPBW=1,"<tel.cislo SIM karty>"


# Kontrola parametru adresa testovaneho routeru
if [ -z $1 ];then
  echo "Missing parameter address of router" 1>&2
  exit 1
fi

# IP adresa prvniho routeru
ROUTER1=$1

# Vycteni tel.cisla
CISLO=$(tl_remote $1 "gsmat AT+CNUM")
CISLO=${CISLO:12:13}

# Nastaveni konfigurace
echo "SMS_RECV_PHONE_NO1=$CISLO" > /var/testlab/conf/sms/1/sms.cfg

# Nahrani konfigurace s SMS
tl_updateconf -f sms -t sms $1

# Restartovani PPP
tl_remote $1 "service ppp restart" >/dev/null

tl_mobileready $ROUTER1

echo "Cislo: $CISLO"

echo cislo | hexdump

TEXTSMS="get ip"
tl_remote $1 "sms $CISLO ahoj"
