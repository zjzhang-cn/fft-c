# 项目文档索引

**最后更新**: 2025年11月5日  
**项目版本**: 2.0

---

## 📚 文档总览

本项目包含完整的文档体系，涵盖从快速入门到深度技术细节的各个层面。

---

## 🚀 新手入门 (5分钟)

### 最快上手路径
1. **[QUICKSTART.md](QUICKSTART.md)** ⭐ 开始这里！
   - 快速编译和运行
   - 基本使用示例
   - 输出文件说明

2. **[QUICKREF-KSPACE.md](QUICKREF-KSPACE.md)** ⭐ 快速参考
   - 核心命令一览
   - 常用操作速查
   - 问题快速排查

---

## 📖 用户文档 (理解功能)

### 主要功能文档
1. **[README.md](README.md)** - 项目总览
   - 项目简介
   - 主要功能
   - 快速开始

2. **[README-FFT2D.md](README-FFT2D.md)** - 2D FFT详解
   - 2D FFT算法说明
   - BMP图像输出
   - 测试模式介绍
   - 技术细节

3. **[README-KSPACE.md](README-KSPACE.md)** - K空间数据格式
   - K空间概念
   - 二进制格式规范
   - 文本格式规范
   - 使用示例

4. **[README-KSPACE-RECONSTRUCTION.md](README-KSPACE-RECONSTRUCTION.md)** ⭐ K空间重建
   - 重建程序使用指南
   - 输出文件说明
   - 精度分析
   - 应用场景

5. **[KSPACE-RECONSTRUCTION-SUMMARY.md](KSPACE-RECONSTRUCTION-SUMMARY.md)** - 功能总结
   - 完整工作流程
   - 验证结果
   - 文件结构
   - 未来扩展

---

## 🔧 开发者文档 (深入理解)

### 开发历史和技术细节
1. **[SESSION_HISTORY.md](SESSION_HISTORY.md)** ⭐⭐⭐ 完整开发历史
   - 开发时间线
   - 每个阶段的详细记录
   - 技术决策说明
   - 代码实现细节
   - 所有关键算法
   - 问题解决过程

2. **[PROJECT_SNAPSHOT.md](PROJECT_SNAPSHOT.md)** ⭐⭐ 项目快照
   - 当前项目状态
   - 文件清单
   - 技术指标
   - 快速恢复指南

3. **[GIT_GUIDE.md](GIT_GUIDE.md)** - Git操作指南
   - 提交规范
   - 分支管理
   - 版本标签
   - 备份恢复

4. **[DOC_INDEX.md](DOC_INDEX.md)** - 文档索引（本文件）
   - 文档导航
   - 快速查找

---

## 📋 文档分类速查

### 按用途分类

#### 快速参考类 ⚡
- `QUICKSTART.md` - 5分钟快速开始
- `QUICKREF-KSPACE.md` - 命令速查表
- `DOC_INDEX.md` - 文档导航

#### 功能说明类 📘
- `README.md` - 项目主文档
- `README-FFT2D.md` - 2D FFT功能
- `README-KSPACE.md` - K空间格式
- `README-KSPACE-RECONSTRUCTION.md` - 重建功能

#### 技术深度类 🔬
- `SESSION_HISTORY.md` - 完整开发历史
- `KSPACE-RECONSTRUCTION-SUMMARY.md` - 技术总结
- `PROJECT_SNAPSHOT.md` - 项目状态

#### 工具指南类 🛠️
- `GIT_GUIDE.md` - Git使用指南
- `test_kspace_reconstruction.sh` - 自动测试脚本

---

## 🎯 按需求查找文档

### 我想快速开始使用
→ **[QUICKSTART.md](QUICKSTART.md)**

### 我想了解如何使用K空间重建功能
→ **[README-KSPACE-RECONSTRUCTION.md](README-KSPACE-RECONSTRUCTION.md)**  
→ **[QUICKREF-KSPACE.md](QUICKREF-KSPACE.md)**

### 我想理解2D FFT是如何实现的
→ **[README-FFT2D.md](README-FFT2D.md)**  
→ **[SESSION_HISTORY.md](SESSION_HISTORY.md)** (第二阶段)

### 我想知道K空间数据格式
→ **[README-KSPACE.md](README-KSPACE.md)**

### 我想了解完整的开发过程
→ **[SESSION_HISTORY.md](SESSION_HISTORY.md)** ⭐⭐⭐

### 我想恢复或继续开发这个项目
→ **[SESSION_HISTORY.md](SESSION_HISTORY.md)**  
→ **[PROJECT_SNAPSHOT.md](PROJECT_SNAPSHOT.md)**

### 我想查看项目当前状态
→ **[PROJECT_SNAPSHOT.md](PROJECT_SNAPSHOT.md)**

### 我想了解如何使用Git管理项目
→ **[GIT_GUIDE.md](GIT_GUIDE.md)**

### 我遇到了问题需要排查
→ **[QUICKREF-KSPACE.md](QUICKREF-KSPACE.md)** (故障排除)  
→ **[README-KSPACE-RECONSTRUCTION.md](README-KSPACE-RECONSTRUCTION.md)** (故障排除)

---

## 📊 文档统计

| 文档类型 | 文件数 | 总字数 | 说明 |
|---------|--------|--------|------|
| 快速参考 | 2 | ~3,000 | 快速上手 |
| 功能说明 | 5 | ~6,000 | 详细功能 |
| 技术深度 | 3 | ~12,000 | 开发历史 |
| 工具指南 | 2 | ~2,000 | 辅助工具 |
| **总计** | **12** | **~23,000** | 完整文档 |

---

## 🗺️ 学习路径建议

### 路径1: 用户使用 (2小时)
```
1. QUICKSTART.md (10分钟)
   ↓
2. README-FFT2D.md (30分钟)
   ↓
3. README-KSPACE-RECONSTRUCTION.md (30分钟)
   ↓
4. 实践操作 (50分钟)
```

### 路径2: 开发理解 (4小时)
```
1. README.md (20分钟)
   ↓
2. SESSION_HISTORY.md (120分钟) ⭐
   ↓
3. PROJECT_SNAPSHOT.md (30分钟)
   ↓
4. 代码阅读 (70分钟)
```

### 路径3: 深度研究 (8小时)
```
1. SESSION_HISTORY.md (150分钟)
   ↓
2. 所有README系列 (120分钟)
   ↓
3. 源代码详读 (180分钟)
   ↓
4. 实验和修改 (180分钟)
```

---

## 🔍 关键主题索引

### 算法实现
- **1D DFT**: SESSION_HISTORY.md → 第一阶段
- **2D DFT**: SESSION_HISTORY.md → 第二阶段
- **2D IDFT**: SESSION_HISTORY.md → 第五阶段
- **FFTShift**: README-FFT2D.md, SESSION_HISTORY.md

### 文件格式
- **BMP格式**: SESSION_HISTORY.md → 第四阶段
- **K空间二进制**: README-KSPACE.md
- **K空间文本**: README-KSPACE.md

### 功能特性
- **图像生成**: README-FFT2D.md → 测试图像模式
- **频谱可视化**: README-FFT2D.md → 输出文件
- **图像还原**: README-FFT2D.md → 图像还原演示
- **K空间保存**: README-KSPACE.md
- **K空间加载**: README-KSPACE-RECONSTRUCTION.md

### 精度验证
- **误差分析**: SESSION_HISTORY.md → 精度验证
- **虚部残差**: README-KSPACE-RECONSTRUCTION.md
- **浮点精度**: SESSION_HISTORY.md → 开发经验总结

### 应用场景
- **医学影像**: README-KSPACE.md, KSPACE-RECONSTRUCTION-SUMMARY.md
- **频域滤波**: SESSION_HISTORY.md → 应用场景
- **数据压缩**: SESSION_HISTORY.md → 应用场景

---

## 📝 文档更新记录

| 日期 | 文档 | 更新内容 |
|------|------|----------|
| 2025-11-03 | README-FFT2D.md | 创建2D FFT文档 |
| 2025-11-03 | README-KSPACE.md | 创建K空间格式文档 |
| 2025-11-03 | README-KSPACE-RECONSTRUCTION.md | 创建重建指南 |
| 2025-11-03 | KSPACE-RECONSTRUCTION-SUMMARY.md | 创建功能总结 |
| 2025-11-03 | QUICKREF-KSPACE.md | 创建快速参考 |
| 2025-11-05 | SESSION_HISTORY.md | 创建完整会话历史 ⭐ |
| 2025-11-05 | PROJECT_SNAPSHOT.md | 创建项目快照 |
| 2025-11-05 | GIT_GUIDE.md | 创建Git指南 |
| 2025-11-05 | DOC_INDEX.md | 创建文档索引 |

---

## 🎓 推荐阅读顺序

### 第一天：快速上手
1. ✅ QUICKSTART.md
2. ✅ README.md
3. ✅ 运行程序实践

### 第二天：深入功能
1. ✅ README-FFT2D.md
2. ✅ README-KSPACE-RECONSTRUCTION.md
3. ✅ QUICKREF-KSPACE.md

### 第三天：理解原理
1. ✅ SESSION_HISTORY.md (重点！)
2. ✅ README-KSPACE.md
3. ✅ 阅读源代码

### 第四天：项目掌握
1. ✅ PROJECT_SNAPSHOT.md
2. ✅ KSPACE-RECONSTRUCTION-SUMMARY.md
3. ✅ 尝试修改和扩展

---

## 🔗 文档间关联

```
README.md (入口)
  ├─→ QUICKSTART.md (快速开始)
  ├─→ README-FFT2D.md (2D FFT)
  │     └─→ SESSION_HISTORY.md (算法细节)
  ├─→ README-KSPACE.md (数据格式)
  │     └─→ SESSION_HISTORY.md (格式设计)
  └─→ README-KSPACE-RECONSTRUCTION.md (重建功能)
        ├─→ QUICKREF-KSPACE.md (快速参考)
        ├─→ SESSION_HISTORY.md (实现历史)
        └─→ KSPACE-RECONSTRUCTION-SUMMARY.md (总结)

SESSION_HISTORY.md (核心文档)
  ├─→ 包含所有技术细节
  ├─→ 包含完整开发历程
  └─→ 可独立用于项目恢复

PROJECT_SNAPSHOT.md (状态快照)
  ├─→ 当前项目状态
  ├─→ 文件清单
  └─→ 快速恢复指南

GIT_GUIDE.md (版本控制)
  └─→ Git操作规范

DOC_INDEX.md (导航中心)
  └─→ 本文件，导航所有文档
```

---

## ⚡ 常见场景快速导航

| 场景 | 推荐文档 | 时间 |
|------|----------|------|
| 第一次使用 | QUICKSTART.md | 10分钟 |
| 了解2D FFT | README-FFT2D.md | 30分钟 |
| 使用K空间重建 | README-KSPACE-RECONSTRUCTION.md | 30分钟 |
| 查找命令 | QUICKREF-KSPACE.md | 5分钟 |
| 理解数据格式 | README-KSPACE.md | 20分钟 |
| 深入学习算法 | SESSION_HISTORY.md | 2小时 |
| 项目恢复 | PROJECT_SNAPSHOT.md + SESSION_HISTORY.md | 30分钟 |
| Git操作 | GIT_GUIDE.md | 20分钟 |
| 故障排查 | QUICKREF-KSPACE.md → 故障排除 | 10分钟 |

---

## 💡 文档使用技巧

### 快速搜索
```bash
# 在所有Markdown文件中搜索关键词
grep -r "2D DFT" *.md

# 搜索特定主题
grep -r "K空间" *.md

# 搜索函数名
grep -r "calculate_2d_dft" *.md *.c
```

### 离线阅读
```bash
# 使用Markdown查看器
markdown-reader SESSION_HISTORY.md

# 或使用less
less SESSION_HISTORY.md

# 或使用cat配合grep
cat SESSION_HISTORY.md | grep -A 10 "2D DFT"
```

### 生成PDF（可选）
```bash
# 使用pandoc转换
pandoc SESSION_HISTORY.md -o SESSION_HISTORY.pdf

# 批量转换所有文档
for f in *.md; do
    pandoc "$f" -o "${f%.md}.pdf"
done
```

---

## 📌 重要提示

### ⭐⭐⭐ 核心文档（必读）
1. **SESSION_HISTORY.md** - 包含完整开发历程和所有技术细节
2. **PROJECT_SNAPSHOT.md** - 项目当前状态和快速恢复
3. **README-KSPACE-RECONSTRUCTION.md** - K空间重建完整指南

### ⭐⭐ 重要文档（推荐）
1. **README-FFT2D.md** - 2D FFT详细说明
2. **QUICKREF-KSPACE.md** - 快速参考手册
3. **README-KSPACE.md** - K空间数据格式

### ⭐ 辅助文档
1. **README.md** - 项目总览
2. **QUICKSTART.md** - 快速入门
3. **GIT_GUIDE.md** - Git使用
4. **KSPACE-RECONSTRUCTION-SUMMARY.md** - 功能总结

---

## 🆘 获取帮助

### 文档中找不到答案？
1. 先查看 **SESSION_HISTORY.md** - 最全面
2. 再查看 **QUICKREF-KSPACE.md** - 常见问题
3. 查看源代码注释 - `*.c` 文件
4. 运行测试脚本验证 - `./test_kspace_reconstruction.sh`

### 需要恢复项目？
1. 阅读 **PROJECT_SNAPSHOT.md** → 快速恢复指南
2. 阅读 **SESSION_HISTORY.md** → 完整开发历程
3. 按步骤重新编译和测试

---

## 📚 相关资源

### 外部参考
- Cooley-Tukey FFT算法
- 离散傅里叶变换理论
- BMP文件格式规范
- MRI K空间成像原理

### 在线资源
- C99标准文档
- GCC编译器手册
- GNU Make文档
- Git官方教程

---

**索引维护者**: 项目文档管理  
**最后更新**: 2025年11月5日  
**文档总数**: 12个  
**总字数**: 约23,000字

---

*使用本索引可快速定位所需文档，所有文档相互关联形成完整知识体系。*
