# FM调频信号生成与解调系统

## 📡 概述

本模块实现了完整的调频(FM)信号生成和解调系统，包括信号生成、多种解调算法和性能分析。

## 🎯 功能特性

### 信号生成
- **调频信号生成器**: 生成标准FM信号 `s(t) = cos(2πfc·t + β·sin(2πfm·t))`
- **可配置参数**:
  - 载波频率 (fc)
  - 调制频率 (fm)
  - 调制指数 (β)
  - 采样频率 (fs)
  - 信号持续时间

### 解调算法

实现了三种FM解调方法：

1. **瞬时频率法** (`fm_demodulate`)
   - 基于信号导数计算瞬时频率
   - 计算简单但精度较低

2. **相位鉴频器** (`fm_demodulate_phase_discriminator`)
   - 使用Hilbert变换构造解析信号
   - 通过相位差分计算瞬时频率
   - 包含相位解包装(unwrapping)

3. **解析信号法** (`fm_demodulate_analytic`) ⭐ **推荐**
   - 使用离散Hilbert变换
   - 精确计算瞬时相位和频率
   - 最高精度，适合实际应用

### 信号处理
- **低通滤波器**: 移动平均滤波，去除高频噪声
- **直流去除**: 消除解调信号的直流偏移
- **误差分析**: MSE、SNR等性能指标

## 🚀 快速开始

### 编译

```bash
gcc -o fm_signal main-fm.c -lm -Wall
```

### 运行

```bash
./fm_signal
```

### 输出文件

程序会生成以下文件：

- `fm_signal.txt/csv` - 原始FM调制信号
- `fm_demodulated.txt/csv` - 解调后的信号
- `fm_original_modulating.txt` - 理论调制信号(用于对比)

## 📊 可视化

### 使用Python绘图

```bash
python3 plot_fm.py
```

这将生成 `fm_demodulation_result.png`，包含：
- 原始FM信号波形
- 解调信号vs理论值对比
- 解调误差分析

### 使用gnuplot

绘制FM信号：
```bash
gnuplot -e "set term png; set output 'fm_signal.png'; plot 'fm_signal.txt' with lines title 'FM Signal'"
```

对比解调结果：
```bash
gnuplot -e "set term png; set output 'comparison.png'; plot 'fm_demodulated.txt' with lines title 'Demodulated', 'fm_original_modulating.txt' with lines title 'Original'"
```

## 📖 技术细节

### FM调制原理

调频信号的数学表达式：

$$s(t) = A \cos\left(2\pi f_c t + \beta \sin(2\pi f_m t)\right)$$

其中：
- $f_c$ = 载波频率
- $f_m$ = 调制信号频率
- $\beta$ = 调制指数
- $\Delta f = \beta \cdot f_m$ = 最大频率偏移

### Carson带宽规则

FM信号的带宽估计：

$$BW \approx 2(\Delta f + f_m) = 2((\beta + 1) \cdot f_m)$$

### 解调原理

**瞬时频率**定义为相位对时间的导数：

$$f_{inst}(t) = \frac{1}{2\pi} \frac{d\phi(t)}{dt}$$

**Hilbert变换**用于构造解析信号：

$$s_a(t) = s(t) + j\mathcal{H}\{s(t)\}$$

瞬时相位：

$$\phi(t) = \arctan\left(\frac{\mathcal{H}\{s(t)\}}{s(t)}\right)$$

### 当前性能

使用默认参数(fc=1000Hz, fm=100Hz, β=5):

- **理论最大频偏**: 500 Hz
- **实际解调幅度**: ~530 Hz  
- **信噪比**: -2.7 dB
- **RMS误差**: ~330 Hz

性能受限于：
1. 离散Hilbert变换的有限长度近似
2. 数值微分的截断误差
3. 低通滤波器的简单设计

## 🔧 参数调整

在 `main-fm.c` 的 `main()` 函数中修改参数：

```c
double fs = 8000.0;        // 采样频率
double duration = 0.1;     // 信号时长(秒)
double fc = 1000.0;        // 载波频率
double fm = 100.0;         // 调制频率
double beta = 5.0;         // 调制指数
int filter_window = 15;    // 低通滤波窗口
```

### 参数建议

- **高精度解调**: 增加采样频率 (fs > 10×fc)
- **窄带FM**: β < 1
- **宽带FM**: β > 1  
- **滤波器**: 窗口大小约为 fs/(2×fm)

## 📈 改进方向

1. **使用FFT实现Hilbert变换** - 提高精度和效率
2. **PLL锁相环解调** - 更稳定的解调方法
3. **自适应滤波** - 根据信噪比调整滤波参数
4. **FIR/IIR滤波器** - 替代简单移动平均
5. **噪声注入测试** - 评估抗噪声性能

## 🔬 应用场景

- **通信系统仿真**: FM广播、无线电通信
- **教学演示**: 调制解调原理
- **信号处理研究**: 瞬时频率分析
- **测试信号生成**: 系统测试和验证

## 📚 相关文档

- [DTMF信号](README-DTMF.md) - 双音多频信号生成
- [2D FFT](README-FFT2D.md) - 二维傅里叶变换
- [K空间重建](README-KSPACE.md) - MRI图像重建

## 🎓 参考资料

- Oppenheim & Schafer, "Discrete-Time Signal Processing"
- Proakis & Manolakis, "Digital Signal Processing"
- Carson规则: J. R. Carson (1922), Bell System Technical Journal

## 📝 示例输出

```
=== 调频(FM)信号生成与解调系统 ===

采样设置:
  采样频率 fs = 8000 Hz
  信号时长 = 0.100 s
  采样点数 = 800

调频信号参数:
  载波频率 fc = 1000.00 Hz
  调制频率 fm = 100.00 Hz
  调制指数 β = 5.00
  最大频偏 Δf = 500.00 Hz
  带宽估计 (Carson规则) ≈ 1200.00 Hz

解调性能:
  均方误差 MSE = 233363.93
  信噪比 SNR = -2.71 dB
```

## ⚖️ 许可证

MIT License - 详见项目根目录LICENSE文件
