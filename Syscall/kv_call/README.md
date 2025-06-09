## Test
```
gcc -o test1_serial test1_serial.c -static -lpthread -ldl
# ---------------------------------------------------------

cd testfiles/kv_call
sysbench --threads=100 --events=1000000 test2.lua run
# ---------------------------------------------------------
make -f Makefile.test3 kbuild
# 进入测试目录
cd /testfiles/kv_call

# 加载内核模块
insmod testmodule.ko

# 查看内核日志
dmesg | tail


# 卸载模块
rmmod testmodule.ko
```