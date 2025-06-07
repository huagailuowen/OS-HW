#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "nccl_wrapper.h"

// 计时辅助函数
double getTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1.0e-9;
}

// TCP/IP广播实现(简化版本，实际应使用MPI或自定义socket实现)
void tcp_broadcast_test(float* data, int size, int root) {
    double startTime = getTime();
    
    // 这里应该是实际的TCP/IP广播实现
    // 此处仅为模拟延迟
    usleep(100000);  // 模拟100ms网络延迟
    
    double endTime = getTime();
    double elapsedMs = (endTime - startTime) * 1000.0;
    double bandwidthGB = (size * sizeof(float)) / (elapsedMs * 1.0e6);
    
    printf("\nTCP/IP广播性能(模拟):\n");
    printf("  数据大小: %d MB\n", size * sizeof(float) / (1024 * 1024));
    printf("  时间: %.2f ms\n", elapsedMs);
    printf("  带宽: %.2f GB/s\n", bandwidthGB);
}

// 错误检查宏
#define CUDA_CHECK(call) do {                            \
    cudaError_t err = call;                              \
    if (err != cudaSuccess) {                            \
        printf("CUDA error at %s:%d: %s\n",              \
               __FILE__, __LINE__,                       \
               cudaGetErrorString(err));                 \
        exit(1);                                         \
    }                                                    \
} while(0)

#define NCCL_CHECK(call) do {                            \
    ncclResult_t res = call;                             \
    if (res != ncclSuccess) {                            \
        printf("NCCL error at %s:%d: %s\n",              \
               __FILE__, __LINE__,                       \
               ncclGetErrorString(res));                 \
        exit(1);                                         \
    }                                                    \
} while(0)

int main() {
    // 初始化变量
    int nDev = 4;  // 假设有4个GPU
    int size = 32 * 1024 * 1024;  // 32MB数据大小
    int root = 0;  // 广播源GPU
    
    // 获取可用GPU数量
    int deviceCount;
    CUDA_CHECK(cudaGetDeviceCount(&deviceCount));
    if (nDev > deviceCount) {
        nDev = deviceCount;
        printf("Warning: 只有 %d 个GPU可用, 将使用全部可用GPU\n", nDev);
    }
    
    // 测试Broadcast
    printf("\n-----测试广播操作-----\n");
    
    // 分配主机内存
    float* hostData = (float*)malloc(size * sizeof(float));
    if (!hostData) {
        printf("Error: 主机内存分配失败\n");
        return 1;
    }
    
    // 初始化数据
    for (int i = 0; i < size; i++) {
        hostData[i] = i;
    }
    
    // 初始化NCCL
    ncclComm_t comms[nDev];
    int devs[nDev];
    
    for (int i = 0; i < nDev; i++) {
        devs[i] = i;
    }
    
    NCCL_CHECK(ncclCommInitAll(comms, nDev, devs));
    
    printf("启动NCCL广播测试，使用 %d 个GPU设备\n", nDev);
    
    // 执行广播
    double startTime = getTime();
    NCCL_CHECK(nccl_broadcast_data(hostData, size, root, comms[0]));
    double endTime = getTime();
    
    // 计算带宽
    double elapsedMs = (endTime - startTime) * 1000.0;
    double bandwidthGB = (size * sizeof(float)) / (elapsedMs * 1.0e6);
    
    printf("NCCL广播性能:\n");
    printf("  数据大小: %d MB\n", size * sizeof(float) / (1024 * 1024));
    printf("  时间: %.2f ms\n", elapsedMs);
    printf("  带宽: %.2f GB/s\n", bandwidthGB);
    
    // 对比以太网TCP/IP性能
    tcp_broadcast_test(hostData, size, root);
    
    // 测试AllReduce
    printf("\n-----测试AllReduce操作-----\n");
    
    // 准备AllReduce测试数据
    float* allreduceInput = (float*)malloc(size * sizeof(float));
    float* allreduceOutput = (float*)malloc(size * sizeof(float));
    
    for (int i = 0; i < size; i++) {
        allreduceInput[i] = i % 100;  // 简单的测试数据
    }
    
    printf("启动NCCL AllReduce测试，使用 %d 个GPU设备\n", nDev);
    
    // 执行AllReduce
    startTime = getTime();
    NCCL_CHECK(nccl_allreduce_data(
        allreduceInput, 
        allreduceOutput, 
        size, 
        ncclFloat, 
        ncclSum, 
        comms[0]
    ));
    endTime = getTime();
    
    // 计算带宽
    elapsedMs = (endTime - startTime) * 1000.0;
    bandwidthGB = (2 * size * sizeof(float)) / (elapsedMs * 1.0e6); // AllReduce带宽计算需要乘以2
    
    printf("NCCL AllReduce性能:\n");
    printf("  数据大小: %d MB\n", size * sizeof(float) / (1024 * 1024));
    printf("  时间: %.2f ms\n", elapsedMs);
    printf("  带宽: %.2f GB/s\n", bandwidthGB);
    
    // 清理资源
    for (int i = 0; i < nDev; i++) {
        ncclCommDestroy(comms[i]);
    }
    free(hostData);
    free(allreduceInput);
    free(allreduceOutput);
    
    printf("\n测试完成\n");
    
    return 0;
}