#!/bin/sh

PREFIX=neptune
NETIF=eth0

MAC_ADDRESS=`ip addr show ${NETIF} | grep link/ether | awk '{print $2}'`
MAC4=`echo $MAC_ADDRESS | awk -F ":" '{print $4}'`
MAC5=`echo $MAC_ADDRESS | awk -F ":" '{print $5}'`
MAC6=`echo $MAC_ADDRESS | awk -F ":" '{print $6}'`

MACHINE_NAME=${PREFIX}-${MAC4}${MAC5}${MAC6}

#echo $MACHINE_NAME

CURRENT_NAME=`hostname`

echo "Hostname:"${CURRENT_NAME}

if [ "${CURRENT_NAME}" != "${MACHINE_NAME}" ]; then 
    echo "Hostname unmatch, change to:" ${MACHINE_NAME}
    echo ${MACHINE_NAME} > /etc/hostname
    hostname -f /etc/hostname 
fi 

