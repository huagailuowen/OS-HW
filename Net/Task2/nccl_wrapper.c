#include "nccl_wrapper.h"

ncclResult_t nccl_broadcast_data(void* data, size_t count, int root, ncclComm_t comm) {
    // 1. 创建CUDA流
    cudaStream_t stream;
    cudaError_t cuda_err = cudaStreamCreate(&stream);
    if (cuda_err != cudaSuccess) {
        return ncclSystemError;
    }
    
    // 2. 在GPU上分配内存
    void* device_data;
    cuda_err = cudaMalloc(&device_data, count * sizeof(float)); // 假设数据是float类型
    if (cuda_err != cudaSuccess) {
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 3. 将主机数据拷贝到GPU
    cuda_err = cudaMemcpyAsync(device_data, data, count * sizeof(float), 
                              cudaMemcpyHostToDevice, stream);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_data);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 4. 执行NCCL广播操作
    ncclResult_t nccl_result = ncclBroadcast(
        device_data,    // 发送缓冲区
        device_data,    // 接收缓冲区(广播时源和目的可以是同一缓冲区)
        count,          // 元素数量
        ncclFloat,      // 数据类型
        root,           // 根节点ID
        comm,           // NCCL通信器
        stream          // CUDA流
    );
    
    if (nccl_result != ncclSuccess) {
        cudaFree(device_data);
        cudaStreamDestroy(stream);
        return nccl_result;
    }
    
    // 5. 将结果拷贝回主机内存
    cuda_err = cudaMemcpyAsync(data, device_data, count * sizeof(float),
                              cudaMemcpyDeviceToHost, stream);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_data);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 6. 同步CUDA流
    cuda_err = cudaStreamSynchronize(stream);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_data);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 7. 清理资源
    cudaFree(device_data);
    cudaStreamDestroy(stream);
    
    return ncclSuccess;
}

ncclResult_t nccl_allreduce_data(const void* sendbuff, void* recvbuff, 
                                size_t count, ncclDataType_t datatype, 
                                ncclRedOp_t op, ncclComm_t comm) {
    // 创建CUDA流
    cudaStream_t stream;
    cudaError_t cuda_err = cudaStreamCreate(&stream);
    if (cuda_err != cudaSuccess) {
        return ncclSystemError;
    }
    
    // 在GPU上分配发送和接收内存
    void* device_sendbuff;
    void* device_recvbuff;
    size_t typeSize;
    
    // 根据数据类型确定大小
    switch(datatype) {
        case ncclInt8:
        case ncclUint8:
            typeSize = 1;
            break;
        case ncclInt32:
        case ncclUint32:
        case ncclFloat32:
            typeSize = 4;
            break;
        case ncclInt64:
        case ncclUint64:
        case ncclFloat64:
            typeSize = 8;
            break;
        default:
            cudaStreamDestroy(stream);
            return ncclInvalidArgument;
    }
    
    // 分配设备内存
    cuda_err = cudaMalloc(&device_sendbuff, count * typeSize);
    if (cuda_err != cudaSuccess) {
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    cuda_err = cudaMalloc(&device_recvbuff, count * typeSize);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_sendbuff);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 将发送数据拷贝到GPU
    cuda_err = cudaMemcpyAsync(device_sendbuff, sendbuff, count * typeSize,
                              cudaMemcpyHostToDevice, stream);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_sendbuff);
        cudaFree(device_recvbuff);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 执行All-Reduce操作
    ncclResult_t nccl_result = ncclAllReduce(
        device_sendbuff,  // 发送缓冲区
        device_recvbuff,  // 接收缓冲区
        count,            // 元素数量
        datatype,         // 数据类型
        op,               // 规约操作类型
        comm,             // NCCL通信器
        stream            // CUDA流
    );
    
    if (nccl_result != ncclSuccess) {
        cudaFree(device_sendbuff);
        cudaFree(device_recvbuff);
        cudaStreamDestroy(stream);
        return nccl_result;
    }
    
    // 将结果拷贝回主机内存
    cuda_err = cudaMemcpyAsync(recvbuff, device_recvbuff, count * typeSize,
                              cudaMemcpyDeviceToHost, stream);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_sendbuff);
        cudaFree(device_recvbuff);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 同步CUDA流
    cuda_err = cudaStreamSynchronize(stream);
    if (cuda_err != cudaSuccess) {
        cudaFree(device_sendbuff);
        cudaFree(device_recvbuff);
        cudaStreamDestroy(stream);
        return ncclSystemError;
    }
    
    // 清理资源
    cudaFree(device_sendbuff);
    cudaFree(device_recvbuff);
    cudaStreamDestroy(stream);
    
    return ncclSuccess;
}