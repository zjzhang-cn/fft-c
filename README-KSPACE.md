# K空间数据 (K-Space Data)

## 什么是K空间?

K空间(K-Space)是频域空间的另一种称呼,在医学成像(特别是MRI)中被广泛使用。K空间数据包含了图像的所有频率信息,是傅里叶变换后的复数数据。

## 数据格式

### 二进制格式 (kspace_data.bin)

- **文件头**: 
  - 宽度 (int, 4字节)
  - 高度 (int, 4字节)
- **实部数据**: width × height 个 double (8字节/个)
- **虚部数据**: width × height 个 double (8字节/个)

**总大小**: 8 + (width × height × 2 × 8) 字节

对于256×256图像:
- 文件头: 8 字节
- 实部: 256 × 256 × 8 = 524,288 字节
- 虚部: 256 × 256 × 8 = 524,288 字节
- **总计**: 1,048,584 字节 (约1MB)

### 文本格式 (kspace_data.txt)

每行格式:
```
row col real imag magnitude phase(rad)
```

- **row**: 行索引 (0到height-1)
- **col**: 列索引 (0到width-1)
- **real**: 实部值
- **imag**: 虚部值
- **magnitude**: 幅度值 = √(real² + imag²)
- **phase**: 相位值(弧度) = atan2(imag, real)

## 使用方法

### 1. 保存K空间数据

程序自动在执行2D FFT后保存K空间数据:

```c
save_kspace_binary("kspace_data.bin", X_real, X_imag, width, height);
save_kspace_txt("kspace_data.txt", X_real, X_imag, width, height);
```

### 2. 加载K空间数据

从二进制文件加载:

```c
double *X_real = NULL;
double *X_imag = NULL;
int width, height;

load_kspace_binary("kspace_data.bin", &X_real, &X_imag, &width, &height);
```

### 3. 从K空间重建图像

使用逆傅里叶变换:

```c
double *image_real = malloc(width * height * sizeof(double));
double *image_imag = malloc(width * height * sizeof(double));

calculate_2d_idft(X_real, X_imag, height, width, image_real, image_imag);
```

## 应用场景

### 医学成像 (MRI)
- 📡 **原始数据存储**: MRI扫描仪直接采集K空间数据
- 🔄 **图像重建**: 通过逆FFT从K空间重建图像
- 🎯 **部分采样**: 欠采样K空间加速扫描,后续通过算法重建

### 图像处理
- 🔍 **频域滤波**: 修改K空间数据进行滤波
- 📦 **图像压缩**: 保留重要频率成分
- ✨ **图像增强**: 选择性增强特定频率

### 科学研究
- 🔬 **信号分析**: 研究信号的频域特性
- 📊 **数据可视化**: 可视化频域信息
- 🎓 **教学演示**: 展示傅里叶变换原理

## 数据特性

### K空间中心 vs 边缘

- **中心区域**: 
  - 包含低频信息
  - 决定图像对比度和整体结构
  - 幅度值较大

- **边缘区域**:
  - 包含高频信息
  - 决定图像细节和边缘
  - 幅度值较小

### 共轭对称性

对于实数图像,K空间具有共轭对称性:
```
K(ky, kx) = K*(-ky, -kx)
```
其中 * 表示复共轭

## 示例程序

查看 `load_kspace_demo.c` 了解如何:
1. 加载保存的K空间数据
2. 执行逆傅里叶变换
3. 重建原始图像

## 注意事项

⚠️ **文件大小**: 
- 文本格式文件较大(约4-5倍于二进制格式)
- 建议长期存储使用二进制格式

💡 **精度**: 
- 二进制格式使用双精度浮点数(64位)
- 保证了数值精度,适合科学计算

🔬 **可逆性**:
- K空间数据包含完整的频域信息
- 可以完美重建原始图像(误差在浮点精度范围内)

## 相关文件

- `main-fft2d.c` - 主程序,生成K空间数据
- `kspace_data.bin` - K空间二进制数据文件
- `kspace_data.txt` - K空间文本数据文件
- `load_kspace_demo.c` - K空间数据加载示例

## 参考资料

- [傅里叶变换](https://en.wikipedia.org/wiki/Fourier_transform)
- [K空间(MRI)](https://en.wikipedia.org/wiki/K-space_(magnetic_resonance_imaging))
- [离散傅里叶变换](https://en.wikipedia.org/wiki/Discrete_Fourier_transform)
