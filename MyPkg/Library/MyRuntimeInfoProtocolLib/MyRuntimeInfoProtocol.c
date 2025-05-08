/** @file
  Runtime Information Protocol Definition

  Copyright (c) 2025, Your Name. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef RUNTIME_INFO_PROTOCOL_H_
#define RUNTIME_INFO_PROTOCOL_H_

#define MY_RUNTIME_INFO_PROTOCOL_GUID \
  { \
    0x12345678, 0xabcd, 0xef01, { 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01 } \
  }

typedef struct _MY_RUNTIME_INFO_PROTOCOL MY_RUNTIME_INFO_PROTOCOL;

/**
  运行时信息协议结构体
**/
struct _MY_RUNTIME_INFO_PROTOCOL {
  // 根据您的需求添加协议成员
  UINT32 Version;
  // 其他成员...
};

extern EFI_GUID gMyRuntimeInfoProtocolGuid;

#endif // RUNTIME_INFO_PROTOCOL_H_