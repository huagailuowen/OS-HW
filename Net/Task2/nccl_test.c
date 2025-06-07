#include <stdlib.h>
#include <stdio.h>
#include "cuda_runtime.h"
#include "nccl.h"
#include "nccl_wrapper.h"

#define CUDACHECK(cmd) do {                         \
  cudaError_t err = cmd;                            \
  if (err != cudaSuccess) {                         \
    printf("Failed: Cuda error %s:%d '%s'\n",       \
        __FILE__,__LINE__,cudaGetErrorString(err)); \
    exit(EXIT_FAILURE);                             \
  }                                                 \
} while(0)

#define NCCLCHECK(cmd) do {                         \
  ncclResult_t res = cmd;                           \
  if (res != ncclSuccess) {                         \
    printf("Failed, NCCL error %s:%d '%s'\n",       \
        __FILE__,__LINE__,ncclGetErrorString(res)); \
    exit(EXIT_FAILURE);                             \
  }                                                 \
} while(0)

void test_broadcast() {
    printf("Testing nccl_broadcast_data function...\n");
    
    // 管理两个设备
    int nDev = 2;
    int size = 32 * 1024;
    int devs[2] = { 0, 1 };
    ncclComm_t comms[2];
    
    // 分配和初始化设备缓冲区
    float** data = (float**)malloc(nDev * sizeof(float*));
    cudaStream_t* s = (cudaStream_t*)malloc(sizeof(cudaStream_t) * nDev);
    
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        CUDACHECK(cudaMalloc((void**)data + i, size * sizeof(float)));
        CUDACHECK(cudaStreamCreate(s + i));
        
        // 设备0的数据设为1，设备1的数据设为0
        float value = (i == 0) ? 1.0f : 0.0f;
        CUDACHECK(cudaMemset(data[i], 0, size * sizeof(float)));
        
        // 使用主机数据初始化
        float* h_data = (float*)malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            h_data[j] = value;
        }
        CUDACHECK(cudaMemcpy(data[i], h_data, size * sizeof(float), cudaMemcpyHostToDevice));
        free(h_data);
    }
    
    // 初始化NCCL
    NCCLCHECK(ncclCommInitAll(comms, nDev, devs));
    
    // 使用NCCL Group API进行广播
    NCCLCHECK(ncclGroupStart());
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        NCCLCHECK(ncclBroadcast(data[i], data[i], size, ncclFloat, 0, comms[i], s[i]));
    }
    NCCLCHECK(ncclGroupEnd());
    
    // 同步所有设备
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        CUDACHECK(cudaStreamSynchronize(s[i]));
    }
    
    // 验证结果
    float* host_data = (float*)malloc(size * sizeof(float));
    CUDACHECK(cudaSetDevice(1));
    CUDACHECK(cudaMemcpy(host_data, data[1], size * sizeof(float), cudaMemcpyDeviceToHost));
    
    // 检查设备1的数据是否被正确广播 (应该全部为1.0)
    int success = 1;
    for (int i = 0; i < 10; ++i) {
        if (host_data[i] != 1.0f) {
            printf("验证失败：index %d, 预期 1.0，实际 %f\n", i, host_data[i]);
            success = 0;
            break;
        }
    }
    
    if (success) {
        printf("广播测试成功!\n");
    }
    
    // 清理资源
    free(host_data);
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        CUDACHECK(cudaFree(data[i]));
        CUDACHECK(cudaStreamDestroy(s[i]));
        ncclCommDestroy(comms[i]);
    }
    
    free(data);
    free(s);
}

void test_allreduce() {
    printf("Testing nccl_allreduce_data function...\n");
    
    // 管理两个设备
    int nDev = 2;
    int size = 32 * 1024;
    int devs[2] = { 0, 1 };
    ncclComm_t comms[2];
    
    // 分配和初始化设备缓冲区
    float** sendbuff = (float**)malloc(nDev * sizeof(float*));
    float** recvbuff = (float**)malloc(nDev * sizeof(float*));
    cudaStream_t* s = (cudaStream_t*)malloc(sizeof(cudaStream_t) * nDev);
    
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        CUDACHECK(cudaMalloc((void**)sendbuff + i, size * sizeof(float)));
        CUDACHECK(cudaMalloc((void**)recvbuff + i, size * sizeof(float)));
        CUDACHECK(cudaStreamCreate(s + i));
        
        // 设置发送缓冲区为1.0，接收缓冲区为0.0
        float* h_data = (float*)malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            h_data[j] = 1.0f;
        }
        
        CUDACHECK(cudaMemcpy(sendbuff[i], h_data, size * sizeof(float), cudaMemcpyHostToDevice));
        CUDACHECK(cudaMemset(recvbuff[i], 0, size * sizeof(float)));
        
        free(h_data);
    }
    
    // 初始化NCCL
    NCCLCHECK(ncclCommInitAll(comms, nDev, devs));
    
    // 执行AllReduce操作
    NCCLCHECK(ncclGroupStart());
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        NCCLCHECK(ncclAllReduce(sendbuff[i], recvbuff[i], size, ncclFloat, ncclSum, comms[i], s[i]));
    }
    NCCLCHECK(ncclGroupEnd());
    
    // 同步所有设备
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        CUDACHECK(cudaStreamSynchronize(s[i]));
    }
    
    // 验证结果
    float* host_data = (float*)malloc(size * sizeof(float));
    CUDACHECK(cudaSetDevice(0));
    CUDACHECK(cudaMemcpy(host_data, recvbuff[0], size * sizeof(float), cudaMemcpyDeviceToHost));
    
    // 检查接收缓冲区是否包含正确的结果 (1.0 + 1.0 = 2.0)
    int success = 1;
    for (int i = 0; i < 10; ++i) {
        if (host_data[i] != 2.0f) {
            printf("验证失败：index %d, 预期 2.0，实际 %f\n", i, host_data[i]);
            success = 0;
            break;
        }
    }
    
    if (success) {
        printf("AllReduce测试成功!\n");
    }
    
    // 清理资源
    free(host_data);
    for (int i = 0; i < nDev; ++i) {
        CUDACHECK(cudaSetDevice(i));
        CUDACHECK(cudaFree(sendbuff[i]));
        CUDACHECK(cudaFree(recvbuff[i]));
        CUDACHECK(cudaStreamDestroy(s[i]));
        ncclCommDestroy(comms[i]);
    }
    
    free(sendbuff);
    free(recvbuff);
    free(s);
}

int main(int argc, char* argv[]) {
    int deviceCount = 0;
    CUDACHECK(cudaGetDeviceCount(&deviceCount));
    
    if (deviceCount < 2) {
        printf("需要至少2个GPU设备来运行测试程序，当前有%d个设备\n", deviceCount);
        return 0;
    }
    
    // 测试广播
    test_broadcast();
    
    // 测试AllReduce
    test_allreduce();
    
    printf("所有测试完成!\n");
    return 0;
}