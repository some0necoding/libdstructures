#!/bin/bash

FANGFRISCH_CONF="fangfrisch.conf"
LOCAL_FANGFRISCH_CONF_DIR="/home/marco/coding/bash/linux_install_scripts/config"
FANGFRISCH_CONF_DIR="/etc"
ENV_DIR="/var/lib/fangfrisch"
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
            return 1
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
        return 1
    fi

    # Enabling clamav-freshclam.service
    (systemctl enable clamav-freshclam.service) > /dev/null 2>&1; ret_value=$?

    # Error checking
    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot enable clamav-freshclam.service"
        return 1
    fi

    # Check wether the user wants to enable the daemon or not
    echo "Do you want to enable the clamav daemon? (not needed for stand-alone scans) [y/N]: "
    read daemon

    if [[ $daemon == "y" || $daemon == "Y" ]]; then
        
        (systemctl start clamav-daemon.service) > /dev/null 2>&1; ret_value=$?

        # Error checking
        if [[ $ret_value != 0 ]]; then
            echo "Error: cannot start clamav-daemon.service"
            return 1
        fi

        (systemctl enable clamav-daemon.service) > /dev/null 2>&1; ret_value=$?
    
        # Error checking
        if [[ $ret_value != 0 ]]; then
            echo "Error: cannot enable clamav-daemon.service"
            return 1
        fi
    fi

    return 0
}

# Create directory for python environment
create_env_dir () {

    local ret_value

    echo "Creating $ENV_DIR directory..."
    
    mkdir -m 0770 -p $ENV_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return 1
    
    chgrp clamav $ENV_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    return 0
}

# Install fangfrisch inside env
install_fangfrisch () {

    local ret_value

    cd $ENV_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    python3 -m venv venv; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    source venv/bin/activate; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    pip install fangfrisch; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    deactivate; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    return 0
}

# Setup fangfrisch.conf file
setup_fangfrisch_conf () {

    local ret_value

    echo "Setting up configuration file in /etc/fangfrisch.conf"
    cp -r "$LOCAL_FANGFRISCH_CONF_DIR/$FANGFRISCH_CONF" $FANGFRISCH_CONF_DIR; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    sudo -u clamav -- "$ENV_DIR/venv/bin/fangfrisch" --conf "$FANGFRISCH_CONF_DIR/$FANGFRISCH_CONF" initdb; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    (systemctl enable fangfrisch.timer) > /dev/null 2>&1; ret_value=$?
    
    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot enable fangfrisch.timer"
        return 1
    fi

    return 0
}

# Adding fangfrisch databases
fangfrisch_setup () {

    local ret_value

    echo "Creating needed directories..."
    create_env_dir; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    echo "Installing fangfrisch"
    install_fangfrisch; ret_value=$?
    [[ $ret_value != 0 ]] && return 1
    echo "Done"

    echo "Configuring fangfrisch"
    setup_fangfrisch_conf; ret_value=$?
    [[ $ret_value != 0 ]] && return 1

    return 0
}

run_setup () {

    local ret_value

    check_root; ret_value=$?
    [[ $ret_value != 0 ]] && exit 1

    clamav_setup; ret_value=$?
    [[ $ret_value != 0 ]] && exit 1

    fangfrisch_setup; ret_value=$?
    [[ $ret_value != 0 ]] && exit 1

    echo "Done"

    return 0
}

run_setup