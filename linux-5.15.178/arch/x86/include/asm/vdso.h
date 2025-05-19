/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_VDSO_H
#define _ASM_X86_VDSO_H

#include <asm/page_types.h>
#include <linux/linkage.h>
#include <linux/init.h>

#ifndef __ASSEMBLER__

#include <linux/mm_types.h>

/* vtask 元数据结构 */
struct vtask_metadata {
    uint64_t magic;          // 固定8字节
    uint64_t task_offset;    
    uint64_t task_struct_addr;
    uint32_t pid;            // 明确4字节
    uint64_t timestamp;      // 固定8字节
} __attribute__((packed));   // 保留packed防止编译器插入填充

/* 全局变量声明 */
extern struct page *vtask_pages[6];  /* 0: 元数据页, 1: task_struct 页 */
extern struct vtask_metadata *vtask_meta; /* 元数据指针 */
extern void update_vtask_page(struct task_struct *task);

struct vdso_image {
	void *data;
	unsigned long size;   /* Always a multiple of PAGE_SIZE */

	unsigned long alt, alt_len;
	unsigned long extable_base, extable_len;
	const void *extable;

	long sym_vvar_start;  /* Negative offset to the vvar area */

	long sym_vvar_page;
	long sym_pvclock_page;
	long sym_hvclock_page;
	long sym_timens_page;
	long sym_VDSO32_NOTE_MASK;
	long sym___kernel_sigreturn;
	long sym___kernel_rt_sigreturn;
	long sym___kernel_vsyscall;
	long sym_int80_landing_pad;
	long sym_vdso32_sigreturn_landing_pad;
	long sym_vdso32_rt_sigreturn_landing_pad;
};

#ifdef CONFIG_X86_64
extern const struct vdso_image vdso_image_64;
#endif

#ifdef CONFIG_X86_X32
extern const struct vdso_image vdso_image_x32;
#endif

#if defined CONFIG_X86_32 || defined CONFIG_COMPAT
extern const struct vdso_image vdso_image_32;
#endif

extern void __init init_vdso_image(const struct vdso_image *image);

extern int map_vdso_once(const struct vdso_image *image, unsigned long addr);

extern bool fixup_vdso_exception(struct pt_regs *regs, int trapnr,
				 unsigned long error_code,
				 unsigned long fault_addr);
#endif /* __ASSEMBLER__ */

#endif /* _ASM_X86_VDSO_H */
