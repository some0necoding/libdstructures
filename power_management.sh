#!/bin/bash

install_powertop () {

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

tune_settings () {

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

run_configuration () {

    local ret_value

    install_powertop

    ret_value=$?; [[ $ret_value != 0 ]] && return ret_value

    tune_settings

    ret_value=$?; [[ $ret_value != 0 ]] && return ret_value || return 0
}

run_configuration