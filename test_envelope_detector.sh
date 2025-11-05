#!/bin/bash
# 包络检波器测试脚本

echo "========================================="
echo "  包络检波电路模拟器 - 测试脚本"
echo "========================================="
echo ""

# 编译程序
echo "【步骤 1】编译程序..."
gcc -o envelope_detector envelope_detector.c -lm -O2 -Wall

if [ $? -ne 0 ]; then
    echo "❌ 编译失败！"
    exit 1
fi
echo "✓ 编译成功"
echo ""

# 运行程序
echo "【步骤 2】运行包络检波器模拟..."
./envelope_detector

if [ $? -ne 0 ]; then
    echo "❌ 程序运行失败！"
    exit 1
fi
echo "✓ 程序运行成功"
echo ""

# 检查输出文件
echo "【步骤 3】检查输出文件..."
if [ -f "envelope_detector_result.csv" ]; then
    echo "✓ 找到输出文件: envelope_detector_result.csv"
    lines=$(wc -l < envelope_detector_result.csv)
    echo "  文件包含 $lines 行数据"
    
    # 显示文件头部
    echo ""
    echo "  文件前 5 行:"
    head -5 envelope_detector_result.csv | sed 's/^/    /'
else
    echo "❌ 未找到输出文件"
    exit 1
fi
echo ""

# 绘制结果（如果有 Python）
echo "【步骤 4】绘制结果图..."
if command -v python3 &> /dev/null; then
    # 检查必要的 Python 库
    python3 -c "import numpy, matplotlib, pandas" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "正在生成可视化图表..."
        python3 plot_envelope_detector.py
        
        if [ -f "envelope_detector_result.png" ]; then
            echo "✓ 结果图已生成: envelope_detector_result.png"
        fi
        
        # 生成其他图表
        echo ""
        echo "生成 RC 滤波器频率响应图..."
        python3 plot_envelope_detector.py --rc
        
        echo ""
        echo "生成二极管特性曲线..."
        python3 plot_envelope_detector.py --diode
        
    else
        echo "⚠ Python 库未安装，跳过绘图"
        echo "  请运行: pip3 install numpy matplotlib pandas"
    fi
else
    echo "⚠ 未找到 Python3，跳过绘图"
fi
echo ""

# 显示结果统计
echo "【步骤 5】结果统计..."
if command -v python3 &> /dev/null && python3 -c "import pandas" 2>/dev/null; then
    python3 << EOF
import pandas as pd
import numpy as np

data = pd.read_csv('envelope_detector_result.csv')
am_signal = data['AM_Signal'].values
demod = data['Demodulated'].values

print("AM 信号统计:")
print(f"  最大值: {np.max(am_signal):.4f} V")
print(f"  最小值: {np.min(am_signal):.4f} V")
print(f"  均值: {np.mean(am_signal):.4f} V")
print(f"  标准差: {np.std(am_signal):.4f} V")
print()
print("解调信号统计:")
print(f"  最大值: {np.max(demod):.4f} V")
print(f"  最小值: {np.min(demod):.4f} V")
print(f"  均值: {np.mean(demod):.4f} V")
print(f"  标准差: {np.std(demod):.4f} V")
EOF
fi
echo ""

echo "========================================="
echo "  测试完成！"
echo "========================================="
echo ""
echo "生成的文件:"
echo "  - envelope_detector (可执行文件)"
echo "  - envelope_detector_result.csv (数据文件)"
if [ -f "envelope_detector_result.png" ]; then
    echo "  - envelope_detector_result.png (结果图)"
fi
if [ -f "rc_filter_response.png" ]; then
    echo "  - rc_filter_response.png (RC 滤波器响应)"
fi
if [ -f "diode_characteristic.png" ]; then
    echo "  - diode_characteristic.png (二极管特性)"
fi
echo ""
echo "查看帮助文档:"
echo "  cat README-ENVELOPE-DETECTOR.md"
echo ""
