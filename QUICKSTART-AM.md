# AM调幅系统快速入门

## 快速开始

### 1. 编译程序

```bash
# 方法1: 使用Makefile
make am_signal

# 方法2: 直接使用gcc
gcc -o am_signal main-am.c -lm
```

### 2. 运行基本示例

```bash
# 使用默认参数运行
./am_signal
```

默认参数：
- 载波频率：10 kHz
- 调制频率：1 kHz
- 采样频率：100 kHz
- 持续时间：10 ms
- 调制指数：0.8 (80%)

### 3. 自定义参数

```bash
# 完整示例
./am_signal -fc 20000 -fm 2000 -fs 200000 -d 0.02 -m 0.5

# 参数说明
-fc <Hz>    载波频率
-fm <Hz>    调制频率
-fs <Hz>    采样频率
-d <秒>     持续时间
-m <0-1>    调制指数
-h          帮助信息
```

### 4. 运行完整测试

```bash
# 运行测试脚本（包含多个测试案例）
./test_am.sh
```

测试脚本会生成：
- 不同调制度的信号（30%, 80%, 100%, 120%）
- 高频载波测试
- 长时间信号测试

### 5. 可视化结果

```bash
# 使用Python绘制结果
python3 plot_am.py

# 或
python plot_am.py
```

会生成以下图片：
- `am_modulation.png` - AM调制过程
- `am_demodulation.png` - 解调结果详细对比
- `am_demod_comparison.png` - 解调方法总对比
- `am_spectrum.png` - AM信号频谱
- `am_modulation_index_comparison.png` - 不同调制指数对比

## 输出文件

### 文本文件
- `am_signal.txt` - AM信号时域数据（制表符分隔）
- `am_signal.csv` - 包含调制信号、载波和AM信号（CSV格式）
- `am_demodulated.csv` - 原始信号与三种解调结果对比

### 查看数据

```bash
# 查看前10行
head am_demodulated.csv

# 使用电子表格软件
libreoffice am_signal.csv
# 或
gnumeric am_signal.csv
```

## 应用示例

### 示例1: 标准AM广播模拟

```bash
# 中波AM广播：1000 kHz载波，5 kHz音频
./am_signal -fc 1000000 -fm 5000 -fs 5000000 -d 0.001 -m 0.7
```

### 示例2: 测试过调制

```bash
# 调制度120%（会产生失真）
./am_signal -fc 10000 -fm 1000 -m 1.2
```

### 示例3: 低调制度

```bash
# 调制度30%（功率效率低）
./am_signal -fc 10000 -fm 1000 -m 0.3
```

### 示例4: 多频调制（需要修改代码）

在代码中已经实现了 `generate_am_signal_complex()` 函数，
可以生成包含多个频率分量的复杂调制信号。

## 性能评估

程序自动计算三种解调方法的SNR（信噪比）：

```
解调性能对比:
  包络检波法 SNR = XX.XX dB
  Hilbert变换法 SNR = XX.XX dB
  相干解调法 SNR = XX.XX dB
```

一般来说：
- **相干解调** 性能最好（需要载波同步）
- **Hilbert变换** 性能中等
- **包络检波** 最简单但性能较低

## 理论基础

### AM信号公式

$$s_{AM}(t) = [1 + \mu \cdot m(t)] \cdot \cos(2\pi f_c t)$$

### 调制指数

$$\mu = \frac{A_{max} - A_{min}}{A_{max} + A_{min}}$$

### 带宽

$$BW = 2 \cdot f_{m(max)}$$

## 故障排除

### 问题1: 编译错误

```bash
# 确保安装了gcc和math库
sudo apt-get install build-essential
```

### 问题2: Python绘图失败

```bash
# 安装必要的Python库
pip3 install pandas matplotlib numpy
```

### 问题3: 权限问题

```bash
# 添加执行权限
chmod +x test_am.sh plot_am.py
```

## 扩展学习

详细文档请参考：`README-AM.md`

包含内容：
- AM调制原理详解
- 三种解调方法的数学推导
- 频谱分析
- 应用场景
- 参考资料

## 项目集成

本AM系统是FFT-C项目的一部分，其他模块包括：
- DTMF信号生成器
- FM调频系统
- 2D FFT图像处理
- K空间重建

查看主README了解更多：`README.md`
