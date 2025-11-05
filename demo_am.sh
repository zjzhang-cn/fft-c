#!/bin/bash
# AM系统演示脚本 - 快速展示各种功能

echo "======================================"
echo "   AM调幅系统演示"
echo "======================================"
echo ""

# 检查程序是否存在
if [ ! -f "./am_signal" ]; then
    echo "程序不存在，正在编译..."
    gcc -o am_signal main-am.c -lm
    if [ $? -ne 0 ]; then
        echo "编译失败！"
        exit 1
    fi
    echo "编译成功！"
    echo ""
fi

echo "演示1: 标准AM信号（调制度80%）"
echo "--------------------------------------"
./am_signal -fc 10000 -fm 1000 -m 0.8 -d 0.01
echo ""

echo "演示2: 高调制度（90%）"
echo "--------------------------------------"
./am_signal -fc 10000 -fm 1000 -m 0.9 -d 0.01
echo ""

echo "演示3: 满调制（100%）"
echo "--------------------------------------"
./am_signal -fc 10000 -fm 1000 -m 1.0 -d 0.01
echo ""

echo "======================================"
echo "   演示完成！"
echo "======================================"
echo ""
echo "生成的文件："
ls -lh am_*.csv am_*.txt 2>/dev/null | awk '{print "  " $9, "(" $5 ")"}'
echo ""
echo "下一步："
echo "  1. 查看数据: head am_demodulated.csv"
echo "  2. 可视化: python3 plot_am.py"
echo "  3. 完整测试: ./test_am.sh"
echo "  4. 帮助信息: ./am_signal -h"
