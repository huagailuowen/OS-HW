copy_required_libraries() {
    local executable="$1"
    local output_dir="${2:-./extracted}"
    
    # 检查参数
    if [ -z "$executable" ]; then
        echo "错误: 请提供可执行文件路径"
        return 1
    fi
    
    if [ ! -f "$executable" ]; then
        echo "错误: 文件 '$executable' 不存在"
        return 1
    fi
    
    # 创建目录结构
    mkdir -p "$output_dir/lib/x86_64-linux-gnu"
    mkdir -p "$output_dir/lib64"
    
    # 获取所需库列表
    echo "分析 $executable 的库依赖..."
    local libs=$(ldd "$executable" | grep -v linux-vdso | awk '{print $3}')
    
    # 复制每个库
    for lib in $libs; do
        if [ -f "$lib" ]; then
            # 确定目标路径
            local relative_path=$(echo "$lib" | sed 's|^/||')
            local dir_path=$(dirname "$relative_path")
            
            # 创建目标目录
            mkdir -p "$output_dir/$dir_path"
            
            # 复制库文件
            echo "复制 $lib 到 $output_dir/$relative_path"
            cp -L "$lib" "$output_dir/$dir_path/"
            
            # 复制动态链接器特别处理
            if [[ "$lib" == *"ld-linux"* ]]; then
                if [[ "$lib" != "/lib64/"* ]]; then
                    echo "复制动态链接器 $lib 到 $output_dir/lib64/"
                    cp -L "$lib" "$output_dir/lib64/"
                fi
            fi
        fi
    done
    
    # 可能需要的额外库
    local extra_libs=("/lib/x86_64-linux-gnu/libm.so.6"
                      "/lib/x86_64-linux-gnu/libpthread.so.0"
                      "/lib/x86_64-linux-gnu/librt.so.1")
    
    for lib in "${extra_libs[@]}"; do
        if [ -f "$lib" ] && [ ! -f "$output_dir$lib" ]; then
            local dir_path=$(dirname "$lib")
            echo "复制额外库 $lib 到 $output_dir$lib"
            mkdir -p "$output_dir$dir_path"
            cp -L "$lib" "$output_dir$dir_path/"
        fi
    done
    
    # 创建运行脚本
    cat > "$output_dir/run.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib/x86_64-linux-gnu:$SCRIPT_DIR/lib64:$LD_LIBRARY_PATH"
"$SCRIPT_DIR/$(basename $1)" "${@:2}"
EOF
    chmod +x "$output_dir/run.sh"
    
    # 复制可执行文件
    cp "$executable" "$output_dir/"
    
    echo "完成！库文件已复制到 $output_dir"
    echo "使用以下命令在目标系统运行："
    echo "  cd $output_dir && ./run.sh $(basename $executable) [参数...]"
    
    return 0
}

# 使用示例：
# copy_required_libraries ./your_program [./output_directory]


# mkdir -p ./initramfs_work
# cp initramfs.cpio.gz ./initramfs_work
cd ./initramfs_work
# gunzip -k initramfs.cpio.gz
# mkdir -p extracted
# cd extracted
# cat ../initramfs.cpio | cpio -idmv
# cd ..
# cp -r ../../FS/Task1/test_read extracted/testfiles
# cp  /home/luowen/syshw/Net/Task1/client extracted/testfiles
# cp  /home/luowen/syshw/Net/Task1/server extracted/testfiles

# copy_required_libraries /home/luowen/syshw/Syscall/Task2/test_vdso  ./extracted


cp /home/luowen/syshw/Syscall/Task2/test_vdso extracted/testfiles/
# 回到解压目录
cd extracted

# 重新创建cpio归档
find . -print0 | cpio --null -ov --format=newc > ../new_initramfs.cpio

# 返回工作目录并压缩
cd ..
gzip -c new_initramfs.cpio > new_initramfs.cpio.gz
