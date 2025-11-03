#!/bin/bash

echo "=========================================="
echo "  K空间数据重建 - 完整测试流程"
echo "=========================================="
echo

# 清理之前的输出文件
echo "步骤 1: 清理旧文件..."
rm -f *.bmp kspace_data.bin kspace_data.txt
echo "  ✓ 完成"
echo

# 生成测试图像并计算K空间
echo "步骤 2: 生成测试图像并计算K空间..."
./fft2d > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✓ 完成"
    echo "  - 生成文件: original_image.bmp"
    echo "  - 生成文件: kspace_data.bin ($(ls -lh kspace_data.bin | awk '{print $5}'))"
else
    echo "  ✗ 失败"
    exit 1
fi
echo

# 从K空间还原图像
echo "步骤 3: 从K空间还原图像..."
./kspace_to_image kspace_data.bin > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✓ 完成"
    echo "  - 生成文件: reconstructed_image.bmp"
else
    echo "  ✗ 失败"
    exit 1
fi
echo

# 比较文件大小
echo "步骤 4: 验证图像还原..."
orig_size=$(stat -c%s original_image.bmp 2>/dev/null || stat -f%z original_image.bmp 2>/dev/null)
rest_size=$(stat -c%s reconstructed_image.bmp 2>/dev/null || stat -f%z reconstructed_image.bmp 2>/dev/null)

if [ "$orig_size" = "$rest_size" ]; then
    echo "  ✓ 文件大小一致: $orig_size 字节"
else
    echo "  ⚠ 文件大小不同: original=$orig_size, reconstructed=$rest_size"
fi
echo

# 显示所有生成的文件
echo "=========================================="
echo "  生成的文件列表"
echo "=========================================="
ls -lh *.bmp *.bin 2>/dev/null | awk '{printf "  %-40s %8s\n", $9, $5}'
echo

echo "=========================================="
echo "  测试完成！"
echo "=========================================="
echo
echo "可以查看以下图像文件进行对比："
echo "  - original_image.bmp          (原始图像)"
echo "  - reconstructed_image.bmp     (还原图像)"
echo "  - restored_image.bmp          (fft2d内部还原)"
echo
echo "理论上 original_image.bmp 应该与 reconstructed_image.bmp 完全一致"
echo "（可能存在微小的浮点运算误差）"
echo
