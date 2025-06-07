## Test
```
export CUDA_VISIBLE_DEVICES=0,1,2,3,4,5,6
nvcc -o nccl_test nccl_wrapper.c nccl_test.c -lnccl -lcudart -lcuda
./nccl_test -b 8 -n 1000 -f 2 -g 7
```