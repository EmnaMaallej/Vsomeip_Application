# speed_montoring
## Linux Build & Run Instructions

### 1. Create network namespaces
```bash
cd <root directory of Speed_Monitoring>
chmod +x ns
./ns
```
2. Configure routes and IP addresses
sudo ip route add 224.0.0.1 dev br0

sudo ip addr add 192.168.0.1/24 dev server-br
sudo ip addr add 192.168.0.2/24 dev client1-br
sudo ip addr add 192.168.0.3/24 dev client2-br

