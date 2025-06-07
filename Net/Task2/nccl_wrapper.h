#ifndef NCCL_WRAPPER_H
#define NCCL_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <nccl.h>

/**
* @brief 使用NCCL执行多GPU数据广播操作
* @param data 需要广播的数据缓冲区指针
* @param count 数据元素的个数
* @param root 广播发送方的GPU ID
* @param comm NCCL通信器
* @return ncclSuccess表示成功，其他错误码请参照NCCL官方文档
*/
ncclResult_t nccl_broadcast_data(void* data, size_t count, int root, ncclComm_t comm);

/**
 * @brief 使用NCCL执行All-Reduce操作
 * @param sendbuff 发送缓冲区指针
 * @param recvbuff 接收缓冲区指针
 * @param count 数据元素的个数
 * @param datatype 数据类型
 * @param op Reduce操作类型（如求和）
 * @param comm NCCL通信器
 * @return ncclSuccess表示成功，其他错误码请参照NCCL官方文档
 */
ncclResult_t nccl_allreduce_data(const void* sendbuff, void* recvbuff, 
                                size_t count, ncclDataType_t datatype, 
                                ncclRedOp_t op, ncclComm_t comm);

#endif /* NCCL_WRAPPER_H */