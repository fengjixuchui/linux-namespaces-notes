# linux-namespaces-notes

## Process namespaces

### unshare command

`--fork` - Fork current shell to new namespace  
`--pid` - Create new process namespace  
`--mount-proc` - ensure that /proc filesystem is mounted in new namespace  

``` bash
sudo unshare --fork --pid --mount-proc bash
```

``` bash
ps -aux
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         1  0.2  0.3   8032  3104 pts/0    S    08:59   0:00 bash
root         4  0.0  0.2  10860  2636 pts/0    R+   08:59   0:00 ps -aux
```

## Network namespace
Same command as in process namespace, but with the `--net` flag:  
``` bash
sudo unshare --fork --pid --mount-proc --net bash
```

ifconfig output will be empty.

### Communication between two isolated processes
[tutorial: setting-up-linux-network-namespace-and-bridge-for-network-isolation](https://medium.com/@masud.educations/setting-up-linux-network-namespace-and-bridge-for-network-isolation-9a9bba6e75de)   

#### step 0 - create two bash processes in new network namespaces
``` bash
sudo unshare --fork --pid --mount-proc --net bash
```

#### step 1 - create network namespaces
``` bash
sudo ip netns add ns1
sudo ip netns add ns2
```

You can check the created network name spaces
``` bash
sudo ip netns list
ns2 (id: 1)
ns1 (id: 0)
```

#### step 2 - create virtual ethernet pairs
``` bash
sudo ip link add veth1 type veth peer name vpeer1
sudo ip link add veth2 type veth peer name vpeer2
```

ifconfig output for the created devices:  
``` bash
veth1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet6 fe80::bbfb:9b6f:a37a:19b7  prefixlen 64  scopeid 0x20<link>
        ether 32:de:35:9c:30:19  txqueuelen 1000  (Ethernet)
        RX packets 13  bytes 1989 (1.9 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 12  bytes 1882 (1.8 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

veth2: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet6 fe80::b741:1593:de03:3620  prefixlen 64  scopeid 0x20<link>
        ether 4a:20:db:99:e9:c8  txqueuelen 1000  (Ethernet)
        RX packets 8  bytes 1183 (1.1 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 8  bytes 1183 (1.1 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet6 fe80::581f:6921:28c4:ca9  prefixlen 64  scopeid 0x20<link>
        ether ea:39:9b:75:de:e0  txqueuelen 1000  (Ethernet)
        RX packets 12  bytes 1882 (1.8 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 13  bytes 1989 (1.9 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer2: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet6 fe80::ed6:7e7b:2e9:9573  prefixlen 64  scopeid 0x20<link>
        ether d6:e5:ac:1d:c1:c5  txqueuelen 1000  (Ethernet)
        RX packets 8  bytes 1183 (1.1 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 8  bytes 1183 (1.1 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

```

#### step 3 - Bring up network interfaces
``` bash
sudo ip link set veth1 up
sudo ip link set veth2 up
sudo ip link set vpeer1 up
sudo ip link set vpeer2 up
```

ifconfig output for the created devices:  
```bash
veth1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 169.254.238.203  netmask 255.255.0.0  broadcast 169.254.255.255
        inet6 fe80::bbfb:9b6f:a37a:19b7  prefixlen 64  scopeid 0x20<link>
        ether 32:de:35:9c:30:19  txqueuelen 1000  (Ethernet)
        RX packets 41  bytes 6052 (5.9 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 35  bytes 5454 (5.3 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

veth2: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 169.254.16.171  netmask 255.255.0.0  broadcast 169.254.255.255
        inet6 fe80::b741:1593:de03:3620  prefixlen 64  scopeid 0x20<link>
        ether 4a:20:db:99:e9:c8  txqueuelen 1000  (Ethernet)
        RX packets 40  bytes 5983 (5.8 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 35  bytes 5448 (5.3 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 169.254.52.43  netmask 255.255.0.0  broadcast 169.254.255.255
        inet6 fe80::581f:6921:28c4:ca9  prefixlen 64  scopeid 0x20<link>
        ether ea:39:9b:75:de:e0  txqueuelen 1000  (Ethernet)
        RX packets 35  bytes 5454 (5.3 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 41  bytes 6052 (5.9 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer2: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 169.254.199.49  netmask 255.255.0.0  broadcast 169.254.255.255
        inet6 fe80::ed6:7e7b:2e9:9573  prefixlen 64  scopeid 0x20<link>
        ether d6:e5:ac:1d:c1:c5  txqueuelen 1000  (Ethernet)
        RX packets 35  bytes 5448 (5.3 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 40  bytes 5983 (5.8 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

#### step 4 - move the virtual ethernet devices into the namespaces
``` bash
sudo ip link set veth1 netns ns1
sudo ip link set veth2 netns ns2
```

veth1 and veth2 are not seen on the ifconfig anymore:  
``` bash
vpeer1: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        ether ea:39:9b:75:de:e0  txqueuelen 1000  (Ethernet)
        RX packets 36  bytes 5541 (5.4 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 42  bytes 6139 (5.9 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer2: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        ether d6:e5:ac:1d:c1:c5  txqueuelen 1000  (Ethernet)
        RX packets 36  bytes 5535 (5.4 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 41  bytes 6070 (5.9 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

```

#### step 5 - add the processes to the network namespaces
`sudo nsenter --net=/var/run/netns/<target_namespace_name> --target <process_pid>`

Run the following commands in the container process.  
Note that target pid is 1 since in the example every container has only one process with pid 1.  
``` bash
sudo nsenter --net=/var/run/netns/ns1 --target 1
sudo nsenter --net=/var/run/netns/ns2 --target 1
```

``` bash
root@raspberrypi:/home/pi# ifconfig -a
lo: flags=8<LOOPBACK>  mtu 65536
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

veth2: flags=4098<BROADCAST,MULTICAST>  mtu 1500
        ether 4a:20:db:99:e9:c8  txqueuelen 1000  (Ethernet)
        RX packets 41  bytes 6070 (5.9 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 36  bytes 5535 (5.4 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

#### step 6 - Set Up Interfaces within Namespaces
In the containers, set up the interfaces.  
``` bash
sudo ip netns exec ns1 ip link set veth1 up
sudo ip netns exec ns2 ip link set veth2 up
```

#### step 7 - assign ip addresses to the network devices
In the containers, assign IP address to the network devices.  
``` bash
sudo ip netns exec ns1 ip addr add 10.10.0.10/16 dev veth1
sudo ip netns exec ns2 ip addr add 10.10.0.20/16 dev veth2
```

``` bash
root@raspberrypi:/home/pi# ifconfig
veth1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.10.0.10  netmask 255.255.0.0  broadcast 0.0.0.0
        inet6 fe80::30de:35ff:fe9c:3019  prefixlen 64  scopeid 0x20<link>
        ether 32:de:35:9c:30:19  txqueuelen 1000  (Ethernet)
        RX packets 92  bytes 14501 (14.1 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 48  bytes 6477 (6.3 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

```

#### step 8 - create the bridge between the two containers
In the host running the containers create the bridge and assign it IP address.  
``` bash
sudo ip link add br1 type bridge
sudo ip link set br1 up
sudo ip link set vpeer1 master br1
sudo ip link set vpeer2 master br1
sudo ip addr add 10.10.0.1/16 dev br1
```

``` bash
pi@raspberrypi: ifconfig
br1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.10.0.1  netmask 255.255.0.0  broadcast 0.0.0.0
        inet6 fe80::748b:86ff:fe5c:8b2f  prefixlen 64  scopeid 0x20<link>
        ether d6:e5:ac:1d:c1:c5  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 30  bytes 4089 (3.9 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 169.254.52.43  netmask 255.255.0.0  broadcast 169.254.255.255
        inet6 fe80::581f:6921:28c4:ca9  prefixlen 64  scopeid 0x20<link>
        ether ea:39:9b:75:de:e0  txqueuelen 1000  (Ethernet)
        RX packets 49  bytes 6547 (6.3 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 115  bytes 18615 (18.1 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

vpeer2: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 169.254.199.49  netmask 255.255.0.0  broadcast 169.254.255.255
        inet6 fe80::ed6:7e7b:2e9:9573  prefixlen 64  scopeid 0x20<link>
        ether d6:e5:ac:1d:c1:c5  txqueuelen 1000  (Ethernet)
        RX packets 48  bytes 6471 (6.3 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 100  bytes 15501 (15.1 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

#### step 9 - verify connectivity with ping

