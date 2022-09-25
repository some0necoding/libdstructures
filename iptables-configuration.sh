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
ROOT_UID=0

# Check for root
check_root () {
    if [[ $UID != $ROOT_UID ]]; then
        echo "You must run this script as root"
        return 1
    fi

    return 0
}

# Create TCP and UDP chains
create_chains () {
    iptables -N TCP
    iptables -N UDP
}

# Set default policy of INPUT FORWARD and DROP chains
set_policy () {
    iptables -P INPUT DROP
    iptables -P FORWARD DROP
    iptables -P OUTPUT ACCEPT
}

# Set custom firewall rules
set_rules () {
    iptables -A INPUT -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
    iptables -A INPUT -i lo -j ACCEPT
    iptables -A INPUT -m conntrack --ctstate INVALID -j DROP
    iptables -A INPUT -p icmp -m icmp --icmp-type 8 -m conntrack --ctstate NEW -j ACCEPT
    iptables -A INPUT -p udp -m conntrack --ctstate NEW -j UDP
    iptables -A INPUT -p tcp -m tcp --tcp-flags FIN,SYN,RST,ACK SYN -m conntrack --ctstate NEW -j TCP
    iptables -A INPUT -p udp -j REJECT --reject-with icmp-port-unreachable
    iptables -A INPUT -p tcp -j REJECT --reject-with tcp-reset
    iptables -A INPUT -j REJECT --reject-with icmp-proto-unreachable
    iptables -A TCP -p tcp -m tcp --dport 80 -j DROP
    iptables -A TCP -p tcp -m tcp --dport 8080 -j DROP
    iptables -A TCP -p tcp -m tcp --dport 443 -j ACCEPT
    iptables -A TCP -p tcp -m tcp --dport 22 -j DROP
    iptables -A TCP -p tcp -m tcp --dport 53 -j ACCEPT
    iptables -A UDP -p udp -m udp --dport 53 -j ACCEPT
}

# Apply changes system-wide and start/enable iptables
apply_changes () {
    iptables-save -f $IPTABLES_RULES
    systemctl start iptables
    systemctl enable iptables
}

# Main function
run_iptables_config () {

    # Checking for root
    check_root

    # Ceate needed chains
    create_chains

    # Set default policy
    set_policy

    # Set custom rules
    set_rules

    # Save changes
    apply_changes

    exit 0
}

run_iptables_config