#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// BMP 文件头结构
#pragma pack(push, 1)
typedef struct {
    uint16_t type;        // 文件类型，必须是 0x4D42 ('BM')
    uint32_t size;        // 文件大小（字节）
    uint16_t reserved1;   // 保留，必须是 0
    uint16_t reserved2;   // 保留，必须是 0
    uint32_t offset;      // 从文件头到位图数据的偏移量
} BMPFileHeader;

// BMP 信息头结构
typedef struct {
    uint32_t size;           // 信息头大小
    int32_t  width;          // 图像宽度
    int32_t  height;         // 图像高度
    uint16_t planes;         // 颜色平面数，必须是 1
    uint16_t bits;           // 每像素位数
    uint32_t compression;    // 压缩类型
    uint32_t imagesize;      // 图像大小
    int32_t  xresolution;    // 水平分辨率
    int32_t  yresolution;    // 垂直分辨率
    uint32_t ncolors;        // 颜色数
    uint32_t importantcolors;// 重要颜色数
} BMPInfoHeader;
#pragma pack(pop)

/**
 * 将数值归一化到 [0, 255] 范围
 */
uint8_t normalize_to_byte(double value, double min_val, double max_val) {
    if (max_val == min_val) return 128;
    double normalized = (value - min_val) / (max_val - min_val) * 255.0;
    if (normalized < 0) return 0;
    if (normalized > 255) return 255;
    return (uint8_t)normalized;
}

/**
 * 将 2D 数据保存为灰度 BMP 图像
 * @param filename 输出文件名
 * @param data 2D 数据数组
 * @param width 图像宽度
 * @param height 图像高度
 */
int save_bmp_grayscale(const char* filename, double* data, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("无法创建文件: %s\n", filename);
        return -1;
    }
    
    // 找出数据的最小值和最大值
    double min_val = data[0], max_val = data[0];
    for (int i = 1; i < width * height; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    // BMP 要求每行字节数是 4 的倍数
    int row_size = ((width * 3 + 3) / 4) * 4;
    int padding = row_size - width * 3;
    
    // 填充文件头
    BMPFileHeader file_header;
    file_header.type = 0x4D42;  // 'BM'
    file_header.size = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + row_size * height;
    file_header.reserved1 = 0;
    file_header.reserved2 = 0;
    file_header.offset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    
    // 填充信息头
    BMPInfoHeader info_header;
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = width;
    info_header.height = height;
    info_header.planes = 1;
    info_header.bits = 24;  // 24位真彩色
    info_header.compression = 0;
    info_header.imagesize = row_size * height;
    info_header.xresolution = 2835;  // 72 DPI
    info_header.yresolution = 2835;
    info_header.ncolors = 0;
    info_header.importantcolors = 0;
    
    // 写入文件头和信息头
    fwrite(&file_header, sizeof(BMPFileHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);
    
    // 写入像素数据 (BMP 从下到上存储)
    uint8_t pad[3] = {0, 0, 0};
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            uint8_t pixel = normalize_to_byte(data[i * width + j], min_val, max_val);
            // BGR 格式
            fwrite(&pixel, 1, 1, file);  // B
            fwrite(&pixel, 1, 1, file);  // G
            fwrite(&pixel, 1, 1, file);  // R
        }
        // 写入填充字节
        if (padding > 0) {
            fwrite(pad, 1, padding, file);
        }
    }
    
    fclose(file);
    printf("已保存图像: %s (尺寸: %dx%d, 范围: [%.3f, %.3f])\n", 
           filename, width, height, min_val, max_val);
    return 0;
}

/**
 * 保存K空间数据到文本文件
 * @param filename 输出文件名
 * @param real 实部数据数组
 * @param imag 虚部数据数组
 * @param width 数据宽度
 * @param height 数据高度
 */
int save_kspace_txt(const char* filename, double* real, double* imag, int width, int height) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件: %s\n", filename);
        return -1;
    }
    
    // 写入文件头
    fprintf(file, "# K-Space Data (Frequency Domain)\n");
    fprintf(file, "# Size: %d x %d\n", width, height);
    fprintf(file, "# Format: row col real imag magnitude phase(rad)\n");
    fprintf(file, "#\n");
    
    // 写入数据
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = i * width + j;
            double r = real[idx];
            double im = imag[idx];
            double mag = sqrt(r * r + im * im);
            double phase = atan2(im, r);
            fprintf(file, "%4d %4d %15.8e %15.8e %15.8e %15.8e\n", 
                    i, j, r, im, mag, phase);
        }
    }
    
    fclose(file);
    printf("已保存K空间数据: %s (尺寸: %dx%d)\n", filename, width, height);
    return 0;
}

/**
 * 保存K空间数据到二进制文件
 * @param filename 输出文件名
 * @param real 实部数据数组
 * @param imag 虚部数据数组
 * @param width 数据宽度
 * @param height 数据高度
 */
int save_kspace_binary(const char* filename, double* real, double* imag, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("无法创建文件: %s\n", filename);
        return -1;
    }
    
    // 写入文件头
    fwrite(&width, sizeof(int), 1, file);
    fwrite(&height, sizeof(int), 1, file);
    
    // 写入实部数据
    fwrite(real, sizeof(double), width * height, file);
    
    // 写入虚部数据
    fwrite(imag, sizeof(double), width * height, file);
    
    fclose(file);
    printf("已保存K空间二进制数据: %s (尺寸: %dx%d, 大小: %ld 字节)\n", 
           filename, width, height, 
           (long)(2 * sizeof(int) + 2 * width * height * sizeof(double)));
    return 0;
}

/**
 * 从二进制文件加载K空间数据
 * @param filename 输入文件名
 * @param real 实部数据数组(需要预先分配)
 * @param imag 虚部数据数组(需要预先分配)
 * @param width 输出数据宽度
 * @param height 输出数据高度
 */
int load_kspace_binary(const char* filename, double** real, double** imag, int* width, int* height) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("无法打开文件: %s\n", filename);
        return -1;
    }
    
    // 读取文件头
    fread(width, sizeof(int), 1, file);
    fread(height, sizeof(int), 1, file);
    
    // 分配内存
    *real = (double*)malloc((*width) * (*height) * sizeof(double));
    *imag = (double*)malloc((*width) * (*height) * sizeof(double));
    
    if (!*real || !*imag) {
        printf("内存分配失败\n");
        fclose(file);
        return -1;
    }
    
    // 读取实部数据
    fread(*real, sizeof(double), (*width) * (*height), file);
    
    // 读取虚部数据
    fread(*imag, sizeof(double), (*width) * (*height), file);
    
    fclose(file);
    printf("已加载K空间数据: %s (尺寸: %dx%d)\n", filename, *width, *height);
    return 0;
}

/**
 * FFT频谱中心化 (FFTShift)
 * 将零频率分量移到频谱中心
 * @param data 输入/输出数据数组
 * @param width 图像宽度
 * @param height 图像高度
 */
void fft_shift(double* data, int width, int height) {
    double* temp = (double*)malloc(width * height * sizeof(double));
    if (!temp) return;
    
    int half_h = height / 2;
    int half_w = width / 2;
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int new_i = (i + half_h) % height;
            int new_j = (j + half_w) % width;
            temp[new_i * width + new_j] = data[i * width + j];
        }
    }
    
    // 复制回原数组
    for (int i = 0; i < width * height; i++) {
        data[i] = temp[i];
    }
    
    free(temp);
}

/**
 * 计算一维离散傅里叶变换 (1D DFT)
 * @param x_real 输入信号的实部数组
 * @param x_imag 输入信号的虚部数组
 * @param N 信号长度
 * @param X_real 输出信号频域的实部数组
 * @param X_imag 输出信号频域的虚部数组
 */
void calculate_1d_dft(double* x_real, double* x_imag, int N, double* X_real, double* X_imag) {
    for (int k = 0; k < N; k++) {
        X_real[k] = 0.0;
        X_imag[k] = 0.0;
        for (int n = 0; n < N; n++) {
            double angle = 2.0 * M_PI * k * n / N;
            double cos_val = cos(angle);
            double sin_val = sin(angle);
            // 复数乘法: (a + bi) * (cos - i*sin)
            X_real[k] += x_real[n] * cos_val + x_imag[n] * sin_val;
            X_imag[k] += x_imag[n] * cos_val - x_real[n] * sin_val;
        }
    }
}

/**
 * 计算一维离散傅里叶逆变换 (1D IDFT)
 * @param X_real 输入频域信号的实部数组
 * @param X_imag 输入频域信号的虚部数组
 * @param N 信号长度
 * @param x_real 输出时域信号的实部数组
 * @param x_imag 输出时域信号的虚部数组
 */
void calculate_1d_idft(double* X_real, double* X_imag, int N, double* x_real, double* x_imag) {
    for (int n = 0; n < N; n++) {
        x_real[n] = 0.0;
        x_imag[n] = 0.0;
        for (int k = 0; k < N; k++) {
            double angle = 2.0 * M_PI * k * n / N;
            double cos_val = cos(angle);
            double sin_val = sin(angle);
            // 复数乘法: (a + bi) * (cos + i*sin)，注意符号与DFT相反
            x_real[n] += X_real[k] * cos_val - X_imag[k] * sin_val;
            x_imag[n] += X_imag[k] * cos_val + X_real[k] * sin_val;
        }
        // 归一化
        x_real[n] /= N;
        x_imag[n] /= N;
    }
}

/**
 * 计算二维离散傅里叶变换 (2D DFT)
 * @param x_real 输入信号的实部数组 (M x N)
 * @param x_imag 输入信号的虚部数组 (M x N)
 * @param M 行数
 * @param N 列数
 * @param X_real 输出信号频域的实部数组 (M x N)
 * @param X_imag 输出信号频域的虚部数组 (M x N)
 */
void calculate_2d_dft(double* x_real, double* x_imag, int M, int N, 
                      double* X_real, double* X_imag) {
    // 临时存储数组
    double *temp_real = (double *)malloc(M * N * sizeof(double));
    double *temp_imag = (double *)malloc(M * N * sizeof(double));
    double *row_real = (double *)malloc(N * sizeof(double));
    double *row_imag = (double *)malloc(N * sizeof(double));
    double *row_out_real = (double *)malloc(N * sizeof(double));
    double *row_out_imag = (double *)malloc(N * sizeof(double));
    
    if (!temp_real || !temp_imag || !row_real || !row_imag || !row_out_real || !row_out_imag) {
        printf("内存分配失败\n");
        return;
    }
    
    // 第一步: 对每一行进行1D DFT
    for (int i = 0; i < M; i++) {
        // 提取第i行
        for (int j = 0; j < N; j++) {
            row_real[j] = x_real[i * N + j];
            row_imag[j] = x_imag[i * N + j];
        }
        
        // 对该行进行1D DFT
        calculate_1d_dft(row_real, row_imag, N, row_out_real, row_out_imag);
        
        // 存储结果
        for (int j = 0; j < N; j++) {
            temp_real[i * N + j] = row_out_real[j];
            temp_imag[i * N + j] = row_out_imag[j];
        }
    }
    
    // 分配列数组
    double *col_real = (double *)malloc(M * sizeof(double));
    double *col_imag = (double *)malloc(M * sizeof(double));
    double *col_out_real = (double *)malloc(M * sizeof(double));
    double *col_out_imag = (double *)malloc(M * sizeof(double));
    
    if (!col_real || !col_imag || !col_out_real || !col_out_imag) {
        printf("内存分配失败\n");
        free(temp_real); free(temp_imag);
        free(row_real); free(row_imag); free(row_out_real); free(row_out_imag);
        return;
    }
    
    // 第二步: 对每一列进行1D DFT
    for (int j = 0; j < N; j++) {
        // 提取第j列
        for (int i = 0; i < M; i++) {
            col_real[i] = temp_real[i * N + j];
            col_imag[i] = temp_imag[i * N + j];
        }
        
        // 对该列进行1D DFT
        calculate_1d_dft(col_real, col_imag, M, col_out_real, col_out_imag);
        
        // 存储结果
        for (int i = 0; i < M; i++) {
            X_real[i * N + j] = col_out_real[i];
            X_imag[i * N + j] = col_out_imag[i];
        }
    }
    
    // 释放临时内存
    free(temp_real);
    free(temp_imag);
    free(row_real);
    free(row_imag);
    free(row_out_real);
    free(row_out_imag);
    free(col_real);
    free(col_imag);
    free(col_out_real);
    free(col_out_imag);
}

/**
 * 计算二维离散傅里叶逆变换 (2D IDFT)
 * @param X_real 输入频域信号的实部数组 (M x N)
 * @param X_imag 输入频域信号的虚部数组 (M x N)
 * @param M 行数
 * @param N 列数
 * @param x_real 输出时域信号的实部数组 (M x N)
 * @param x_imag 输出时域信号的虚部数组 (M x N)
 */
void calculate_2d_idft(double* X_real, double* X_imag, int M, int N, 
                       double* x_real, double* x_imag) {
    // 临时存储数组
    double *temp_real = (double *)malloc(M * N * sizeof(double));
    double *temp_imag = (double *)malloc(M * N * sizeof(double));
    double *row_real = (double *)malloc(N * sizeof(double));
    double *row_imag = (double *)malloc(N * sizeof(double));
    double *row_out_real = (double *)malloc(N * sizeof(double));
    double *row_out_imag = (double *)malloc(N * sizeof(double));
    
    if (!temp_real || !temp_imag || !row_real || !row_imag || !row_out_real || !row_out_imag) {
        printf("内存分配失败\n");
        return;
    }
    
    // 第一步: 对每一行进行1D IDFT
    for (int i = 0; i < M; i++) {
        // 提取第i行
        for (int j = 0; j < N; j++) {
            row_real[j] = X_real[i * N + j];
            row_imag[j] = X_imag[i * N + j];
        }
        
        // 对该行进行1D IDFT
        calculate_1d_idft(row_real, row_imag, N, row_out_real, row_out_imag);
        
        // 存储结果
        for (int j = 0; j < N; j++) {
            temp_real[i * N + j] = row_out_real[j];
            temp_imag[i * N + j] = row_out_imag[j];
        }
    }
    
    // 分配列数组
    double *col_real = (double *)malloc(M * sizeof(double));
    double *col_imag = (double *)malloc(M * sizeof(double));
    double *col_out_real = (double *)malloc(M * sizeof(double));
    double *col_out_imag = (double *)malloc(M * sizeof(double));
    
    if (!col_real || !col_imag || !col_out_real || !col_out_imag) {
        printf("内存分配失败\n");
        free(temp_real); free(temp_imag);
        free(row_real); free(row_imag); free(row_out_real); free(row_out_imag);
        return;
    }
    
    // 第二步: 对每一列进行1D IDFT
    for (int j = 0; j < N; j++) {
        // 提取第j列
        for (int i = 0; i < M; i++) {
            col_real[i] = temp_real[i * N + j];
            col_imag[i] = temp_imag[i * N + j];
        }
        
        // 对该列进行1D IDFT
        calculate_1d_idft(col_real, col_imag, M, col_out_real, col_out_imag);
        
        // 存储结果
        for (int i = 0; i < M; i++) {
            x_real[i * N + j] = col_out_real[i];
            x_imag[i * N + j] = col_out_imag[i];
        }
    }
    
    // 释放临时内存
    free(temp_real);
    free(temp_imag);
    free(row_real);
    free(row_imag);
    free(row_out_real);
    free(row_out_imag);
    free(col_real);
    free(col_imag);
    free(col_out_real);
    free(col_out_imag);
}

int main() {
    int M = 256;  // 图像行数 (增大以生成更清晰的图像)
    int N = 256;  // 图像列数
    
    printf("2D FFT 示例 - 图像尺寸: %d x %d\n\n", M, N);
    
    // 分配内存用于存储2D信号 (模拟图像)
    double *x_real = (double *)calloc(M * N, sizeof(double));
    double *x_imag = (double *)calloc(M * N, sizeof(double));
    double *X_real = (double *)malloc(M * N * sizeof(double));
    double *X_imag = (double *)malloc(M * N * sizeof(double));
    double *magnitude = (double *)malloc(M * N * sizeof(double));
    
    if (!x_real || !x_imag || !X_real || !X_imag || !magnitude) {
        printf("内存分配失败\n");
        return 1;
    }
    
    // 生成测试图像: 多个2D正弦波的叠加
    printf("生成测试图像 (多个2D频率成分的叠加)...\n\n");
    
    // 频率成分1: 低频
    double fx1 = 2.0, fy1 = 1.0, amp1 = 1.0;
    // 频率成分2: 中频
    double fx2 = 4.0, fy2 = 3.0, amp2 = 0.5;
    // 频率成分3: 高频
    double fx3 = 1.0, fy3 = 5.0, amp3 = 0.3;
    
    printf("频率成分:\n");
    printf("  1. fx=%.1f, fy=%.1f, 振幅=%.2f\n", fx1, fy1, amp1);
    printf("  2. fx=%.1f, fy=%.1f, 振幅=%.2f\n", fx2, fy2, amp2);
    printf("  3. fx=%.1f, fy=%.1f, 振幅=%.2f\n\n", fx3, fy3, amp3);
    
    // 方式1: 正弦波叠加
    // for (int i = 0; i < M; i++) {
    //     for (int j = 0; j < N; j++) {
    //         x_real[i * N + j] = 
    //             amp1 * sin(2.0 * M_PI * fx1 * j / N) * sin(2.0 * M_PI * fy1 * i / M) +
    //             amp2 * sin(2.0 * M_PI * fx2 * j / N) * sin(2.0 * M_PI * fy2 * i / M) +
    //             amp3 * sin(2.0 * M_PI * fx3 * j / N) * sin(2.0 * M_PI * fy3 * i / M);
    //         x_imag[i * N + j] = 0.0;  // 输入为实数信号
    //     }
    // }
    
    // 方式2: 添加一个中心圆形 (取消下面注释可使用)
    /*
    int center_i = M / 2;
    int center_j = N / 2;
    int radius = M / 8;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int di = i - center_i;
            int dj = j - center_j;
            if (di * di + dj * dj < radius * radius) {
                x_real[i * N + j] = 1.0;
            } else {
                x_real[i * N + j] = 0.0;
            }
            x_imag[i * N + j] = 0.0;
        }
    }
    */
    
    // 方式3: 添加一个方形 (取消下面注释可使用)
#if 1    
    int box_size = M / 4;
    int box_start = M / 2 - box_size / 2;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (i >= box_start && i < box_start + box_size &&
                j >= box_start && j < box_start + box_size) {
                x_real[i * N + j] = 1.0;
            } else {
                x_real[i * N + j] = 0.0;
            }
            x_imag[i * N + j] = 0.0;
        }
    }
#endif
    
    // 显示原始图像 (部分)
    printf("原始图像 (左上角 8x8 区域):\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%6.2f ", x_real[i * N + j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // 执行2D DFT
    printf("正在执行 2D DFT...\n");
    calculate_2d_dft(x_real, x_imag, M, N, X_real, X_imag);
    
    // 计算幅度谱
    for (int i = 0; i < M * N; i++) {
        magnitude[i] = sqrt(X_real[i] * X_real[i] + X_imag[i] * X_imag[i]);
    }
    
    // 保存原始图像为BMP
    printf("\n保存图像文件...\n");
    save_bmp_grayscale("original_image.bmp", x_real, N, M);
    
    // 保存K空间数据 (频域数据)
    printf("\n保存K空间数据...\n");
    save_kspace_binary("kspace_data.bin", X_real, X_imag, N, M);
    save_kspace_txt("kspace_data.txt", X_real, X_imag, N, M);
    
    // 对幅度谱进行中心化
    fft_shift(magnitude, N, M);
    
    // 保存幅度谱为BMP (对数尺度以便更好地可视化)
    double *log_magnitude = (double *)malloc(M * N * sizeof(double));
    if (log_magnitude) {
        for (int i = 0; i < M * N; i++) {
            log_magnitude[i] = log(1.0 + magnitude[i]);  // log(1+x) 避免 log(0)
        }
        save_bmp_grayscale("magnitude_spectrum.bmp", log_magnitude, N, M);
        save_bmp_grayscale("magnitude_spectrum_linear.bmp", magnitude, N, M);
        free(log_magnitude);
    }
    
    // 执行2D逆DFT (IDFT) 还原图像
    printf("\n正在执行 2D IDFT (逆变换)...\n");
    double *restored_real = (double *)malloc(M * N * sizeof(double));
    double *restored_imag = (double *)malloc(M * N * sizeof(double));
    
    if (restored_real && restored_imag) {
        calculate_2d_idft(X_real, X_imag, M, N, restored_real, restored_imag);
        
        // 保存还原的图像 (只保存实部，虚部应该接近0)
        save_bmp_grayscale("restored_image.bmp", restored_real, N, M);
        
        // 计算还原误差
        double max_error = 0.0;
        double avg_error = 0.0;
        for (int i = 0; i < M * N; i++) {
            double error = fabs(x_real[i] - restored_real[i]);
            if (error > max_error) max_error = error;
            avg_error += error;
        }
        avg_error /= (M * N);
        
        printf("还原完成！\n");
        printf("  最大误差: %.6e\n", max_error);
        printf("  平均误差: %.6e\n", avg_error);
        printf("  还原精度: %.2f%%\n", (1.0 - avg_error / fmax(1e-10, fabs(x_real[0]))) * 100.0);
        
        // 显示还原图像的一小部分
        printf("\n还原图像 (左上角 8x8 区域):\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("%6.2f ", restored_real[i * N + j]);
            }
            printf("\n");
        }
        
        free(restored_real);
        free(restored_imag);
    }
    
    // 显示幅度谱
    printf("\n2D 幅度谱 (左上角 8x8 区域):\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%6.1f ", magnitude[i * N + j]);
        }
        printf("\n");
    }
    
    // 找出幅度谱中的峰值位置
    printf("\n主要频率分量 (幅度 > 5.0):\n");
    printf("位置 (ky, kx) | 幅度      | 对应频率 (fy, fx)\n");
    printf("------------------------------------------------\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (magnitude[i * N + j] > 5.0) {
                double fy = (i <= M/2) ? i : i - M;
                double fx = (j <= N/2) ? j : j - N;
                printf("(%2d, %2d)      | %8.2f  | (%.0f, %.0f)\n", 
                       i, j, magnitude[i * N + j], fy, fx);
            }
        }
    }
    
    // 释放内存
    free(x_real);
    free(x_imag);
    free(X_real);
    free(X_imag);
    free(magnitude);
    
    return 0;
}