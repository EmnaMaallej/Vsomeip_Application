#!/bin/bash

# Network Namespace Setup for SOME/IP Applications
# This script creates 3 namespaces: server-ns, client1-ns, client2-ns
# Connected via a bridge to enable multicast communication

echo "Creating network namespaces and bridge setup..."

# Clean up any existing setup
sudo ip netns del server-ns 2>/dev/null || true
sudo ip netns del client1-ns 2>/dev/null || true  
sudo ip netns del client2-ns 2>/dev/null || true
sudo ip link del br0 2>/dev/null || true

# Create namespaces
sudo ip netns add server-ns
sudo ip netns add client1-ns
sudo ip netns add client2-ns

# Create bridge
sudo ip link add name br0 type bridge
sudo ip link set br0 up
sudo ip addr add 192.168.0.254/24 dev br0

# Create veth pairs for each namespace
sudo ip link add server-veth type veth peer name server-br
sudo ip link add client1-veth type veth peer name client1-br  
sudo ip link add client2-veth type veth peer name client2-br

# Move one end of each veth pair to respective namespace
sudo ip link set server-veth netns server-ns
sudo ip link set client1-veth netns client1-ns
sudo ip link set client2-veth netns client2-ns

# Attach bridge ends to bridge
sudo ip link set server-br master br0
sudo ip link set client1-br master br0
sudo ip link set client2-br master br0

# Bring up bridge interfaces
sudo ip link set server-br up
sudo ip link set client1-br up  
sudo ip link set client2-br up

# Configure SERVER namespace
sudo ip netns exec server-ns ip link set lo up
sudo ip netns exec server-ns ip addr add 192.168.0.1/24 dev server-veth
sudo ip netns exec server-ns ip link set server-veth up multicast on
sudo ip netns exec server-ns ip route add 224.0.0.0/4 dev server-veth  # Route multicast ajoutée

# Configure CLIENT1 namespace  
sudo ip netns exec client1-ns ip link set lo up
sudo ip netns exec client1-ns ip addr add 192.168.0.2/24 dev client1-veth
sudo ip netns exec client1-ns ip link set client1-veth up multicast on
sudo ip netns exec client1-ns ip route add 224.0.0.0/4 dev client1-veth  # Route multicast ajoutée

# Configure CLIENT2 namespace
sudo ip netns exec client2-ns ip link set lo up
sudo ip netns exec client2-ns ip addr add 192.168.0.3/24 dev client2-veth
sudo ip netns exec client2-ns ip link set client2-veth up multicast on
sudo ip netns exec client2-ns ip route add 224.0.0.0/4 dev client2-veth  # Route multicast ajoutée

# Enable multicast on bridge (important for SOME/IP Service Discovery)
echo 0 | sudo tee /sys/class/net/br0/bridge/multicast_snooping  # Désactivé pour flooding
echo 1 | sudo tee /sys/class/net/br0/bridge/multicast_querier   # Querier activé
echo 2 | sudo tee /sys/class/net/br0/bridge/multicast_router    # Forwarding forcé
sudo sysctl -w net.ipv4.ip_forward=1

# Enable proxy_ARP on bridge to fix potential ARP issues in namespaces
echo 1 | sudo tee /proc/sys/net/ipv4/conf/br0/proxy_arp
