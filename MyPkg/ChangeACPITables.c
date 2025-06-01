#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>



/**
  应用程序入口点。

  @param[in] ImageHandle    加载该应用程序的镜像句柄
  @param[in] SystemTable    指向EFI系统表的指针

  @retval EFI_SUCCESS       应用程序退出正常
  @retval 其他              应用程序异常退出

**/
VOID
EFIAPI UpdateAcpiChecksum (
    IN EFI_ACPI_DESCRIPTION_HEADER *Table
)
{
    Table->Checksum = 0;  // 先清零
    Table->Checksum = CalculateCheckSum8((UINT8*)Table, Table->Length);
}

VOID
EFIAPI ChangeACPITable (
    IN UINT32             Signature,
    IN EFI_ACPI_DESCRIPTION_HEADER *NewTable
    )
{
    EFI_ACPI_6_3_ROOT_SYSTEM_DESCRIPTION_POINTER  *Root;

    EFI_CONFIGURATION_TABLE *configTab = gST->ConfigurationTable;
    UINTN i;
    for(i = 0; i < gST->NumberOfTableEntries; i++) {
        if((CompareGuid(&configTab[i].VendorGuid, &gEfiAcpiTableGuid)) ||
            (CompareGuid(&configTab[i].VendorGuid, &gEfiAcpi20TableGuid))) {
            
            Root = configTab[i].VendorTable;
            Print(L"RSDP at 0x%p\n", Root);
            Print(L"Length: %d\n", Root->Length);
            Print(L"Signature: %c%c%c%c%c%c%c%c\n", ((CHAR8*)&Root->Signature)[0], ((CHAR8*)&Root->Signature)[1], ((CHAR8*)&Root->Signature)[2], ((CHAR8*)&Root->Signature)[3], ((CHAR8*)&Root->Signature)[4], ((CHAR8*)&Root->Signature)[5], ((CHAR8*)&Root->Signature)[6], ((CHAR8*)&Root->Signature)[7]);
            Print(L"OEM ID: %c%c%c%c%c%c\n", ((CHAR8*)&Root->OemId)[0], ((CHAR8*)&Root->OemId)[1], ((CHAR8*)&Root->OemId)[2], ((CHAR8*)&Root->OemId)[3], ((CHAR8*)&Root->OemId)[4], ((CHAR8*)&Root->OemId)[5]);
            Print(L"Checksum: 0x%02x\n", Root->Checksum);
            
            // get the RSDT/XSDT
            EFI_ACPI_DESCRIPTION_HEADER *Xsdt;
            Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Root->XsdtAddress;
            if(Xsdt->Signature == Signature) {
                // change the table
                if(NewTable == NULL) {
                    ZeroMem(Xsdt->OemId, sizeof(Xsdt->OemId));
                }else{
                    CopyMem(Xsdt, NewTable, NewTable->Length);
                }
                UpdateAcpiChecksum(Xsdt);
            }
            Print(L"XSDT at 0x%p\n", Xsdt);
            Print(L"Length: %d\n", Xsdt->Length);
            Print(L"Signature: %c%c%c%c\n", ((CHAR8*)&Xsdt->Signature)[0], ((CHAR8*)&Xsdt->Signature)[1], ((CHAR8*)&Xsdt->Signature)[2], ((CHAR8*)&Xsdt->Signature)[3]);
            Print(L"OEM ID: %c%c%c%c%c%c\n", ((CHAR8*)&Xsdt->OemId)[0], ((CHAR8*)&Xsdt->OemId)[1], ((CHAR8*)&Xsdt->OemId)[2], ((CHAR8*)&Xsdt->OemId)[3], ((CHAR8*)&Xsdt->OemId)[4], ((CHAR8*)&Xsdt->OemId)[5]);
            Print(L"Checksum: 0x%02x\n", Xsdt->Checksum);
            UINTN EntryCount = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);

            // get the tables
            UINT64 *EntryPtr = (UINT64 *)(Xsdt + 1);
            for(UINTN j = 0; j < EntryCount; j++) {

                
                EFI_ACPI_DESCRIPTION_HEADER *Table = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)EntryPtr[j];
                if(Table->Signature == Signature) {
                    // change the table
                    
                    Print(L"Checksum: 0x%02x\n", Xsdt->Checksum);
                    if(NewTable == NULL) {
                        ZeroMem(Table->OemId, sizeof(Table->OemId));
                    }else{
                        CopyMem(Table, NewTable, NewTable->Length);
                    }
                    UpdateAcpiChecksum(Table);
                }
                Print(L"Table %d at 0x%p\n", j, Table);
                Print(L"Length: %d\n", Table->Length);
                Print(L"Signature: %c%c%c%c\n", ((CHAR8*)&Table->Signature)[0], ((CHAR8*)&Table->Signature)[1], ((CHAR8*)&Table->Signature)[2], ((CHAR8*)&Table->Signature)[3]);
                Print(L"OEM ID: %c%c%c%c%c%c\n", ((CHAR8*)&Table->OemId)[0], ((CHAR8*)&Table->OemId)[1], ((CHAR8*)&Table->OemId)[2], ((CHAR8*)&Table->OemId)[3], ((CHAR8*)&Table->OemId)[4], ((CHAR8*)&Table->OemId)[5]);
                Print(L"Checksum: 0x%02x\n", Table->Checksum);
                if (Table->Signature == EFI_ACPI_6_3_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
                    // FADT (Signature == FACP)
                    // find the another 2 table : DSDT and FACS
                    EFI_ACPI_6_3_FIXED_ACPI_DESCRIPTION_TABLE *Fadt = (EFI_ACPI_6_3_FIXED_ACPI_DESCRIPTION_TABLE *)Table;

                    EFI_ACPI_DESCRIPTION_HEADER *Dsdt;
                    Dsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Fadt->Dsdt;
                    // Print(L"-----------------------------%d %d\n",Dsdt->Signature,Signature);
                    if(Dsdt->Signature == Signature) {
                    // if(1){
                        // change the table
                        if(NewTable == NULL) {
                            ZeroMem(Dsdt->OemId, 6);
                        }else{
                            CopyMem(Dsdt, NewTable, NewTable->Length);
                        }
                        UpdateAcpiChecksum(Dsdt);
                    }
                    Print(L"DSDT at 0x%p\n", Dsdt);
                    Print(L"Length: %d\n", Dsdt->Length);
                    Print(L"Signature: %c%c%c%c\n", ((CHAR8*)&Dsdt->Signature)[0], ((CHAR8*)&Dsdt->Signature)[1], ((CHAR8*)&Dsdt->Signature)[2], ((CHAR8*)&Dsdt->Signature)[3]);
                    Print(L"OEM ID: %c%c%c%c%c%c\n", ((CHAR8*)&Dsdt->OemId)[0], ((CHAR8*)&Dsdt->OemId)[1], ((CHAR8*)&Dsdt->OemId)[2], ((CHAR8*)&Dsdt->OemId)[3], ((CHAR8*)&Dsdt->OemId)[4], ((CHAR8*)&Dsdt->OemId)[5]);
                    Print(L"Checksum: 0x%02x\n", Dsdt->Checksum);

                    EFI_ACPI_DESCRIPTION_HEADER *Facs;
                    Facs = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Fadt->FirmwareCtrl;
                    if(Facs->Signature == Signature) {
                        // change the table
                        if(NewTable == NULL) {
                            ZeroMem(Facs->OemId, sizeof(Facs->OemId));
                        }else{
                            CopyMem(Facs, NewTable, NewTable->Length);
                        }
                        UpdateAcpiChecksum(Facs);
                    }
                    Print(L"FACS at 0x%p\n", Facs);
                    Print(L"Length: %d\n", Facs->Length);
                    Print(L"Signature: %c%c%c%c\n", ((CHAR8*)&Facs->Signature)[0], ((CHAR8*)&Facs->Signature)[1], ((CHAR8*)&Facs->Signature)[2], ((CHAR8*)&Facs->Signature)[3]);
                    Print(L"OEM ID: %c%c%c%c%c%c\n", ((CHAR8*)&Facs->OemId)[0], ((CHAR8*)&Facs->OemId)[1], ((CHAR8*)&Facs->OemId)[2], ((CHAR8*)&Facs->OemId)[3], ((CHAR8*)&Facs->OemId)[4], ((CHAR8*)&Facs->OemId)[5]);
                    Print(L"Checksum: 0x%02x\n", Facs->Checksum);
                }
                
            }

        }
    }

    
    return;
}