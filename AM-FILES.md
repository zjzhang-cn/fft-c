# AM调幅系统 - 文件清单

本文档列出了AM调幅信号生成与解调系统的所有相关文件。

## 📋 核心文件

### 源代码
- **main-am.c** (约550行)
  - AM调制信号生成
  - 三种解调算法实现
  - 性能评估和文件输出
  - 命令行参数处理

### 可执行文件
- **am_signal** (编译后生成)
  - 主程序可执行文件
  - 编译命令: `gcc -o am_signal main-am.c -lm`

## 📚 文档文件

### 技术文档
1. **README-AM.md** (约800行)
   - 完整的技术文档
   - AM调制/解调原理详解
   - 数学公式推导
   - 频谱分析
   - 应用场景
   - 参考资料

2. **QUICKSTART-AM.md** (约300行)
   - 快速入门指南
   - 5分钟上手教程
   - 常用命令示例
   - 故障排除

3. **AM-SUMMARY.md** (约450行)
   - 项目总结
   - 功能概览
   - 性能指标
   - 扩展建议

### 集成文档
4. **README.md** (已更新)
   - 主项目文档中添加了AM系统章节
   - 包含快速使用示例

## 🔧 脚本文件

### 测试脚本
1. **test_am.sh**
   - 自动化测试脚本
   - 多种参数组合测试
   - 生成对比数据
   - 使用: `./test_am.sh`

2. **demo_am.sh**
   - 快速演示脚本
   - 展示基本功能
   - 使用: `./demo_am.sh`

### 可视化脚本
3. **plot_am.py** (Python)
   - 绘制调制过程
   - 绘制解调结果对比
   - 频谱分析
   - 生成高质量图表
   - 使用: `python3 plot_am.py`

## 📊 输出文件

### 数据文件（运行时生成）
- **am_signal.txt**
  - AM信号时域数据（制表符分隔）
  - 包含时间和信号幅度

- **am_signal.csv**
  - CSV格式的完整数据
  - 列: Time, Modulating, Carrier, AM_Signal

- **am_demodulated.csv**
  - 解调结果对比
  - 列: Time, Original, Envelope, Hilbert, Coherent

### 测试文件（test_am.sh生成）
- **am_signal_m30.csv** - 30%调制度
- **am_signal_m100.csv** - 100%调制度
- **am_signal_m120.csv** - 120%过调制
- **am_demodulated_m30.csv**
- **am_demodulated_m100.csv**
- **am_demodulated_m120.csv**
- **am_signal_hf.csv** - 高频载波测试
- **am_signal_long.csv** - 长时间信号

### 图像文件（plot_am.py生成）
- **am_modulation.png**
  - 调制过程图（调制信号、载波、AM信号）
  
- **am_demodulation.png**
  - 解调结果详细对比图
  
- **am_demod_comparison.png**
  - 所有解调方法总对比
  
- **am_spectrum.png**
  - AM信号频谱（线性和对数尺度）
  
- **am_modulation_index_comparison.png**
  - 不同调制指数对比

## 🔨 构建系统

### Makefile（已更新）
- 添加了 `am_signal` 目标
- 支持 `make am_signal` 命令
- 集成到 `make all` 中
- 清理规则已包含AM文件

## 📁 文件组织

```
fft-c/
├── 源代码
│   └── main-am.c
│
├── 文档
│   ├── README-AM.md
│   ├── QUICKSTART-AM.md
│   ├── AM-SUMMARY.md
│   └── AM-FILES.md (本文件)
│
├── 脚本
│   ├── test_am.sh
│   ├── demo_am.sh
│   └── plot_am.py
│
├── 输出数据 (运行时生成)
│   ├── am_signal.txt
│   ├── am_signal.csv
│   ├── am_demodulated.csv
│   └── am_signal_*.csv (测试数据)
│
└── 图像 (可视化生成)
    ├── am_modulation.png
    ├── am_demodulation.png
    ├── am_demod_comparison.png
    ├── am_spectrum.png
    └── am_modulation_index_comparison.png
```

## 📊 统计信息

### 代码统计
- C源代码: ~550行
- 文档: ~1500行
- Python脚本: ~350行
- Shell脚本: ~150行
- **总计: ~2550行**

### 文件数量
- 源文件: 1个
- 文档文件: 4个
- 脚本文件: 3个
- **核心文件总计: 8个**

### 功能模块
- 调制器: 1个 (支持单频和多频)
- 解调器: 3个 (包络检波、Hilbert变换、相干解调)
- 性能评估: 1个 (SNR计算)
- 文件I/O: 4个函数
- 可视化: 6个图表类型

## 🚀 快速命令参考

### 编译
```bash
make am_signal
# 或
gcc -o am_signal main-am.c -lm
```

### 运行
```bash
# 默认参数
./am_signal

# 自定义参数
./am_signal -fc 20000 -fm 2000 -m 0.5

# 帮助
./am_signal -h
```

### 测试
```bash
# 快速演示
./demo_am.sh

# 完整测试
./test_am.sh
```

### 可视化
```bash
python3 plot_am.py
```

## 📖 文档阅读顺序建议

### 新手入门
1. **QUICKSTART-AM.md** - 快速上手
2. **demo_am.sh** - 运行演示
3. **README-AM.md** - 深入学习

### 进阶使用
1. **AM-SUMMARY.md** - 全面了解
2. **test_am.sh** - 查看测试案例
3. **main-am.c** - 研究源代码

### 项目集成
1. **README.md** - 查看AM在整体项目中的位置
2. **Makefile** - 了解构建流程

## 🎯 学习路径

### Level 1: 基础使用
- [ ] 阅读 QUICKSTART-AM.md
- [ ] 编译并运行程序
- [ ] 查看生成的数据文件

### Level 2: 理解原理
- [ ] 阅读 README-AM.md 的原理部分
- [ ] 运行不同参数的测试
- [ ] 使用 plot_am.py 可视化结果

### Level 3: 深入分析
- [ ] 研究三种解调算法的差异
- [ ] 分析 SNR 性能指标
- [ ] 测试边界情况（过调制等）

### Level 4: 代码研究
- [ ] 阅读 main-am.c 源代码
- [ ] 理解每个函数的实现
- [ ] 尝试修改和扩展功能

### Level 5: 项目扩展
- [ ] 添加噪声模拟
- [ ] 实现 DSB-SC 或 SSB
- [ ] 集成到更大的通信系统

## 🔗 相关文件

### 项目中的其他模块
- DTMF系统: main-dtmf.c, README-DTMF.md
- FM系统: main-fm.c, README-FM.md
- FFT系统: main-fft2d.c, README-FFT2D.md
- K空间: kspace_to_image.c, README-KSPACE.md

### 构建系统
- Makefile
- .gitignore

### 版本控制
- GIT_GUIDE.md
- SESSION_HISTORY.md

## 📝 维护记录

### 版本 1.0 (2025-01-05)
- ✅ 创建所有核心文件
- ✅ 实现三种解调算法
- ✅ 完成文档编写
- ✅ 集成到主项目
- ✅ 测试和验证

## 📞 获取帮助

### 查看文档
```bash
cat README-AM.md | less
cat QUICKSTART-AM.md
```

### 运行帮助
```bash
./am_signal -h
```

### 查看示例
```bash
head -50 main-am.c  # 查看代码注释
./demo_am.sh        # 运行演示
```

## ✅ 验证清单

使用此清单验证AM系统是否完整安装：

- [ ] main-am.c 存在
- [ ] README-AM.md 存在
- [ ] QUICKSTART-AM.md 存在
- [ ] AM-SUMMARY.md 存在
- [ ] test_am.sh 存在且可执行
- [ ] demo_am.sh 存在且可执行
- [ ] plot_am.py 存在且可执行
- [ ] Makefile 包含 am_signal 目标
- [ ] README.md 包含 AM 章节
- [ ] 程序可以成功编译
- [ ] 程序可以成功运行
- [ ] 生成正确的输出文件

---

**文件清单完成！** 📋✨
