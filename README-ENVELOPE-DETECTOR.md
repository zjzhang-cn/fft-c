# 包络检波电路模拟器

## 概述

本程序实现了真实的包络检波电路（二极管 + RC 低通滤波器）的 C 语言模拟，用于 AM 信号解调。

## 电路原理

### 电路结构

```
AM信号输入 → 二极管 → RC滤波器 → 解调信号输出
                      ┌─R─┐
                      │   │
                      C   负载
                      │   │
                      └───┘
```

### 工作原理

1. **二极管整流**
   - 二极管只允许正向电流通过
   - 将 AM 信号整流为单向脉动信号
   - 使用肖克利方程或简化分段线性模型

2. **RC 低通滤波**
   - 电容充电：信号高电平时，通过电阻对电容充电
   - 电容放电：信号低电平时，电容通过负载放电
   - 滤除载波频率，保留包络信号

3. **直流分量去除**
   - 消除检波后的直流偏置
   - 恢复原始调制信号

## 数学模型

### 二极管模型

**指数模型（肖克利方程）**：
$$I_D = I_S \left( e^{\frac{V_D}{nV_T}} - 1 \right)$$

其中：
- $I_S$：反向饱和电流（约 $10^{-12}$ A）
- $n$：理想因子（1-2）
- $V_T$：热电压（室温下约 26 mV）

**简化模型（分段线性）**：
$$V_{out} = \begin{cases}
V_{in} - V_D & \text{if } V_{in} > V_D \\
0 & \text{if } V_{in} \leq V_D
\end{cases}$$

其中 $V_D$ 是二极管导通电压（约 0.6-0.7 V）

### RC 滤波器

**微分方程**：
$$RC \frac{dV_{out}}{dt} = V_{in} - V_{out}$$

**离散化（欧拉法）**：
$$V_{out}[n] = \alpha V_{in}[n] + (1-\alpha) V_{out}[n-1]$$

其中：
$$\alpha = \frac{\Delta t}{RC + \Delta t}$$

**截止频率**：
$$f_c = \frac{1}{2\pi RC}$$

## RC 参数设计准则

### 设计要求

1. **滤除载波频率**：
   $$f_c \ll f_{carrier}$$
   通常要求 $f_c < \frac{f_{carrier}}{20}$

2. **保留调制信号**：
   $$f_c > f_{modulating}$$
   通常要求 $f_c \approx 5-10 \times f_{modulating}$

3. **平衡考虑**：
   - RC 太小：滤波不足，载波残留
   - RC 太大：响应慢，信号失真

### 典型参数

| 载波频率 | 调制频率 | 推荐 R | 推荐 C | 截止频率 |
|---------|---------|--------|--------|---------|
| 10 kHz  | 500 Hz  | 47 kΩ  | 0.47 μF | ~7.2 Hz |
| 100 kHz | 5 kHz   | 10 kΩ  | 0.47 μF | ~34 Hz |
| 1 MHz   | 10 kHz  | 4.7 kΩ | 0.1 μF | ~339 Hz |

## 编译和运行

### 编译

```bash
gcc -o envelope_detector envelope_detector.c -lm -O2
```

### 运行

```bash
./envelope_detector
```

### 输出

程序会生成：
- `envelope_detector_result.csv`：包含 AM 信号和解调信号的数据

### 可视化

```bash
# 绘制包络检波结果
python3 plot_envelope_detector.py

# 绘制 RC 滤波器频率响应
python3 plot_envelope_detector.py --rc

# 绘制二极管特性曲线
python3 plot_envelope_detector.py --diode

# 绘制所有图表
python3 plot_envelope_detector.py --all
```

## 程序特性

### 实现的功能

1. **AM 信号生成**
   - 可配置载波频率、调制频率、调制指数
   - 支持单频和多频调制信号

2. **二极管整流**
   - 指数模型（高精度）
   - 分段线性模型（高效率）

3. **RC 滤波器**
   - 欧拉法（一阶精度）
   - 梯形法（二阶精度）

4. **自动参数优化**
   - 根据载波和调制频率自动计算最佳 RC 值
   - 选择标准元件值

5. **性能分析**
   - 计算解调误差（RMSE）
   - 参数影响测试

### 代码结构

```c
// 二极管模型
double diode_current(double v_in, ...);     // 指数模型
double diode_rectifier(double v_in, ...);   // 简化模型

// RC 滤波器
void rc_lowpass_filter(...);                 // 欧拉法
void rc_lowpass_filter_trapezoidal(...);     // 梯形法

// 包络检波器
void envelope_detector(...);                 // 基本版本
void envelope_detector_improved(...);        // 改进版本

// 参数计算
void calculate_optimal_rc(...);              // 最佳参数
```

## 示例结果

### 标准测试案例

**参数**：
- 载波频率：10 kHz
- 调制频率：500 Hz
- 调制指数：0.8
- R = 47 kΩ, C = 0.47 μF
- 二极管压降：0.7 V

**预期结果**：
- 截止频率：约 7.2 Hz
- 时间常数：约 22 ms
- 解调 RMSE < 0.1

## 与理想包络检波的对比

| 特性 | 理想模型 | 真实电路模拟 |
|-----|---------|------------|
| 二极管 | 完美整流 | 有压降和非线性 |
| 滤波器 | 理想低通 | RC 一阶滤波器 |
| 响应速度 | 瞬时 | 受 RC 时间常数限制 |
| 失真 | 无 | 有（取决于参数） |
| 计算复杂度 | 低 | 中 |

## 应用场景

1. **通信系统设计**
   - AM 接收机设计
   - 解调电路参数优化

2. **教学演示**
   - 模拟电子电路教学
   - 信号处理实验

3. **电路仿真**
   - 验证理论设计
   - 参数敏感性分析

## 扩展方向

1. **改进二极管模型**
   - 考虑温度影响
   - 添加寄生电容

2. **高阶滤波器**
   - 二阶或多阶 RC 滤波器
   - 有源滤波器设计

3. **非线性效果**
   - 过调制失真
   - 谐波失真分析

4. **噪声模拟**
   - 热噪声
   - 闪烁噪声

## 参考资料

1. **通信原理**
   - 樊昌信，《通信原理》（第7版）
   - B.P. Lathi, "Modern Digital and Analog Communication Systems"

2. **模拟电子技术**
   - 童诗白，《模拟电子技术基础》（第5版）
   - Sedra/Smith, "Microelectronic Circuits"

3. **信号处理**
   - Oppenheim, "Signals and Systems"

## 许可

本程序仅供学习和研究使用。

## 作者

FFT-C 项目组

## 更新日志

- 2025-11-05: 初始版本
  - 实现基本包络检波器
  - 添加参数优化功能
  - 提供可视化工具
