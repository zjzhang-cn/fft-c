#!/bin/bash
# FM信号生成与解调测试脚本

echo "======================================"
echo "FM信号生成与解调系统测试"
echo "======================================"
echo

# 检查程序是否已编译
if [ ! -f "./fm_signal" ]; then
    echo "正在编译 FM 信号程序..."
    gcc -o fm_signal main-fm.c -lm -Wall
    if [ $? -ne 0 ]; then
        echo "编译失败！"
        exit 1
    fi
    echo "编译成功！"
    echo
fi

# 运行FM信号生成与解调
echo "运行 FM 信号生成与解调..."
echo "--------------------------------------"
./fm_signal
echo

# 检查输出文件
echo "--------------------------------------"
echo "检查输出文件:"
echo "--------------------------------------"
for file in fm_signal.txt fm_signal.csv fm_demodulated.txt fm_demodulated.csv fm_original_modulating.txt; do
    if [ -f "$file" ]; then
        size=$(wc -l < "$file")
        echo "✓ $file (行数: $size)"
    else
        echo "✗ $file (未找到)"
    fi
done
echo

# 显示部分数据
echo "--------------------------------------"
echo "FM信号数据示例 (前10行):"
echo "--------------------------------------"
head -n 11 fm_signal.txt
echo

echo "--------------------------------------"
echo "解调信号数据示例 (前10行):"
echo "--------------------------------------"
head -n 11 fm_demodulated.txt
echo

# 提供可视化建议
echo "======================================"
echo "可视化建议:"
echo "======================================"
echo
echo "1. 使用 Python 绘图 (推荐):"
echo "   python3 plot_fm.py"
echo
echo "2. 使用 gnuplot 快速查看:"
echo "   gnuplot -p -e \"plot 'fm_signal.txt' with lines\""
echo
echo "3. 对比解调结果:"
echo "   gnuplot -p -e \"plot 'fm_demodulated.txt' w l, 'fm_original_modulating.txt' w l\""
echo
echo "======================================"
echo "测试完成！"
echo "======================================"
