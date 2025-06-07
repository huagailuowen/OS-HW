## Task1

### Description
filter packets based on the minimum and maximum size of the packet.

### Usage
```bash
sudo ./tcpdump -i any "host www.baidu.com" -g 10-1000

tcpdump -g min-max
```

### Bugs

- dynamic link can not be used in the qemu environment
- do not have reasonable method to test the code

## Task2

### Test
```bash
nc -l -p 12345 &
echo "hello" | nc 127.0.0.1 12345&



gcc server.c -o server
gcc client.c -o client

./server &
./client

cat /proc/socket_fairness



```
