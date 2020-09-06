#!/bin/sh
BUILD_NAME="vsftpd"
CONTAINER_NAME="agilent-e2960-vsftpd"

if [ -z "$(docker images | grep $BUILD_NAME)" ]
then
	docker build --tag $BUILD_NAME:latest .
fi

if [ -z "$(docker container ls -a | grep agilent-e2960-dhcpd)" ]
then
	docker run -d \
	  -it --rm --init --net host \
	  --name agilent-e2960-dhcpd \
	  -v "$(pwd)/data":/data \
	  networkboot/dhcpd
fi

if [ -z "$(docker container ls -a | grep $CONTAINER_NAME)" ]
then
	docker run -d \
	  -it --rm --init --net host\
	  --name $CONTAINER_NAME \
	  -v "$(pwd)/ftproot":/srv/ftp \
	  $BUILD_NAME:latest
else
	docker start -a $CONTAINER_NAME
fi
