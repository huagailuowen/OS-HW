#include <Uefi.h>
#include "ChangeACPITables.c"
#include "PrintAllACPITables.c"
EFI_STATUS 
EFIAPI
UefiMain(
	IN EFI_HANDLE Imagehandle, 
	IN EFI_SYSTEM_TABLE *SystemTable
) {
	
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"HelloWorld\n");
	PrintAllACPITables(Imagehandle, SystemTable);
	ChangeACPITable(1414678338, NULL);
	return EFI_SUCCESS;
}
