## test acpi
use My.inf to test acpi
in hello.efi
```bash
cp /home/luowen/software/edk2/Build/Mde/DEBUG_GCC5/X64/MyPkg/My/DEBUG/hello.efi /home/luowen/syshw/QemuTest/uefi/acpi.efi
```

### wrong output
```
dmesg | grep ACPI
[    0.000000] BIOS-e820: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x0000000000811000-0x00000000008fffff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bf76d000-0x00000000bf77efff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000bf77f000-0x00000000bf7fefff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bfeb3000-0x00000000bfeb4fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bff78000-0x00000000bfffffff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x0000000000811000-0x00000000008fffff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bf76d000-0x00000000bf77efff] ACPI data
[    0.000000] reserve setup_data: [mem 0x00000000bf77f000-0x00000000bf7fefff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bfeb3000-0x00000000bfeb4fff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bff78000-0x00000000bfffffff] ACPI NVS
[    0.000000] efi: SMBIOS=0xbf53f000 SMBIOS 3.0=0xbf53d000 ACPI=0xbf77e000 ACPI 2.0=0xbf77e014 MEMATTR=0xbe15a018 
[    0.024420] ACPI: Early table checksum verification disabled
[    0.024704] ACPI: RSDP 0x00000000BF77E014 000024 (v02 BOCHS )
[    0.024910] ACPI: XSDT 0x00000000BF77D0E8 00004C (v01 BOCHS  BXPC     00000001      01000013)
[    0.025293] ACPI: FACP 0x00000000BF77A000 000074 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.025724] ACPI: DSDT 0x00000000BF77B000 001AB4 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.025794] ACPI: FACS 0x00000000BF7DD000 000040
[    0.025840] ACPI: APIC 0x00000000BF779000 000078 (v03 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.025857] ACPI: HPET 0x00000000BF778000 000038 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.025870] ACPI: WAET 0x00000000BF777000 000028 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.025883] ACPI: BGRT 0x00000000BF776000 000038 (v01 INTEL  EDK2     00000002      01000013)
[    0.025951] ACPI: Reserving FACP table memory at [mem 0xbf77a000-0xbf77a073]
[    0.025981] ACPI: Reserving DSDT table memory at [mem 0xbf77b000-0xbf77cab3]
[    0.025987] ACPI: Reserving FACS table memory at [mem 0xbf7dd000-0xbf7dd03f]
[    0.025992] ACPI: Reserving APIC table memory at [mem 0xbf779000-0xbf779077]
[    0.025997] ACPI: Reserving HPET table memory at [mem 0xbf778000-0xbf778037]
[    0.026002] ACPI: Reserving WAET table memory at [mem 0xbf777000-0xbf777027]
[    0.026007] ACPI: Reserving BGRT table memory at [mem 0xbf776000-0xbf776037]
[    0.083373] ACPI: PM-Timer IO Port: 0xb008
[    0.083749] ACPI: LAPIC_NMI (acpi_id[0xff] dfl dfl lint[0x1])
[    0.084147] ACPI: INT_SRC_OVR (bus 0 bus_irq 0 global_irq 2 dfl dfl)
[    0.084330] ACPI: INT_SRC_OVR (bus 0 bus_irq 5 global_irq 5 high level)
[    0.084364] ACPI: INT_SRC_OVR (bus 0 bus_irq 9 global_irq 9 high level)
[    0.084428] ACPI: INT_SRC_OVR (bus 0 bus_irq 10 global_irq 10 high level)
[    0.084435] ACPI: INT_SRC_OVR (bus 0 bus_irq 11 global_irq 11 high level)
[    0.084561] ACPI: Using ACPI (MADT) for SMP configuration information
[    0.084595] ACPI: HPET id: 0x8086a201 base: 0xfed00000
[    0.576741] ACPI: Core revision 20210730
[    1.007384] ACPI: PM: Registering ACPI NVS region [mem 0x00800000-0x00807fff] (32768 bytes)
[    1.007699] ACPI: PM: Registering ACPI NVS region [mem 0x0080b000-0x0080bfff] (4096 bytes)
[    1.007862] ACPI: PM: Registering ACPI NVS region [mem 0x00811000-0x008fffff] (978944 bytes)
[    1.008073] ACPI: PM: Registering ACPI NVS region [mem 0xbf77f000-0xbf7fefff] (524288 bytes)
[    1.008378] ACPI: PM: Registering ACPI NVS region [mem 0xbfeb3000-0xbfeb4fff] (8192 bytes)
[    1.008638] ACPI: PM: Registering ACPI NVS region [mem 0xbff78000-0xbfffffff] (557056 bytes)
[    1.033239] ACPI: bus type PCI registered
[    1.086080] ACPI: Added _OSI(Module Device)
[    1.086655] ACPI: Added _OSI(Processor Device)
[    1.086773] ACPI: Added _OSI(3.0 _SCP Extensions)
[    1.087354] ACPI: Added _OSI(Processor Aggregator Device)
[    1.087558] ACPI: Added _OSI(Linux-Dell-Video)
[    1.087803] ACPI: Added _OSI(Linux-Lenovo-NV-HDMI-Audio)
[    1.087907] ACPI: Added _OSI(Linux-HPI-Hybrid-Graphics)
[    1.106433] ACPI: 1 ACPI AML tables successfully acquired and loaded
[    1.127871] ACPI: Interpreter enabled
[    1.129453] ACPI: PM: (supports S0 S3 S4 S5)
[    1.129713] ACPI: Using IOAPIC for interrupt routing
[    1.130435] PCI: Using host bridge windows from ACPI; if necessary, use "pci=nocrs" and report a bug
[    1.132211] ACPI: Enabled 2 GPEs in block 00 to 0F
[    1.166826] ACPI: PCI Root Bridge [PCI0] (domain 0000 [bus 00-ff])
[    1.190958] pci 0000:00:01.3: quirk: [io  0xb000-0xb03f] claimed by PIIX4 ACPI
[    1.212994] ACPI: PCI: Interrupt link LNKA configured for IRQ 10
[    1.213753] ACPI: PCI: Interrupt link LNKB configured for IRQ 10
[    1.214178] ACPI: PCI: Interrupt link LNKC configured for IRQ 11
[    1.214633] ACPI: PCI: Interrupt link LNKD configured for IRQ 11
[    1.214935] ACPI: PCI: Interrupt link LNKS configured for IRQ 9
[    1.227620] ACPI: bus type USB registered
[    1.245520] PCI: Using ACPI for IRQ routing
[    1.309163] pnp: PnP ACPI init
[    1.317383] pnp: PnP ACPI: found 6 devices
[    1.819014] ACPI: button: Power Button [PWRF]
[    1.948190] ACPI: \_SB_.LNKC: Enabled at IRQ 11
```

### right output
```
dmesg | grep ACPI[    9.632281] clocksource: timekeeping watchdog on CPU0: Marking clocksource 'tsc' as unstable because the skew is too large:
[    9.632550] clocksource:                       'hpet' wd_nsec: 496465510 wd_now: 35cb8d4b wd_last: 32d60174 mask: ffffffff
[    9.632716] clocksource:                       'tsc' cs_nsec: 495601885 cs_now: ede20d106 cs_last: e7b28bbd8 mask: ffffffffffffffff
[    9.632905] clocksource:                       'tsc' is current clocksource.
[    9.633116] tsc: Marking TSC unstable due to clocksource watchdog
[    9.633370] TSC found unstable after boot, most likely due to broken BIOS. Use 'tsc=unstable'.
[    9.633582] sched_clock: Marking unstable (9591130806, 42179201)<-(9639096934, -5782662)
[    9.634668] clocksource: Not enough CPUs to check clocksource 'tsc'.
[    9.634921] clocksource: Switched to clocksource hpet

[    0.000000] BIOS-e820: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x0000000000811000-0x00000000008fffff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bf76d000-0x00000000bf77efff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000bf77f000-0x00000000bf7fefff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bfeb3000-0x00000000bfeb4fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bff78000-0x00000000bfffffff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x0000000000811000-0x00000000008fffff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bf76d000-0x00000000bf77efff] ACPI data
[    0.000000] reserve setup_data: [mem 0x00000000bf77f000-0x00000000bf7fefff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bfeb3000-0x00000000bfeb4fff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bff78000-0x00000000bfffffff] ACPI NVS
[    0.000000] efi: SMBIOS=0xbf53f000 SMBIOS 3.0=0xbf53d000 ACPI=0xbf77e000 ACPI 2.0=0xbf77e014 MEMATTR=0xbe15f118 
[    0.024367] ACPI: Early table checksum verification disabled
[    0.024638] ACPI: RSDP 0x00000000BF77E014 000024 (v02 BOCHS )
[    0.024848] ACPI: XSDT 0x00000000BF77D0E8 00004C (v01 BOCHS  BXPC     00000001      01000013)
[    0.025336] ACPI: FACP 0x00000000BF77A000 000074 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.025844] ACPI: DSDT 0x00000000BF77B000 001AB4 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.026100] ACPI: FACS 0x00000000BF7DD000 000040
[    0.026183] ACPI: APIC 0x00000000BF779000 000078 (v03 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.026202] ACPI: HPET 0x00000000BF778000 000038 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.026217] ACPI: WAET 0x00000000BF777000 000028 (v01 BOCHS  BXPC     00000001 BXPC 00000001)
[    0.026232] ACPI: BGRT 0x00000000BF776000 000038 (v01 INTEL  EDK2     00000002      01000013)
[    0.026316] ACPI: Reserving FACP table memory at [mem 0xbf77a000-0xbf77a073]
[    0.026355] ACPI: Reserving DSDT table memory at [mem 0xbf77b000-0xbf77cab3]
[    0.026363] ACPI: Reserving FACS table memory at [mem 0xbf7dd000-0xbf7dd03f]
[    0.026370] ACPI: Reserving APIC table memory at [mem 0xbf779000-0xbf779077]
[    0.026376] ACPI: Reserving HPET table memory at [mem 0xbf778000-0xbf778037]
[    0.026381] ACPI: Reserving WAET table memory at [mem 0xbf777000-0xbf777027]
[    0.026386] ACPI: Reserving BGRT table memory at [mem 0xbf776000-0xbf776037]
[    0.086969] ACPI: PM-Timer IO Port: 0xb008
[    0.087343] ACPI: LAPIC_NMI (acpi_id[0xff] dfl dfl lint[0x1])
[    0.087737] ACPI: INT_SRC_OVR (bus 0 bus_irq 0 global_irq 2 dfl dfl)
[    0.087924] ACPI: INT_SRC_OVR (bus 0 bus_irq 5 global_irq 5 high level)
[    0.087960] ACPI: INT_SRC_OVR (bus 0 bus_irq 9 global_irq 9 high level)
[    0.088026] ACPI: INT_SRC_OVR (bus 0 bus_irq 10 global_irq 10 high level)
[    0.088033] ACPI: INT_SRC_OVR (bus 0 bus_irq 11 global_irq 11 high level)
[    0.088166] ACPI: Using ACPI (MADT) for SMP configuration information
[    0.088200] ACPI: HPET id: 0x8086a201 base: 0xfed00000
[    0.606533] ACPI: Core revision 20210730
[    1.063448] ACPI: PM: Registering ACPI NVS region [mem 0x00800000-0x00807fff] (32768 bytes)
[    1.063902] ACPI: PM: Registering ACPI NVS region [mem 0x0080b000-0x0080bfff] (4096 bytes)
[    1.064110] ACPI: PM: Registering ACPI NVS region [mem 0x00811000-0x008fffff] (978944 bytes)
[    1.064350] ACPI: PM: Registering ACPI NVS region [mem 0xbf77f000-0xbf7fefff] (524288 bytes)
[    1.064518] ACPI: PM: Registering ACPI NVS region [mem 0xbfeb3000-0xbfeb4fff] (8192 bytes)
[    1.064874] ACPI: PM: Registering ACPI NVS region [mem 0xbff78000-0xbfffffff] (557056 bytes)
[    1.087008] ACPI: bus type PCI registered
[    1.122934] ACPI: Added _OSI(Module Device)
[    1.123255] ACPI: Added _OSI(Processor Device)
[    1.123512] ACPI: Added _OSI(3.0 _SCP Extensions)
[    1.123589] ACPI: Added _OSI(Processor Aggregator Device)
[    1.123746] ACPI: Added _OSI(Linux-Dell-Video)
[    1.123856] ACPI: Added _OSI(Linux-Lenovo-NV-HDMI-Audio)
[    1.123879] ACPI: Added _OSI(Linux-HPI-Hybrid-Graphics)
[    1.137108] ACPI: 1 ACPI AML tables successfully acquired and loaded
[    1.151375] ACPI: Interpreter enabled
[    1.152408] ACPI: PM: (supports S0 S3 S4 S5)
[    1.152665] ACPI: Using IOAPIC for interrupt routing
[    1.153219] PCI: Using host bridge windows from ACPI; if necessary, use "pci=nocrs" and report a bug
[    1.154478] ACPI: Enabled 2 GPEs in block 00 to 0F
[    1.178020] ACPI: PCI Root Bridge [PCI0] (domain 0000 [bus 00-ff])
[    1.197141] pci 0000:00:01.3: quirk: [io  0xb000-0xb03f] claimed by PIIX4 ACPI
[    1.216612] ACPI: PCI: Interrupt link LNKA configured for IRQ 10
[    1.217396] ACPI: PCI: Interrupt link LNKB configured for IRQ 10
[    1.218033] ACPI: PCI: Interrupt link LNKC configured for IRQ 11
[    1.218525] ACPI: PCI: Interrupt link LNKD configured for IRQ 11
[    1.218913] ACPI: PCI: Interrupt link LNKS configured for IRQ 9
[    1.231255] ACPI: bus type USB registered
[    1.247082] PCI: Using ACPI for IRQ routing
[    1.309221] pnp: PnP ACPI init
[    1.312240] pnp: PnP ACPI: found 6 devices
[    1.857347] ACPI: button: Power Button [PWRF]
[    1.995562] ACPI: \_SB_.LNKC: Enabled at IRQ 11
```