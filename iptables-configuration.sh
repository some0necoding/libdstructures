#!/bin/bash

#***********************************************************#
#															#
#				 iptables-configuration.sh					#
#				     September 21, 2022						#
#															#
#		    Script file to configure iptables.   			#
#															#
#***********************************************************#

IPTABLES_RULES="/etc/iptables/iptables.rules"

# create TCP and UDP chains
create_chains () {
    sudo iptables -N TCP
    sudo iptables -N UDP
}

# set default policy of INPUT FORWARD and DROP chains
set_policy () {
    sudo iptables -P INPUT DROP
    sudo iptables -P FORWARD DROP
    sudo iptables -P OUTPUT ACCEPT
}

# set custom firewall rules
set_rules () {
    sudo iptables -A INPUT -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
    sudo iptables -A INPUT -i lo -j ACCEPT
    sudo iptables -A INPUT -m conntrack --ctstate INVALID -j DROP
    sudo iptables -A INPUT -p icmp -m icmp --icmp-type 8 -m conntrack --ctstate NEW -j ACCEPT
    sudo iptables -A INPUT -p udp -m conntrack --ctstate NEW -j UDP
    sudo iptables -A INPUT -p tcp -m tcp --tcp-flags FIN,SYN,RST,ACK SYN -m conntrack --ctstate NEW -j TCP
    sudo iptables -A INPUT -p udp -j REJECT --reject-with icmp-port-unreachable
    sudo iptables -A INPUT -p tcp -j REJECT --reject-with tcp-reset
    sudo iptables -A INPUT -j REJECT --reject-with icmp-proto-unreachable
    sudo iptables -A TCP -p tcp -m tcp --dport 80 -j DROP
    sudo iptables -A TCP -p tcp -m tcp --dport 8080 -j DROP
    sudo iptables -A TCP -p tcp -m tcp --dport 443 -j ACCEPT
    sudo iptables -A TCP -p tcp -m tcp --dport 22 -j DROP
    sudo iptables -A TCP -p tcp -m tcp --dport 53 -j ACCEPT
    sudo iptables -A UDP -p udp -m udp --dport 53 -j ACCEPT
}

# apply changes system-wide and start/enable iptables
apply_changes () {
    sudo iptables-save -f $IPTABLES_RULES
    sudo systemctl start iptables
    sudo systemctl enable iptables
}

run_iptables_config () {

    # ceate needed chains
    create_chains

    # set default policy
    set_policy

    # set custom rules
    set_rules

    # save changes
    apply_changes

    exit 0
}

run_iptables_config