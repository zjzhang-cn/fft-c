#!/bin/bash
# AM调制解调测试脚本

echo "=== AM调制解调测试脚本 ==="
echo ""

# 编译程序
echo "步骤1: 编译AM程序..."
gcc -o am_signal main-am.c -lm
if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi
echo "编译成功！"
echo ""

# 测试1: 基本AM信号（调制度80%）
echo "测试1: 标准AM信号（调制度80%）"
echo "----------------------------------------"
./am_signal -fc 10000 -fm 1000 -fs 100000 -d 0.01 -m 0.8
echo ""

# 测试2: 不同调制度
echo "测试2: 低调制度（调制度30%）"
echo "----------------------------------------"
./am_signal -fc 10000 -fm 1000 -fs 100000 -d 0.01 -m 0.3
mv am_signal.csv am_signal_m30.csv
mv am_demodulated.csv am_demodulated_m30.csv
echo ""

echo "测试3: 满调制（调制度100%）"
echo "----------------------------------------"
./am_signal -fc 10000 -fm 1000 -fs 100000 -d 0.01 -m 1.0
mv am_signal.csv am_signal_m100.csv
mv am_demodulated.csv am_demodulated_m100.csv
echo ""

echo "测试4: 过调制（调制度120%）"
echo "----------------------------------------"
./am_signal -fc 10000 -fm 1000 -fs 100000 -d 0.01 -m 1.2
mv am_signal.csv am_signal_m120.csv
mv am_demodulated.csv am_demodulated_m120.csv
echo ""

# 测试5: 不同载波频率
echo "测试5: 高频载波（100kHz载波，5kHz调制）"
echo "----------------------------------------"
./am_signal -fc 100000 -fm 5000 -fs 500000 -d 0.002 -m 0.8
mv am_signal.csv am_signal_hf.csv
mv am_demodulated.csv am_demodulated_hf.csv
echo ""

# 测试6: 长时间信号
echo "测试6: 长时间信号（50ms）"
echo "----------------------------------------"
./am_signal -fc 10000 -fm 1000 -fs 100000 -d 0.05 -m 0.8
mv am_signal.csv am_signal_long.csv
mv am_demodulated.csv am_demodulated_long.csv
echo ""

# 恢复默认测试的文件
echo "恢复标准测试结果..."
./am_signal -fc 10000 -fm 1000 -fs 100000 -d 0.01 -m 0.8
echo ""

echo "=== 所有测试完成 ==="
echo ""
echo "生成的文件："
echo "  标准测试:"
echo "    - am_signal.txt, am_signal.csv, am_demodulated.csv"
echo "  不同调制度:"
echo "    - am_signal_m30.csv, am_demodulated_m30.csv"
echo "    - am_signal_m100.csv, am_demodulated_m100.csv"
echo "    - am_signal_m120.csv, am_demodulated_m120.csv"
echo "  高频载波:"
echo "    - am_signal_hf.csv, am_demodulated_hf.csv"
echo "  长时间:"
echo "    - am_signal_long.csv, am_demodulated_long.csv"
echo ""

# 检查是否有Python可用于绘图
if command -v python3 &> /dev/null; then
    echo "检测到Python3，可以运行 plot_am.py 进行可视化"
    echo "运行: python3 plot_am.py"
elif command -v python &> /dev/null; then
    echo "检测到Python，可以运行 plot_am.py 进行可视化"
    echo "运行: python plot_am.py"
else
    echo "未检测到Python，请手动使用其他工具查看CSV文件"
fi
