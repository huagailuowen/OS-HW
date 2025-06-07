## Test
```

nvcc -o nccl_test nccl_wrapper.c nccl_test.c -lnccl -lcudart -lcuda
./nccl_test 
```
## Results
```
Testing nccl_broadcast_data function...
广播测试成功!
Testing nccl_allreduce_data function...
AllReduce测试成功!
所有测试完成!
```