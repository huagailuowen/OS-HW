#ifndef _MY_RUNTIME_INFO_PROTOCOL_H_
#define _MY_RUNTIME_INFO_PROTOCOL_H_

#define MY_RUNTIME_INFO_PROTOCOL_GUID \
  { 0x12345678, 0x9abc, 0xdef0, {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd, 0xef, 0x00 } }

typedef struct _MY_RUNTIME_INFO_PROTOCOL MY_RUNTIME_INFO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *MY_GET_INFO)(
  OUT UINT8 *Buffer,
  IN OUT UINTN *BufferSize
);

struct _MY_RUNTIME_INFO_PROTOCOL {
  MY_GET_INFO GetInfo;
};

extern EFI_GUID gMyRuntimeInfoProtocolGuid;

#endif