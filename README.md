# FFT-C: 信号处理与图像处理工具集

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-C99-orange.svg)](https://en.wikipedia.org/wiki/C99)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](Makefile)

一个用纯C语言实现的信号处理和图像处理工具集，包含DTMF信号生成器、2D傅里叶变换和K空间图像重建功能。

## 📚 目录

- [功能特性](#功能特性)
- [项目结构](#项目结构)
- [快速开始](#快速开始)
- [模块详解](#模块详解)
  - [DTMF信号生成器](#1-dtmf信号生成器)
  - [2D FFT图像处理](#2-2d-fft图像处理)
  - [K空间图像重建](#3-k空间图像重建)
- [编译与运行](#编译与运行)
- [使用示例](#使用示例)
- [技术细节](#技术细节)
- [测试与验证](#测试与验证)
- [文档索引](#文档索引)
- [系统要求](#系统要求)
- [贡献指南](#贡献指南)

---

## 🎯 功能特性

### 核心功能

- ✨ **DTMF信号生成与识别** - 模拟电话拨号音的生成、播放和频谱识别
- 🖼️ **2D傅里叶变换** - 图像的频域分析和空域重建
- 🔬 **K空间处理** - MRI式K空间数据的保存、加载和图像重建
- 📊 **频谱可视化** - 对数/线性幅度谱的BMP图像输出
- ✅ **高精度验证** - 往返变换误差分析（FFT→IDFT→还原）

### 技术亮点

- 🚀 纯C99实现，无外部依赖（除标准库和libm）
- 🎨 支持BMP图像格式的读写
- 🔧 模块化设计，易于扩展
- 📈 完整的误差统计和精度报告
- 🔄 行列分离法实现2D变换，计算效率高

---

## 📁 项目结构

```
fft-c/
├── 源代码文件 (5个, ~52KB)
│   ├── main-dtmf.c              # DTMF信号生成器主程序
│   ├── main-fft1d.c             # 1D FFT演示程序
│   ├── main-fft2d.c             # 2D FFT主程序
│   ├── kspace_to_image.c        # K空间重建程序 ⭐
│   └── load_kspace_demo.c       # K空间加载示例
│
├── 可执行文件 (编译后生成)
│   ├── dtmf                     # DTMF程序
│   ├── fft2d                    # 2D FFT程序
│   └── kspace_to_image          # K空间重建程序 ⭐
│
├── 文档文件 (8个, ~37KB)
│   ├── README.md                         # 本文件 - 项目主文档
│   ├── README-DTMF.md                    # DTMF详细文档
│   ├── README-FFT2D.md                   # 2D FFT详细文档
│   ├── README-KSPACE.md                  # K空间数据格式说明
│   ├── README-KSPACE-RECONSTRUCTION.md   # K空间重建指南
│   ├── KSPACE-RECONSTRUCTION-SUMMARY.md  # 功能总结
│   ├── QUICKREF-KSPACE.md                # 快速参考手册
│   ├── QUICKSTART.md                     # 快速入门指南
│   ├── PROJECT_SNAPSHOT.md               # 项目状态快照
│   ├── DOC_INDEX.md                      # 文档索引
│   ├── GIT_GUIDE.md                      # Git使用指南
│   └── SESSION_HISTORY.md                # 开发历史记录
│
├── 构建与测试
│   ├── Makefile                          # 构建脚本
│   ├── test_kspace_reconstruction.sh     # K空间重建自动测试
│   └── .gitignore                        # Git忽略列表
│
└── 生成数据 (运行后产生)
    ├── original_image.bmp                # 原始测试图像
    ├── magnitude_spectrum.bmp            # FFT幅度谱（对数）
    ├── magnitude_spectrum_linear.bmp     # FFT幅度谱（线性）
    ├── restored_image.bmp                # FFT还原图像
    ├── kspace_magnitude_spectrum.bmp     # K空间幅度谱
    ├── reconstructed_image.bmp           # K空间重建图像
    ├── kspace_data.bin                   # K空间二进制数据(1.1MB)
    └── kspace_data.txt                   # K空间文本数据(4.7MB)
```

---

## 🚀 快速开始

### 系统要求

- **操作系统**: Linux (Ubuntu/Debian/Fedora/Arch等)
- **编译器**: GCC (支持C99标准)
- **依赖库**: libm (数学库), ALSA工具 (用于DTMF音频播放)

### 安装依赖

```bash
# Ubuntu/Debian
sudo apt-get install gcc make alsa-utils

# Fedora/RHEL
sudo dnf install gcc make alsa-utils

# Arch Linux
sudo pacman -S gcc make alsa-utils
```

### 编译所有程序

```bash
# 克隆或下载项目后，进入项目目录
cd fft-c

# 编译所有程序
make all

# 或单独编译
make dtmf              # 编译DTMF程序
make fft2d             # 编译2D FFT程序
make kspace_to_image   # 编译K空间重建程序
```

### 快速测试

```bash
# 1. 测试DTMF信号生成
./dtmf 123

# 2. 测试2D FFT图像处理
./fft2d

# 3. 测试K空间图像重建
./kspace_to_image

# 4. 运行完整自动化测试
./test_kspace_reconstruction.sh
```

---

## 📦 模块详解

### 1. DTMF信号生成器

DTMF (Dual-Tone Multi-Frequency) 双音多频信号生成器，用于模拟电话按键音。

#### 功能特性

- 生成标准DTMF信号（双音频组合）
- 实时播放音频到ALSA设备
- 使用DFT进行频谱分析和按键识别
- 支持所有标准按键：0-9、*、#
- 支持多按键序列播放

#### DTMF频率表

```
        1209Hz  1336Hz  1477Hz
 697Hz    1       2       3
 770Hz    4       5       6
 852Hz    7       8       9
 941Hz    *       0       #
```

#### 使用示例

```bash
# 播放单个按键
./dtmf 5

# 播放电话号码
./dtmf 13800138000

# 播放特殊字符（需要引号）
./dtmf "*123#"

# 模拟拨打电话
./dtmf 02112345678
```

#### 输出示例

```
播放DTMF音到设备: plughw:1,0
按键序列: 123

[1/3] 播放按键 '1' (697 Hz + 1209 Hz)... 完成 [识别: '1' ✓]
[2/3] 播放按键 '2' (697 Hz + 1336 Hz)... 完成 [识别: '2' ✓]
[3/3] 播放按键 '3' (697 Hz + 1477 Hz)... 完成 [识别: '3' ✓]

播放完成！
```

📖 详细文档: [README-DTMF.md](README-DTMF.md)

---

### 2. 2D FFT图像处理

实现二维离散傅里叶变换(2D DFT)和逆变换(2D IDFT)，用于图像的频域分析。

#### 功能特性

- 2D DFT实现（行列分离法）
- 2D IDFT实现（完美还原图像）
- FFTShift功能（频谱中心化）
- BMP图像格式输出
- 对数/线性幅度谱可视化
- 多种测试图像模式（圆形、正弦波、方形）
- 自动精度验证

#### 生成的图像

1. **original_image.bmp** - 原始测试图像 (256×256)
2. **magnitude_spectrum.bmp** - 幅度谱（对数尺度）
3. **magnitude_spectrum_linear.bmp** - 幅度谱（线性尺度）
4. **restored_image.bmp** - 逆变换还原图像

#### 使用示例

```bash
# 运行2D FFT程序
./fft2d

# 查看生成的图像
xdg-open original_image.bmp
xdg-open magnitude_spectrum.bmp
xdg-open restored_image.bmp
```

#### 测试模式

程序支持三种测试图像模式（可在源码中切换）：

- **模式1**: 圆形图案（默认）- 经典频域分析测试
- **模式2**: 多频率正弦波叠加
- **模式3**: 方形图案（展示Gibbs现象）

#### 精度验证输出

```
=================================================
  2D IDFT 还原图像验证
=================================================
虚部分析 (理论上应接近0):
  最大虚部: 3.552714e-14
  平均虚部: 2.210445e-15

还原图像统计:
  最小值: -4.440892e-15
  最大值: 255.000000
  平均值: 78.049316

误差分析:
  最大误差: 4.263256e-14
  平均误差: 5.063971e-15
  均方根误差: 1.043478e-14

✅ 还原精度极高！最大误差在浮点数精度范围内
```

📖 详细文档: [README-FFT2D.md](README-FFT2D.md)

---

### 3. K空间图像重建

从保存的K空间（频域）数据重建原始图像，模拟MRI成像过程。

#### 功能特性

- K空间数据加载（二进制格式）
- 2D逆傅里叶变换（IDFT）
- K空间幅度谱可视化
- 图像还原与导出
- 精度验证（虚部残差分析）

#### 工作流程

```
┌─────────────────┐
│  fft2d 程序     │  生成测试图像
│                 │  执行2D FFT
│                 │  保存K空间数据
└────────┬────────┘
         │
         ▼
  kspace_data.bin (1.1MB)
         │
         ▼
┌─────────────────┐
│ kspace_to_image │  加载K空间数据
│                 │  执行2D IDFT
│                 │  还原图像
└────────┬────────┘
         │
         ▼
  reconstructed_image.bmp (193KB)
```

#### 使用示例

```bash
# 步骤1: 生成K空间数据
./fft2d

# 步骤2: 从K空间还原图像
./kspace_to_image kspace_data.bin

# 步骤3: 比较原始图像和还原图像
xdg-open original_image.bmp
xdg-open reconstructed_image.bmp

# 自动化测试（包含完整流程）
./test_kspace_reconstruction.sh
```

#### K空间数据格式

**二进制格式** (`kspace_data.bin`):

```c
struct {
    int width;      // 图像宽度
    int height;     // 图像高度
    double real[height][width];  // 实部数组
    double imag[height][width];  // 虚部数组
} kspace_data;
```

文件大小: 8 + width × height × 2 × sizeof(double) 字节

**文本格式** (`kspace_data.txt`):

```
width height
real[0][0] imag[0][0]
real[0][1] imag[0][1]
...
```

#### 还原统计示例

```
=================================================
  图像还原统计
=================================================
虚部分析 (理论上应接近0):
  最大虚部: 7.099876e-14
  平均虚部: 4.420890e-15

还原图像统计:
  最小值: 0.000000
  最大值: 255.000000
  平均值: 78.049316

✅ K空间数据成功还原为图像！
```

📖 详细文档: 
- [README-KSPACE-RECONSTRUCTION.md](README-KSPACE-RECONSTRUCTION.md)
- [KSPACE-RECONSTRUCTION-SUMMARY.md](KSPACE-RECONSTRUCTION-SUMMARY.md)
- [QUICKREF-KSPACE.md](QUICKREF-KSPACE.md)

---

## 🔧 编译与运行

### Makefile 命令

```bash
# 编译所有程序
make all

# 单独编译
make dtmf              # 编译DTMF程序
make fft2d             # 编译2D FFT程序
make kspace_to_image   # 编译K空间重建程序

# 清理编译文件
make clean

# 运行DTMF测试
make test

# 显示帮助信息
make help

# 安装到系统（可选，需要sudo）
make install

# 从系统卸载
make uninstall
```

### 手动编译

```bash
# DTMF信号生成器
gcc -Wall -Wextra -O2 -std=c99 -o dtmf main-dtmf.c -lm

# 2D FFT程序
gcc -Wall -Wextra -O2 -std=c99 -o fft2d main-fft2d.c -lm

# K空间重建程序
gcc -Wall -Wextra -O2 -std=c99 -o kspace_to_image kspace_to_image.c -lm

# 1D FFT演示
gcc -Wall -Wextra -O2 -std=c99 -o fft1d main-fft1d.c -lm
```

---

## 💡 使用示例

### 示例1: 完整的图像处理流程

```bash
# 1. 生成测试图像并进行FFT分析
./fft2d
# 输出: original_image.bmp, magnitude_spectrum.bmp, restored_image.bmp
# 输出: kspace_data.bin, kspace_data.txt

# 2. 从K空间数据还原图像
./kspace_to_image kspace_data.bin
# 输出: reconstructed_image.bmp, kspace_magnitude_spectrum.bmp

# 3. 查看所有生成的图像
ls -lh *.bmp
```

### 示例2: DTMF电话拨号模拟

```bash
# 拨打客服热线
./dtmf 4008123456

# 输入分机号
./dtmf 8888

# 语音菜单选择（1=中文服务，9=人工服务）
./dtmf 1
./dtmf 9
```

### 示例3: 自动化测试

```bash
# 运行K空间重建完整测试
./test_kspace_reconstruction.sh

# 输出:
# ✓ 编译程序
# ✓ 生成K空间数据
# ✓ 验证K空间文件
# ✓ 重建图像
# ✓ 验证输出图像
# ✓ 测试完成！
```

---

## 🔬 技术细节

### 算法实现

#### 1D DFT (离散傅里叶变换)

```c
X[k] = Σ(n=0 to N-1) x[n] * e^(-j*2π*k*n/N)
```

- 时间复杂度: O(N²)
- 用于DTMF频谱分析和2D变换的基础

#### 2D DFT (行列分离法)

```c
F[u][v] = Σ(x=0 to M-1) Σ(y=0 to N-1) f[x][y] * e^(-j*2π*(ux/M + vy/N))
```

实现步骤:
1. 对每一行执行1D DFT
2. 对每一列执行1D DFT

- 时间复杂度: O(M²N + MN²) ≈ O(N³) for M=N
- 比直接2D DFT更高效

#### FFTShift (频谱中心化)

将零频率分量移到频谱中心，便于可视化：

```
原始频谱:        FFTShift后:
┌──┬──┐          ┌──┬──┐
│ A│ B│          │ D│ C│
├──┼──┤    →     ├──┼──┤
│ C│ D│          │ B│ A│
└──┴──┘          └──┴──┘
```

### BMP图像格式

生成24位真彩色BMP文件（灰度图像的RGB值相同）：

- 文件头: 14字节
- 信息头: 40字节
- 像素数据: width × height × 3 字节（BGR顺序）
- 每行对齐到4字节边界

### K空间数据

K空间是图像在频域的表示，包含：

- **实部**: 余弦分量
- **虚部**: 正弦分量
- **幅度**: sqrt(实部² + 虚部²)
- **相位**: arctan(虚部/实部)

---

## ✅ 测试与验证

### 精度测试

所有变换均经过往返精度测试：

1. **FFT→IDFT往返测试**
   - 原始图像 → FFT → IDFT → 还原图像
   - 最大误差: ~10⁻¹⁴ (浮点数精度极限)

2. **K空间往返测试**
   - 原始图像 → FFT → 保存 → 加载 → IDFT → 还原图像
   - 最大误差: ~10⁻¹⁴

3. **虚部残差测试**
   - 实数图像经IDFT后虚部应接近0
   - 平均虚部: ~10⁻¹⁵

### 自动化测试脚本

`test_kspace_reconstruction.sh` 提供完整的自动化测试流程：

```bash
#!/bin/bash
# 测试流程:
# 1. 清理旧文件
# 2. 编译程序
# 3. 生成K空间数据
# 4. 验证K空间文件
# 5. 重建图像
# 6. 验证输出图像
# 7. 显示统计信息
```

---

## 📖 文档索引

- **[DOC_INDEX.md](DOC_INDEX.md)** - 完整文档索引
- **[QUICKSTART.md](QUICKSTART.md)** - 快速入门指南
- **[README-DTMF.md](README-DTMF.md)** - DTMF详细文档
- **[README-FFT2D.md](README-FFT2D.md)** - 2D FFT详细文档
- **[README-KSPACE.md](README-KSPACE.md)** - K空间数据格式
- **[README-KSPACE-RECONSTRUCTION.md](README-KSPACE-RECONSTRUCTION.md)** - K空间重建指南
- **[KSPACE-RECONSTRUCTION-SUMMARY.md](KSPACE-RECONSTRUCTION-SUMMARY.md)** - 功能总结
- **[QUICKREF-KSPACE.md](QUICKREF-KSPACE.md)** - 快速参考手册
- **[PROJECT_SNAPSHOT.md](PROJECT_SNAPSHOT.md)** - 项目状态快照
- **[GIT_GUIDE.md](GIT_GUIDE.md)** - Git使用指南
- **[SESSION_HISTORY.md](SESSION_HISTORY.md)** - 开发历史记录

---

## 💻 系统要求

### 最低要求

- **CPU**: 任何支持浮点运算的现代CPU
- **内存**: 至少64MB可用RAM
- **磁盘**: 至少20MB可用空间

### 推荐配置

- **CPU**: 多核处理器（提升编译速度）
- **内存**: 512MB以上
- **磁盘**: 100MB以上（包含生成的测试数据）

### 软件依赖

| 组件 | 版本要求 | 用途 |
|------|---------|------|
| GCC | 4.8+ | C编译器 |
| GNU Make | 3.81+ | 构建工具 |
| ALSA Utils | 1.0+ | DTMF音频播放 |
| libm | - | 数学函数库 |

---

## 📊 项目统计

- **源代码行数**: ~1,560行
- **文档字数**: ~15,000字
- **源代码大小**: ~52KB
- **文档大小**: ~37KB
- **可执行文件**: ~59KB (编译后)
- **测试数据**: ~6.9MB (生成后)

---

## 🤝 贡献指南

欢迎贡献代码、报告bug或提出功能建议！

### 如何贡献

1. Fork本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

### 代码规范

- 遵循C99标准
- 使用4空格缩进
- 添加必要的注释
- 保持函数简洁明了
- 提供测试用例

---

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

---

## 🙏 致谢

- FFT算法基于Cooley-Tukey算法思想
- BMP文件格式参考Microsoft BMP规范
- DTMF标准参考ITU-T Q.23建议书

---

## 📞 联系方式

- **项目主页**: [GitHub Repository](https://github.com/yourusername/fft-c)
- **问题反馈**: [Issue Tracker](https://github.com/yourusername/fft-c/issues)
- **讨论交流**: [Discussions](https://github.com/yourusername/fft-c/discussions)

---

## 🔖 版本历史

### v2.0 (2025-11-05)
- ✨ 新增K空间图像重建功能
- ✨ 新增自动化测试脚本
- 📝 完善项目文档
- 🐛 修复精度问题

### v1.0 (初始版本)
- ✨ 实现DTMF信号生成器
- ✨ 实现2D FFT/IDFT
- ✨ 实现BMP图像输出
- 📝 添加基础文档

---

## 📝 常见问题 (FAQ)

### Q1: 编译时出现 "undefined reference to 'sin'" 错误？

**A**: 确保链接了数学库 `-lm`：
```bash
gcc -o program source.c -lm
```

### Q2: DTMF程序没有声音输出？

**A**: 检查ALSA设备配置，或在源码中修改音频设备：
```c
#define AUDIO_DEVICE "default"  // 改为你的设备
```

### Q3: 生成的BMP图像无法打开？

**A**: 确保使用支持BMP格式的图像查看器，或转换为PNG：
```bash
convert image.bmp image.png
```

### Q4: K空间重建后图像不完整？

**A**: 检查K空间数据文件是否损坏：
```bash
ls -lh kspace_data.bin  # 应约1.1MB
```

### Q5: 如何修改图像分辨率？

**A**: 在源码中修改IMAGE_SIZE常量：
```c
#define IMAGE_SIZE 256  // 改为你需要的尺寸（必须是2的幂）
```

---

**⭐ 如果这个项目对你有帮助，请给个Star！**

**📢 欢迎分享和使用！**
