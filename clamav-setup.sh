#!/bin/bash

#***********************************************************#
#															#
#				       clamav-setup.sh  					#
#				     September 21, 2022						#
#															#
#		      Script file to configure clamav.   			#
#															#
#***********************************************************#

PID=$$
RUNNING_DIR=$(pwdx $PID | cut -d " " -f 2)

FANGFRISCH_CONF="fangfrisch.conf"
LOCAL_CONF_DIR="$RUNNING_DIR/config"
LOCAL_UNIT_DIR="$RUNNING_DIR/units"
FANGFRISCH_CONF_DIR="/etc"
ENV_DIR="/var/lib/fangfrisch"

SYSTEMD_PATH="/etc/systemd/system"
FANGFRISCH_SERVICE="fangfrisch.service"
FANGFRISCH_TIMER="fangfrisch.timer"

ROOT_UID=0

check_root () {
    if [[ $UID != $ROOT_UID ]]; then
        echo "You must run this script as root"
        return 1
    fi

    return 0
}

clamav_setup () {

    local daemon
    local ret_value

    # Look for clamav package
    echo "Looking for clamav package..."
    (pacman -Ss clamav) > /dev/null 2>&1; ret_value=$?
    
    # Install clamav if needed
    if [[ $ret_value != 0 ]]; then
        echo "Installing clamav..."
        pacman -Sy clamav; ret_value=$?
        if [[ $ret_value != 0 ]]; then
            echo "Error: cannot install clamav package"
            return $ret_value
        else
            echo "Done"
        fi
    fi

    # Update virus definitions
    echo "Updating virus definitions..."
    freshclam

    # To always update virus definitions
    echo "Starting clamav-freshclam.service"
    (systemctl start clamav-freshclam.service) > /dev/null 2>&1; ret_value=$?

    # Error checking
    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot start clamav-freshclam.service"
        return $ret_value
    fi

    # Enabling clamav-freshclam.service
    (systemctl enable clamav-freshclam.service) > /dev/null 2>&1; ret_value=$?

    # Error checking
    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot enable clamav-freshclam.service"
        return $ret_value
    fi

    # Check wether the user wants to enable the daemon or not
    echo "Do you want to enable the clamav daemon? (not needed for stand-alone scans) [y/N]: "
    read daemon

    if [[ $daemon == "y" || $daemon == "Y" ]]; then
        
        (systemctl start clamav-daemon.service) > /dev/null 2>&1; ret_value=$?

        # Error checking
        if [[ $ret_value != 0 ]]; then
            echo "Error: cannot start clamav-daemon.service"
            return $ret_value
        fi

        (systemctl enable clamav-daemon.service) > /dev/null 2>&1; ret_value=$?
    
        # Error checking
        if [[ $ret_value != 0 ]]; then
            echo "Error: cannot enable clamav-daemon.service"
            return $ret_value
        fi
    fi

    return 0
}

# Create directory for python environment
create_env_dir () {

    local ret_value

    echo "Creating $ENV_DIR directory..."
    
    mkdir -m 0770 -p $ENV_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value
    
    chgrp clamav $ENV_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    return 0
}

# Install fangfrisch inside env
install_fangfrisch () {

    local ret_value

    cd $ENV_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    python3 -m venv venv; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    source venv/bin/activate; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    pip install fangfrisch; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    deactivate; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    return 0
}

# Setup fangfrisch.conf file
setup_fangfrisch_conf () {

    local ret_value

    # Copy fangfrisch.conf into /etc directory
    echo "Setting up configuration file in /etc/fangfrisch.conf"
    cp "$LOCAL_CONF_DIR/$FANGFRISCH_CONF" $FANGFRISCH_CONF_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    # Set up fangfrisch
    sudo -u clamav -- "$ENV_DIR/venv/bin/fangfrisch" --conf "$FANGFRISCH_CONF_DIR/$FANGFRISCH_CONF" initdb

    # Copy fangfrisch.service into /etc/systemd/system
    cp "$LOCAL_UNIT_DIR/$FANGFRISCH_SERVICE" "$SYSTEMD_PATH"; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    # Copy fangfrisch.timer into /etc/systemd/system
    cp "$LOCAL_UNIT_DIR/$FANGFRISCH_TIMER" "$SYSTEMD_PATH"; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    # Starting fangfrisch.timer
    (systemctl start fangfrisch.timer) > /dev/null 2>&1; ret_value=$?

    # Error checking
    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot start fangfrisch.timer"
        return $ret_value
    fi

    # Enabling fangfrisch.timer
    (systemctl enable fangfrisch.timer) > /dev/null 2>&1; ret_value=$?
    
    # Error checking
    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot enable fangfrisch.timer"
        return $ret_value
    fi

    return 0
}

# Adding fangfrisch databases
fangfrisch_setup () {

    local ret_value

    echo "Creating needed directories..."
    create_env_dir; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    echo "Installing fangfrisch"
    install_fangfrisch; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value
    echo "Done"

    echo "Configuring fangfrisch"
    setup_fangfrisch_conf; ret_value=$?
    [[ $ret_value != 0 ]] && return $ret_value

    return 0
}

run_setup () {

    local ret_value

    check_root; ret_value=$?
    [[ $ret_value != 0 ]] && exit $ret_value

    clamav_setup; ret_value=$?
    [[ $ret_value != 0 ]] && exit $ret_value

    fangfrisch_setup; ret_value=$?
    [[ $ret_value != 0 ]] && exit $ret_value

    echo "Done"

    return 0
}

run_setup