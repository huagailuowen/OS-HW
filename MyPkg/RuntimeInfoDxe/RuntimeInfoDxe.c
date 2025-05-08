#include <Uefi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>      // 添加运行时库
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/GlobalVariable.h>        // 添加全局变量GUID
#include <Protocol/MyRuntimeInfo.h>

extern EFI_GUID gMyRuntimeInfoProtocolGuid;
extern EFI_GUID gEfiGlobalVariableGuid;

// 添加这个辅助函数用于调试
VOID
PrintGuid(
  IN EFI_GUID *Guid
  )
{
  DEBUG ((DEBUG_INFO, 
         "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
         Guid->Data1, Guid->Data2, Guid->Data3,
         Guid->Data4[0], Guid->Data4[1], Guid->Data4[2], Guid->Data4[3],
         Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7]));
}

STATIC EFI_STATUS EFIAPI
MyGetInfoImpl(UINT8 *Buffer, UINTN *BufferSize)
{
  CONST CHAR8 *Msg = "Hardware Info from Runtime Protocol";
  UINTN Len = AsciiStrLen(Msg) + 1;

  if (*BufferSize < Len) {
    *BufferSize = Len;
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem(Buffer, Msg, Len);
  *BufferSize = Len;
  return EFI_SUCCESS;
}

STATIC MY_RUNTIME_INFO_PROTOCOL mMyProtocol = {
  .GetInfo = MyGetInfoImpl
};

#define UNUSED(x) (void)(x)
EFI_STATUS
EFIAPI
RuntimeInfoEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  CHAR8 *Info;
  CHAR8 *RuntimeInfo;
  UINTN Size;
  
  DEBUG ((DEBUG_INFO, "RuntimeInfoDxe: Starting initialization\n"));
  
  // 使用运行时内存分配字符串，这样在运行时也可以访问
  Info = AllocateRuntimePool(AsciiStrSize("Hardware Info from Runtime Protocol"));
  if (Info == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate runtime memory for Info\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  
  AsciiStrCpyS(Info, AsciiStrSize("Hardware Info from Runtime Protocol"),
               "Hardware Info from Runtime Protocol");
  Size = AsciiStrSize(Info);
  
  // 调试打印GUID值
  DEBUG ((DEBUG_INFO, "My Runtime Info Protocol GUID: "));
  
  PrintGuid(&gMyRuntimeInfoProtocolGuid);
  
  // 安装协议
  Status = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gMyRuntimeInfoProtocolGuid,
    &mMyProtocol,
    NULL
  );
  
  if (EFI_ERROR(Status)) {
    FreePool(Info);
    DEBUG ((DEBUG_ERROR, "Failed to install protocol: %r\n", Status));
    return Status;
  }
  
  // 创建一个使用全局变量GUID的测试变量
  Status = gRT->SetVariable(
    L"MyTestInfo",
    &gEfiGlobalVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    Size,
    Info
  );
  
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to create test variable: %r\n", Status));
    // 继续执行，不返回错误
  } else {
    DEBUG ((DEBUG_INFO, "Successfully created test variable MyTestInfo\n"));
  }
  
  // 创建自定义GUID变量
  Status = gRT->SetVariable(
    L"MyRuntimeInfo",
    &gMyRuntimeInfoProtocolGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    Size,
    Info
  );
  
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to create MyRuntimeInfo variable: %r (GUID: ", Status));
    PrintGuid(&gMyRuntimeInfoProtocolGuid);
    DEBUG ((DEBUG_ERROR, ")\n"));
    // 继续执行，不返回错误
  } else {
    DEBUG ((DEBUG_INFO, "Successfully created MyRuntimeInfo variable\n"));
  }
  
  return EFI_SUCCESS;
}