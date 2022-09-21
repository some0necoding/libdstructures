#!/bin/bash

#***********************************************************#
#															#
#					power-management.sh						#
#					 September 21, 2022						#
#															#
#		  Script file to set up power management.			#
#															#
#***********************************************************#

# check powertop utility existance
check_powertop () {

    local ret_value

	echo "Looking for powertop utility..."

    (pacman -Ss powertop) >> /dev/null 2>&1

    ret_value=$?

    # install powertop if it's not
    if [[ $ret_value != 0 ]]; then
        install_powertop
        return $?
    else
        return 0
    fi
}

# install powertop utility
install_powertop () {

    local ret_value

    echo "Installing powertop utility..."

    sudo pacman -Sy powertop

    ret_value=$?

    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot download powertop package"
        return $ret_value
    else
        echo "Done"
        return 0
    fi
}

# tune power managemente settings
tune_settings () {

    local ret_value

    echo "Tuning..."

    # use powertop to tweak settings
    (sudo powertop --auto-tune) >> /dev/null 2>&1

    ret_value=$?

    if [[ $ret_value != 0 ]]; then
        echo "Error: something went wrong"
        return $ret_value
    else
        echo "Done"
        return 0
    fi
}

# main function
run_configuration () {

    local ret_value

    # check for powertop existance
    check_powertop

    ret_value=$?; [[ $ret_value != 0 ]] && exit $ret_value

    # tune power management settings
    tune_settings
    exit $?
}

run_configuration