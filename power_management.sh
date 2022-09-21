#!/bin/bash

check_powertop () {

    local ret_value

    echo "Looking for powertop utility..."

    (pacman -Ss powertop) >> /dev/null 2>&1

    ret_value=$?

    if [[ $ret_value != 0 ]]; then
        install_powertop
        return $?
    else
        return 0
    fi
}

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

tune_settings () {

    local ret_value

    echo "Tuning..."

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

run_configuration () {

    local ret_value

    check_powertop

    ret_value=$?; [[ $ret_value != 0 ]] && exit $ret_value

    tune_settings
    exit $?
}

run_configuration