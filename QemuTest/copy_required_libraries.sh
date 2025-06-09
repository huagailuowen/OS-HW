#!/bin/sh


# copy_required_libraries.sh
# 使用ldd分析文件的依赖库并将其复制到指定目录
# 用法: sh copy_required_libraries.sh <executable> [target_directory]

# 移除 set -e 避免脚本在遇到非致命错误时退出
# set -e

# 默认参数
TARGET_DIR="/home/luowen/syshw/QemuTest/initramfs_work/extracted"

# 函数：显示用法
show_usage() {
    echo "用法: $0 <executable> [target_directory]"
    echo "  executable        - 要分析的可执行文件"
    echo "  target_directory  - 目标目录 (默认: $TARGET_DIR)"
    echo ""
    echo "示例:"
    echo "  $0 /usr/bin/ls"
    echo "  $0 ./my_program ./rootfs"
}

# 函数：检查文件是否存在
check_file() {
    local file="$1"
    if [[ ! -f "$file" ]]; then
        echo "错误: 文件 '$file' 不存在"
        exit 1
    fi
}

# 函数：创建目录结构
create_directories() {
    local base_dir="$1"
    
    # 创建常见的库目录
    mkdir -p "$base_dir/lib"
    mkdir -p "$base_dir/lib64"
    mkdir -p "$base_dir/usr/lib"
    mkdir -p "$base_dir/usr/lib64"
    mkdir -p "$base_dir/lib/x86_64-linux-gnu"
    mkdir -p "$base_dir/usr/lib/x86_64-linux-gnu"
    
    echo "创建目录结构完成"
}

# 函数：复制库文件
copy_library() {
    local lib_path="$1"
    local target_base="$2"
    local target_path
    
    # 跳过vdso等虚拟库
    if [[ "$lib_path" == *"vdso"* ]]; then
        return 0
    fi
    
    # 如果是绝对路径，保持相同的目录结构
    if [[ "$lib_path" == "/"* ]]; then
        target_path="$target_base$lib_path"
    else
        echo "  跳过非绝对路径: $lib_path"
        return 0
    fi
    
    # 创建目标目录
    local target_dir=$(dirname "$target_path")
    mkdir -p "$target_dir"
    
    # 检查目标文件是否已经存在，避免重复复制
    if [[ -f "$target_path" ]]; then
        return 0
    fi
    
    # 复制文件（如果存在）
    if [[ -f "$lib_path" ]]; then
        echo "  复制: $(basename "$lib_path")"
        cp -L "$lib_path" "$target_path" 2>/dev/null || {
            echo "    警告: 无法复制 $lib_path"
            return 1
        }
        
        # 如果是符号链接，也复制链接目标
        if [[ -L "$lib_path" ]]; then
            local link_target=$(readlink "$lib_path")
            # 如果是相对路径，转换为绝对路径
            if [[ "$link_target" != "/"* ]]; then
                local link_dir=$(dirname "$lib_path")
                link_target="$link_dir/$link_target"
            fi
            
            # 递归复制链接目标（但不返回其结果，避免中断主循环）
            if [[ -f "$link_target" ]] && [[ "$link_target" != "$lib_path" ]]; then
                echo "    -> 链接目标: $(basename "$link_target")"
                copy_library "$link_target" "$target_base" || true
            fi
        fi
        return 0
    else
        echo "    警告: 库文件不存在: $lib_path"
        return 1
    fi
}

# 函数：分析并复制依赖库
analyze_and_copy() {
    local executable="$1"
    local target_dir="$2"
    
    echo "分析 '$executable' 的依赖库..."
    
    # 使用ldd分析依赖
    local ldd_output
    ldd_output=$(ldd "$executable" 2>/dev/null) || {
        echo "错误: 无法使用ldd分析 '$executable'"
        echo "这可能不是一个动态链接的可执行文件"
        exit 1
    }
    
    echo "正在复制依赖库..."
    echo ""
    
    # 解析ldd输出并复制库文件
    local copied_count=0
    local failed_count=0
    
    while IFS= read -r line; do
        # 跳过空行
        [[ -z "$line" ]] && continue
        
        # 跳过 linux-vdso
        if [[ "$line" =~ linux-vdso ]]; then
            continue
        fi
        
        # 解析不同格式的ldd输出
        local lib_path=""
        
        # 格式1: libname.so => /path/to/lib (0x...)
        if [[ "$line" =~ "=>"[[:space:]]*([^[:space:]]+)[[:space:]]*"(" ]]; then
            lib_path="${BASH_REMATCH[1]}"
            echo "  找到库: $(basename "$lib_path")"
            echo "    DEBUG: 尝试复制 $lib_path"
        # 格式2: /lib64/ld-linux-x86-64.so.2 (0x...)  
        elif [[ "$line" =~ ^[[:space:]]*(/[^[:space:]]+)[[:space:]]*"(" ]]; then
            lib_path="${BASH_REMATCH[1]}"
            echo "  找到直接路径库: $(basename "$lib_path")"
            echo "    DEBUG: 尝试复制 $lib_path"
        fi
        
        # 复制库文件
        if [[ -n "$lib_path" ]] && [[ "$lib_path" != "(0x"* ]] && [[ "$lib_path" != *"vdso"* ]]; then
            echo "    DEBUG: 开始复制库文件 $lib_path"
            if copy_library "$lib_path" "$target_dir"; then
                ((copied_count++))
                echo "    DEBUG: 复制成功"
            else
                ((failed_count++))
                echo "    DEBUG: 复制失败"
            fi
        else
            echo "    DEBUG: 跳过库文件 $lib_path"
        fi
    done <<< "$ldd_output"
    
    echo ""
    echo "复制完成: 成功 $copied_count 个, 失败 $failed_count 个"
}

# 主程序
main() {
    # 检查参数
    if [[ $# -lt 1 ]]; then
        show_usage
        exit 1
    fi
    
    local executable="$1"
    local target_dir="${2:-$TARGET_DIR}"
    
    # 检查输入文件
    check_file "$executable"
    
    # 检查目标目录
    if [[ ! -d "$target_dir" ]]; then
        echo "目标目录 '$target_dir' 不存在，正在创建..."
        mkdir -p "$target_dir"
    fi
    
    echo "目标目录: $target_dir"
    echo ""
    
    # 创建必要的目录结构
    create_directories "$target_dir"
    
    # 分析并复制依赖库
    analyze_and_copy "$executable" "$target_dir"
    
    echo ""
    echo "依赖库复制完成!"
}

# 执行主程序
main "$@"