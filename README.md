# DTMF 双音多频信号生成器与分析器

这是一个用 C 语言编写的 DTMF（双音多频）信号生成器和分析器，可以生成、播放和识别标准电话按键音。

## 功能特性

- ✨ 生成标准 DTMF 信号（双音频）
- 🔊 实时播放音频到指定设备
- 🎯 使用 DFT（离散傅里叶变换）进行频谱分析
- 🔍 自动识别 DTMF 按键
- 📱 支持所有标准按键：0-9、*、#
- 🎵 支持多按键序列播放（模拟电话拨号）
- 📊 显示频谱分析结果

## DTMF 频率表

```
        1209Hz  1336Hz  1477Hz
 697Hz    1       2       3
 770Hz    4       5       6
 852Hz    7       8       9
 941Hz    *       0       #
```

每个按键由一个低频分量和一个高频分量组成。

## 系统要求

- Linux 操作系统
- GCC 编译器
- ALSA 音频工具（aplay）
- 数学库（libm）

### 安装依赖

```bash
# Ubuntu/Debian
sudo apt-get install gcc alsa-utils

# Fedora/RHEL
sudo dnf install gcc alsa-utils

# Arch Linux
sudo pacman -S gcc alsa-utils
```

## 编译

使用 Makefile 编译：

```bash
make
```

或手动编译：

```bash
gcc main-dtmf.c -lm -o dtmf
```

## 使用方法

### 基本用法

```bash
# 播放单个按键
./dtmf 5

# 播放按键序列
./dtmf 123

# 播放所有数字
./dtmf 0123456789

# 包含特殊字符（需要引号）
./dtmf "*123#"

# 不提供参数（播放默认按键 '1'）
./dtmf
```

### 示例输出

```
播放DTMF音到设备: plughw:1,0
按键序列: 123

[1/3] 播放按键 '1' (697 Hz + 1209 Hz)... 完成 [识别: '1' ✓]
[2/3] 播放按键 '2' (697 Hz + 1336 Hz)... 完成 [识别: '2' ✓]
[3/3] 播放按键 '3' (697 Hz + 1477 Hz)... 完成 [识别: '3' ✓]

播放完成！

DTMF标准频率表：
        1209Hz  1336Hz  1477Hz
 697Hz    1       2       3
 770Hz    4       5       6
 852Hz    7       8       9
 941Hz    *       0       #
```

## 音频设备配置

默认使用 `plughw:1,0` 设备。可以修改 `main-dtmf.c` 中的 `audio_device` 变量：

```c
const char* audio_device = "plughw:1,0";  // Jabra 耳机
// const char* audio_device = "hw:0,0";   // 内置扬声器
// const char* audio_device = NULL;       // 默认设备
```

### 查看可用音频设备

```bash
aplay -l
```

## 技术细节

### 信号参数

- **采样频率**: 8000 Hz（电话标准）
- **信号时长**: 500 ms（每个按键）
- **按键间隔**: 100 ms
- **信号格式**: 16位 PCM, 单声道

### 核心功能

1. **信号生成**: 生成双音频正弦波信号
   ```
   x[n] = sin(2π*f_low*n/fs) + sin(2π*f_high*n/fs)
   ```

2. **DFT 分析**: 离散傅里叶变换用于频谱分析
   ```
   X[k] = Σ x[n] * e^(-j2πkn/N)
   ```

3. **频谱识别**: 检测低频和高频分量，识别按键

### 文件结构

- `main-dtmf.c` - 主程序源代码
- `Makefile` - 编译配置文件
- `README.md` - 项目文档

## API 函数

### 信号处理

- `calculate_dft()` - 计算离散傅里叶变换
- `calculate_spectrum_and_phase()` - 计算幅度谱和相位谱
- `detect_dtmf()` - DTMF 信号识别

### 音频处理

- `double_to_pcm16()` - 浮点数转 PCM16 格式
- `play_audio()` - 播放音频数据

### DTMF 处理

- `get_dtmf_frequencies()` - 获取按键对应的频率

## 清理

```bash
make clean
```

## 应用场景

- 📞 电话系统测试
- 🔬 信号处理教学
- 🎓 数字信号处理（DSP）实验
- 🧪 音频设备测试
- 📡 通信系统开发

## 许可证

本项目仅供学习和研究使用。

## 作者

DTMF Signal Generator & Analyzer

## 参考资料

- [DTMF - Wikipedia](https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling)
- [ITU-T Recommendation Q.23](https://www.itu.int/rec/T-REC-Q.23)

## 故障排除

### 没有声音输出

1. 检查音频设备是否正确：`aplay -l`
2. 确保 ALSA 已安装：`which aplay`
3. 测试音频设备：`speaker-test -c 2`

### 编译错误

1. 确保已安装 GCC：`gcc --version`
2. 确保链接了数学库：`-lm`

### 识别错误

识别算法基于幅度阈值（默认 5.0），可以在 `detect_dtmf()` 函数中调整 `threshold` 值。
