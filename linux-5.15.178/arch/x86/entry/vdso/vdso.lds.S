/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Linker script for 64-bit vDSO.
 * We #include the file to define the layout details.
 *
 * This file defines the version script giving the user-exported symbols in
 * the DSO.
 */

#define BUILD_VDSO64

#include "vdso-layout.lds.S"

/*
 * This controls what userland symbols we export from the vDSO.
 */
VERSION {
	LINUX_2.6 {
	global:
		__vdso_set_vtask_base;
		__vdso_get_task_struct;
		__vdso_get_vtask_base;
		__vdso_get_task_pid;
        __vdso_get_task_tgid;
        __vdso_get_task_comm;
        __vdso_get_task_ppid;
		vdso_get_vtask_base;
        vdso_task_pid;
        vdso_task_tgid;
        vdso_task_comm;
        vdso_task_ppid;
		clock_gettime;
		__vdso_clock_gettime;
		gettimeofday;
		__vdso_gettimeofday;
		getcpu;
		__vdso_getcpu;
		time;
		__vdso_time;
		clock_getres;
		__vdso_clock_getres;
		__vdso_get_task_info;
#ifdef CONFIG_X86_SGX
		__vdso_sgx_enter_enclave;
#endif
	local: *;
	};
}

