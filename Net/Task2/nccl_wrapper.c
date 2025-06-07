#include "nccl_wrapper.h"

ncclResult_t nccl_broadcast_data(void* data, size_t count, int root, ncclComm_t comm) {
    // 假设我们处理的是float类型数据，实际应用中应该将数据类型作为参数传入
    ncclDataType_t dataType = ncclFloat;
    
    // 创建CUDA流
    cudaStream_t stream;
    cudaError_t cudaErr = cudaStreamCreate(&stream);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "CUDA stream creation failed: %s\n", cudaGetErrorString(cudaErr));
        return ncclUnhandledCudaError;
    }
    
    // 执行广播操作，使用相同的指针作为输入和输出
    ncclResult_t result = ncclBroadcast(data, data, count, dataType, root, comm, stream);
    
    // 同步CUDA流以确保操作完成
    cudaErr = cudaStreamSynchronize(stream);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "CUDA stream synchronize failed: %s\n", cudaGetErrorString(cudaErr));
        cudaStreamDestroy(stream);
        return ncclUnhandledCudaError;
    }
    
    // 销毁流
    cudaErr = cudaStreamDestroy(stream);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "CUDA stream destroy failed: %s\n", cudaGetErrorString(cudaErr));
        return ncclUnhandledCudaError;
    }
    
    return result;
}

ncclResult_t nccl_allreduce_data(const void* sendbuff, void* recvbuff, 
                               size_t count, ncclDataType_t datatype, 
                               ncclRedOp_t op, ncclComm_t comm) {
    // 创建CUDA流
    cudaStream_t stream;
    cudaError_t cudaErr = cudaStreamCreate(&stream);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "CUDA stream creation failed: %s\n", cudaGetErrorString(cudaErr));
        return ncclUnhandledCudaError;
    }
    
    // 执行all-reduce操作
    ncclResult_t result = ncclAllReduce(sendbuff, recvbuff, count, datatype, op, comm, stream);
    
    // 同步CUDA流以确保操作完成
    cudaErr = cudaStreamSynchronize(stream);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "CUDA stream synchronize failed: %s\n", cudaGetErrorString(cudaErr));
        cudaStreamDestroy(stream);
        return ncclUnhandledCudaError;
    }
    
    // 销毁流
    cudaErr = cudaStreamDestroy(stream);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "CUDA stream destroy failed: %s\n", cudaGetErrorString(cudaErr));
        return ncclUnhandledCudaError;
    }
    
    return result;
}