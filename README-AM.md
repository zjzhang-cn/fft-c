# AM调幅信号生成与解调系统

## 概述

本项目实现了幅度调制(AM)信号的生成和三种不同的解调方法：
1. **包络检波法**（非相干解调）
2. **Hilbert变换法**（包络检测的改进方法）
3. **相干解调法**（同步检波）

## AM调制原理

### 基本公式

AM信号的数学表达式：

$$s_{AM}(t) = [A + m(t)] \cdot \cos(2\pi f_c t)$$

或归一化形式：

$$s_{AM}(t) = [1 + \mu \cdot m(t)] \cdot \cos(2\pi f_c t)$$

其中：
- $A$ 或 $1$ 是载波幅度
- $m(t)$ 是调制信号（基带信号）
- $f_c$ 是载波频率
- $\mu$ 是调制指数（调制度），范围 0 到 1

### 调制指数

调制指数 $\mu$ 定义为：

$$\mu = \frac{A_{max} - A_{min}}{A_{max} + A_{min}}$$

- $\mu < 1$：正常调制
- $\mu = 1$：100% 调制（最大幅度利用）
- $\mu > 1$：过调制（会产生失真）

### AM信号特性

1. **频谱结构**：
   - 载波分量：$f_c$
   - 上边带：$f_c + f_m$
   - 下边带：$f_c - f_m$

2. **带宽**：
   $$B = 2f_{m(max)}$$
   
3. **功率分配**（调制指数为 $\mu$）：
   - 载波功率：$\frac{P_t}{1 + \mu^2/2}$
   - 边带功率：$\frac{\mu^2 P_t}{2(1 + \mu^2/2)}$

## AM解调方法

### 1. 包络检波法

**原理**：提取AM信号的包络（幅度变化）

**步骤**：
1. 全波整流：$|s_{AM}(t)|$
2. 低通滤波：去除载波频率成分
3. 去直流：减去平均值

**优点**：
- 实现简单
- 不需要载波同步
- 成本低

**缺点**：
- 对噪声敏感
- 不能用于抑制载波的DSB-SC信号

### 2. Hilbert变换包络检波

**原理**：使用解析信号计算包络

解析信号：
$$s_a(t) = s(t) + j\hat{s}(t)$$

其中 $\hat{s}(t)$ 是 $s(t)$ 的Hilbert变换。

包络：
$$A(t) = |s_a(t)| = \sqrt{s^2(t) + \hat{s}^2(t)}$$

**优点**：
- 更准确的包络提取
- 对噪声有一定抑制

**缺点**：
- 计算复杂度较高
- 需要数值计算Hilbert变换

### 3. 相干解调（同步检波）

**原理**：使用本地载波与接收信号相乘，然后低通滤波

**步骤**：
1. 生成本地载波：$c(t) = 2\cos(2\pi f_c t + \phi)$
2. 混频：$r(t) = s_{AM}(t) \cdot c(t)$
3. 低通滤波：去除 $2f_c$ 成分

**数学推导**：
$$r(t) = [1 + \mu m(t)] \cos(2\pi f_c t) \cdot 2\cos(2\pi f_c t + \phi)$$
$$= [1 + \mu m(t)][\cos(\phi) + \cos(4\pi f_c t + \phi)]$$

低通滤波后：
$$m_{out}(t) = \mu m(t) \cos(\phi)$$

**优点**：
- 最佳解调性能
- 抗噪声能力强
- 可用于DSB-SC等抑制载波调制

**缺点**：
- 需要载波同步（频率和相位）
- 实现复杂度高
- 相位误差会降低输出幅度

## 编译和使用

### 编译

```bash
gcc -o am_signal main-am.c -lm
```

或使用 Makefile：

```bash
make am
```

### 运行

基本使用（使用默认参数）：

```bash
./am_signal
```

自定义参数：

```bash
./am_signal -fc 20000 -fm 2000 -fs 200000 -d 0.02 -m 0.5
```

### 命令行参数

- `-fc <Hz>`：载波频率（默认：10000 Hz）
- `-fm <Hz>`：调制信号频率（默认：1000 Hz）
- `-fs <Hz>`：采样频率（默认：100000 Hz）
- `-d <秒>`：信号持续时间（默认：0.01 秒）
- `-m <0-1>`：调制指数（默认：0.8）
- `-h`：显示帮助信息

### 示例

1. **生成标准AM信号**（调制度80%）：
```bash
./am_signal -fc 10000 -fm 1000 -m 0.8
```

2. **测试过调制情况**（调制度120%）：
```bash
./am_signal -fc 10000 -fm 1000 -m 1.2
```

3. **高频载波测试**：
```bash
./am_signal -fc 100000 -fm 5000 -fs 500000 -d 0.001
```

## 输出文件

程序运行后会生成以下文件：

1. **am_signal.txt**：AM信号的时域数据（文本格式）
2. **am_signal.csv**：包含调制信号、载波和AM信号的CSV文件
3. **am_demodulated.csv**：包含原始调制信号和三种解调结果的对比

### CSV文件格式

**am_signal.csv**：
```
Time,Modulating,Carrier,AM_Signal,AM_Signal
0.000000,1.000000,1.000000,1.800000,1.800000
...
```

**am_demodulated.csv**：
```
Time,Original,Envelope,Hilbert,Coherent
0.000000,1.000000,0.985000,0.990000,0.995000
...
```

## 可视化

可以使用Python、MATLAB或其他工具绘制生成的数据：

### Python示例

```python
import pandas as pd
import matplotlib.pyplot as plt

# 读取数据
am_data = pd.read_csv('am_signal.csv')
demod_data = pd.read_csv('am_demodulated.csv')

# 绘制AM调制过程
fig, axes = plt.subplots(3, 1, figsize=(12, 8))

axes[0].plot(am_data['Time'], am_data['Modulating'])
axes[0].set_title('调制信号 m(t)')
axes[0].set_ylabel('幅度')
axes[0].grid(True)

axes[1].plot(am_data['Time'], am_data['Carrier'])
axes[1].set_title('载波信号 c(t)')
axes[1].set_ylabel('幅度')
axes[1].grid(True)

axes[2].plot(am_data['Time'], am_data['AM_Signal'])
axes[2].set_title('AM调制信号 s(t)')
axes[2].set_xlabel('时间 (秒)')
axes[2].set_ylabel('幅度')
axes[2].grid(True)

plt.tight_layout()
plt.savefig('am_modulation.png', dpi=300)
plt.show()

# 绘制解调结果对比
plt.figure(figsize=(12, 6))
plt.plot(demod_data['Time'], demod_data['Original'], 'k-', 
         label='原始信号', linewidth=2)
plt.plot(demod_data['Time'], demod_data['Envelope'], 'r--', 
         label='包络检波', alpha=0.7)
plt.plot(demod_data['Time'], demod_data['Hilbert'], 'g--', 
         label='Hilbert变换', alpha=0.7)
plt.plot(demod_data['Time'], demod_data['Coherent'], 'b--', 
         label='相干解调', alpha=0.7)

plt.title('AM解调结果对比')
plt.xlabel('时间 (秒)')
plt.ylabel('幅度')
plt.legend()
plt.grid(True)
plt.savefig('am_demodulation.png', dpi=300)
plt.show()
```

## 性能指标

程序会自动计算并显示三种解调方法的信噪比（SNR）：

```
解调性能对比:
  包络检波法 SNR = 25.43 dB
  Hilbert变换法 SNR = 28.76 dB
  相干解调法 SNR = 35.21 dB
```

**SNR定义**：
$$SNR = 10 \log_{10} \frac{P_{signal}}{P_{noise}}$$

其中噪声功率为原始信号与解调信号之间的误差功率。

## 应用场景

1. **中波广播（AM广播）**：
   - 载波频率：535-1605 kHz
   - 音频带宽：5 kHz
   
2. **航空通信**：
   - VHF频段：118-137 MHz
   - 语音通信

3. **业余无线电**：
   - 多个频段（HF、VHF等）

4. **教学演示**：
   - 调制解调基本原理
   - 信号处理实验

## 理论背景

### 为什么需要调制？

1. **天线尺寸**：有效辐射需要天线长度约为波长的1/4
   - 音频1kHz：波长300km，天线需75km（不现实）
   - 载波1MHz：波长300m，天线需75m（可行）

2. **频谱利用**：多个信号可以调制到不同载波频率，共享传输介质

3. **抗干扰**：调制可以提高抗噪声能力

### AM的优缺点

**优点**：
- 实现简单
- 解调器成本低（包络检波）
- 技术成熟

**缺点**：
- 功率效率低（载波不携带信息但占用大部分功率）
- 抗噪声能力差
- 带宽利用率低

### 改进的AM调制

1. **DSB-SC**（双边带抑制载波）：去除载波，提高功率效率
2. **SSB**（单边带）：只传输一个边带，带宽减半
3. **VSB**（残留边带）：介于DSB和SSB之间

## 参考资料

1. B.P. Lathi, "Modern Digital and Analog Communication Systems"
2. S. Haykin, "Communication Systems"
3. A.V. Oppenheim, "Signals and Systems"

## 扩展练习

1. 添加噪声到AM信号，观察不同解调方法的抗噪声性能
2. 实现DSB-SC调制和解调
3. 实现科斯塔斯环（Costas Loop）进行载波恢复
4. 使用FFT分析AM信号的频谱
5. 实现自动增益控制（AGC）

## 许可证

MIT License
