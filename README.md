# speed_montoring
## Linux Build & Run Instructions

### Create network namespaces
```bash
cd <root directory of Speed_Monitoring>
chmod +x ns
./ns
```
## Configure routes and IP addresses
```bash
sudo ip route add 224.0.0.1 dev br0

sudo ip addr add 192.168.0.1/24 dev server-br
sudo ip addr add 192.168.0.2/24 dev client1-br
sudo ip addr add 192.168.0.3/24 dev client2-br
```

## Build whole project:
```bash
cd <root directory of Speed_Monitoring>/src
mkdir build
cd build
cmake ..
make
```


## Run server target (Terminal 1):
```bash
sudo ip netns exec server_ns bash
cd <root directory of Speed_Monitoring>/src/build
mount -t tmpfs tmpfs /tmp
VSOMEIP_CONFIGURATION=../../Config/server.json VSOMEIP_APPLICATION_NAME=server ./server

```
## Run client2 target (Terminal 2):
```bash
sudo ip netns exec client2_ns bash
cd <root directory of Speed_Monitoring>/src/build
mount -t tmpfs tmpfs /tmp
VSOMEIP_CONFIGURATION=../../Config/client2.json VSOMEIP_APPLICATION_NAME=client2 ./client2
```

## Run client1 target (Terminal 3):
```bash
sudo ip netns exec client1_ns bash
cd <root directory of Speed_Monitoring>/src/build
mount -t tmpfs tmpfs /tmp
VSOMEIP_CONFIGURATION=../../Config/client1.json VSOMEIP_APPLICATION_NAME=client1 ./client1
```
