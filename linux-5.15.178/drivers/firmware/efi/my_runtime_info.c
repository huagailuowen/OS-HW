// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/efi.h>  // 只包含这个头文件处理EFI
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/slab.h>

// 定义与UEFI一致的GUID (与你的RuntimeInfoDxe.c中相同)
#define MY_GUID \
    EFI_GUID(0x12345678, 0x9abc, 0xdef0, 0x12, 0x34, 0x56, 0x78, 0xab, 0xcd, 0xef, 0x00)

static char info_buf[256];
static struct kobject *runtime_kobj;

static ssize_t info_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sysfs_emit(buf, "%s\n", info_buf);
}

static struct kobj_attribute info_attr = __ATTR_RO(info);

static int call_runtime_protocol(void)
{
    efi_char16_t var_name[] = L"MyTestInfo"; // 先尝试测试变量
    efi_guid_t guid = EFI_GLOBAL_VARIABLE_GUID; // 使用全局变量GUID
    unsigned long size = sizeof(info_buf) - 1;
    u32 attr;
    efi_status_t status;
    
    // 使用 efi.get_variable 而不是 efi_get_variable
    if (!efi.get_variable) {
        pr_err("MyRuntimeInfo: efi.get_variable function not available\n");
        return -ENOSYS;
    }
    
    // 首先尝试读取测试变量
    status = efi.get_variable(var_name, &guid, &attr, &size, info_buf);
    
    if (status == EFI_SUCCESS) {
        info_buf[size] = '\0';
        pr_info("Successfully read test variable MyTestInfo\n");
        return 0;
    }
    
    // 如果测试变量读取失败，尝试读取实际变量
    efi_char16_t runtime_var_name[] = L"MyRuntimeInfo";
    guid = MY_GUID;
    size = sizeof(info_buf) - 1;
    
    status = efi.get_variable(runtime_var_name, &guid, &attr, &size, info_buf);
    
    if (status != EFI_SUCCESS) {
        pr_err("Failed to get EFI variable MyRuntimeInfo: status=%lx\n", 
               (unsigned long)status);
        pr_info("GUID used: %pUl\n", &guid);
        return -ENODEV;
    }
    
    info_buf[size] = '\0';
    return 0;
}

static int __init my_runtime_info_init(void)
{
    int ret;
    
    /* 检查 EFI 运行时服务是否可用 */
    if (!efi_enabled(EFI_RUNTIME_SERVICES)) {
        pr_info("MyRuntimeInfo: EFI runtime services not available\n");
        return -ENODEV;
    }
    
    /* 尝试获取UEFI信息 */
    ret = call_runtime_protocol();
    if (ret != 0) {
        /* 如果调用失败，使用模拟数据 */
        pr_info("MyRuntimeInfo: Using simulation data due to EFI call failure\n");
        snprintf(info_buf, sizeof(info_buf), "Failed to get EFI variable");
    }
    
    pr_info("MyRuntimeInfo: Module initialized with info: %s\n", info_buf);
    
    /* 创建 sysfs 节点 */
    runtime_kobj = kobject_create_and_add("runtime_info", firmware_kobj);
    if (!runtime_kobj)
        return -ENOMEM;
    
    if (sysfs_create_file(runtime_kobj, &info_attr.attr)) {
        kobject_put(runtime_kobj);
        return -EFAULT;
    }
    
    return 0;
}

static void __exit my_runtime_info_exit(void)
{
    if (runtime_kobj) {
        sysfs_remove_file(runtime_kobj, &info_attr.attr);
        kobject_put(runtime_kobj);
        pr_info("MyRuntimeInfo: Module unloaded\n");
    }
}

module_init(my_runtime_info_init);
module_exit(my_runtime_info_exit);

MODULE_AUTHOR("luowen");
MODULE_DESCRIPTION("Access UEFI Runtime Hardware Info");
MODULE_LICENSE("GPL");