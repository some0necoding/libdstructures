#!/bin/bash

#***********************************************************#
#															#
#					  no-hibernate.sh						#
#					September 21, 2022						#
#															#
#		    Script file to disable hibernation. 			#
#															#
#***********************************************************#

LOGIND_CONFIG_FILE="./bin/no-hibernate-1.conf"
SLEEP_CONFIG_FILE="./bin/no-hibernate-2.conf"

LOGIND_DIR="/etc/systemd/logind.conf.d"
SLEEP_DIR="/etc/systemd/sleep.conf.d"

FILE_NAME="no-hibernate.conf"

copy_files () {

    echo "Creating directories..."

    sudo mkdir -p $LOGIND_DIR $SLEEP_DIR

    echo "Copying files..."

    sudo cp $LOGIND_CONFIG_FILE "$LOGIND_DIR/$FILE_NAME"
    sudo cp $SLEEP_CONFIG_FILE "$SLEEP_DIR/$FILE_NAME"

    echo "Done"
}

copy_files