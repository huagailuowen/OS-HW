cmd_/home/luowen/syshw/Syscall/kv_call/modules.order := {   echo /home/luowen/syshw/Syscall/kv_call/testmodule.ko; :; } | awk '!x[$$0]++' - > /home/luowen/syshw/Syscall/kv_call/modules.order
