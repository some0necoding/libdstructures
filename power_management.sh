#!/bin/bash

REPORT_FILE="./powerreport.csv"
TUNINGS_START="Software Settings in Need of Tuning"

powertop_install () {

    echo "Installing powertop utility..."

    sudo pacman -Sy powertop

    echo "Done"
}

powertop_scan () {

    echo "Scanning for tweakings..."

    (sudo powertop -q --csv=$REPORT_FILE) >> /dev/null 2>&1

    echo "Done"
}

get_tunings () {

    local line

    while [[ read line ]]; do 
        if [[ grep $TUNINGS_START $line ]]; 


}

# install powertop
# store powertop scan in csv format
# isolate needed tunings
# perform tunings