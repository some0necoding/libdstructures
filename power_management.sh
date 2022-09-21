#!/bin/bash

powertop_install () {

    local ret_value

    echo "Installing powertop utility..."

    sudo pacman -Sy powertop

    ret_value=$?

    if [[ $ret_value != 0 ]]; then
        echo "Error: cannot download powertop package"
        return $ret_value
    fi

    echo "Done"
    return 0
}

powertop_tuning () {

    local ret_value

    echo "Tuning..."

    (sudo powertop --auto-tune) >> /dev/null 2>&1

    ret_value=$?

    if [[ $ret_value != 0 ]]; then
        echo "Error: something went wrong"
        return $ret_value
    fi

    echo "Done"
    return 0
}

power_management_config () {

    local ret_value

    powertop_install

    ret_value=$?; [[ $ret_value != 0 ]] && return ret_value

    powertop_tuning

    ret_value=$?; [[ $ret_value != 0 ]] && return ret_value || return 0

}

power_management_config