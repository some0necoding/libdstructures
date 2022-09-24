#!/bin/bash

sudo pacman -Sy clamav

sudo freshclam

# To keep up-to-date virus definitions
sudo systemctl start clamav-freshclam.service
sudo systemctl enable clamav-freshclam.service

# To have clamav always running on system
sudo systemctl start clamav-daemon.service
sudo systemctl enable clamav-daemon.service

# Adding more databases
sudo su
mkdir -m 0770 -p /var/lib/fangfrisch
chgrp clamav /var/lib/fangfrisch
cd /var/lib/fangfrisch
python3 -m venv venv
source venv/bin/activate
pip install fangfrisch