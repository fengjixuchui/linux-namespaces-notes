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