#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// BMP 文件头结构
#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPFileHeader;

typedef struct {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t  xresolution;
    int32_t  yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
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
    file_header.type = 0x4D42;
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
    info_header.bits = 24;
    info_header.compression = 0;
    info_header.imagesize = row_size * height;
    info_header.xresolution = 2835;
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
            fwrite(&pixel, 1, 1, file);  // B
            fwrite(&pixel, 1, 1, file);  // G
            fwrite(&pixel, 1, 1, file);  // R
        }
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
 * 从二进制文件加载K空间数据
 */
int load_kspace_binary(const char* filename, double** real, double** imag, int* width, int* height) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("无法打开文件: %s\n", filename);
        return -1;
    }
    
    // 读取文件头
    size_t read_count;
    read_count = fread(width, sizeof(int), 1, file);
    if (read_count != 1) {
        printf("读取宽度失败\n");
        fclose(file);
        return -1;
    }
    
    read_count = fread(height, sizeof(int), 1, file);
    if (read_count != 1) {
        printf("读取高度失败\n");
        fclose(file);
        return -1;
    }
    
    printf("K空间数据尺寸: %d x %d\n", *width, *height);
    
    // 分配内存
    *real = (double*)malloc((*width) * (*height) * sizeof(double));
    *imag = (double*)malloc((*width) * (*height) * sizeof(double));
    
    if (!*real || !*imag) {
        printf("内存分配失败\n");
        fclose(file);
        return -1;
    }
    
    // 读取实部数据
    read_count = fread(*real, sizeof(double), (*width) * (*height), file);
    if (read_count != (*width) * (*height)) {
        printf("读取实部数据失败\n");
        fclose(file);
        free(*real);
        free(*imag);
        return -1;
    }
    
    // 读取虚部数据
    read_count = fread(*imag, sizeof(double), (*width) * (*height), file);
    if (read_count != (*width) * (*height)) {
        printf("读取虚部数据失败\n");
        fclose(file);
        free(*real);
        free(*imag);
        return -1;
    }
    
    fclose(file);
    printf("已加载K空间数据: %s (尺寸: %dx%d)\n", filename, *width, *height);
    return 0;
}

/**
 * 计算一维离散傅里叶逆变换 (1D IDFT)
 */
void calculate_1d_idft(double* X_real, double* X_imag, int N, double* x_real, double* x_imag) {
    for (int n = 0; n < N; n++) {
        x_real[n] = 0.0;
        x_imag[n] = 0.0;
        for (int k = 0; k < N; k++) {
            double angle = 2.0 * M_PI * k * n / N;
            double cos_val = cos(angle);
            double sin_val = sin(angle);
            // 复数乘法: (a + bi) * (cos + i*sin)
            x_real[n] += X_real[k] * cos_val - X_imag[k] * sin_val;
            x_imag[n] += X_imag[k] * cos_val + X_real[k] * sin_val;
        }
        // 归一化
        x_real[n] /= N;
        x_imag[n] /= N;
    }
}

/**
 * 计算二维离散傅里叶逆变换 (2D IDFT)
 */
void calculate_2d_idft(double* X_real, double* X_imag, int M, int N, 
                       double* x_real, double* x_imag) {
    printf("正在执行 2D IDFT...\n");
    
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
    printf("  步骤1: 对 %d 行进行 1D IDFT...\n", M);
    for (int i = 0; i < M; i++) {
        if (i % 50 == 0) printf("    处理第 %d/%d 行...\r", i, M);
        
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
    printf("    完成所有行的处理          \n");
    
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
    printf("  步骤2: 对 %d 列进行 1D IDFT...\n", N);
    for (int j = 0; j < N; j++) {
        if (j % 50 == 0) printf("    处理第 %d/%d 列...\r", j, N);
        
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
    printf("    完成所有列的处理          \n");
    
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
    
    printf("2D IDFT 完成！\n");
}

/**
 * FFT频谱中心化 (FFTShift) - 用于可视化幅度谱
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
    
    for (int i = 0; i < width * height; i++) {
        data[i] = temp[i];
    }
    
    free(temp);
}

int main(int argc, char *argv[]) {
    printf("=================================================\n");
    printf("  K空间数据 → 图像还原程序\n");
    printf("=================================================\n\n");
    
    // 确定输入文件名
    const char *input_file = "kspace_data.bin";
    if (argc > 1) {
        input_file = argv[1];
    }
    
    printf("输入文件: %s\n\n", input_file);
    
    // 加载K空间数据
    double *kspace_real = NULL;
    double *kspace_imag = NULL;
    int width = 0, height = 0;
    
    if (load_kspace_binary(input_file, &kspace_real, &kspace_imag, &width, &height) != 0) {
        printf("加载K空间数据失败！\n");
        return 1;
    }
    
    printf("\n");
    
    // 计算并保存K空间幅度谱 (用于可视化)
    double *magnitude = (double *)malloc(width * height * sizeof(double));
    if (magnitude) {
        for (int i = 0; i < width * height; i++) {
            magnitude[i] = sqrt(kspace_real[i] * kspace_real[i] + 
                               kspace_imag[i] * kspace_imag[i]);
        }
        
        // 对幅度谱进行中心化
        fft_shift(magnitude, width, height);
        
        // 保存对数幅度谱
        double *log_magnitude = (double *)malloc(width * height * sizeof(double));
        if (log_magnitude) {
            for (int i = 0; i < width * height; i++) {
                log_magnitude[i] = log(1.0 + magnitude[i]);
            }
            save_bmp_grayscale("kspace_magnitude_spectrum.bmp", log_magnitude, width, height);
            free(log_magnitude);
        }
        
        free(magnitude);
    }
    
    // 分配还原图像的内存
    double *image_real = (double *)malloc(width * height * sizeof(double));
    double *image_imag = (double *)malloc(width * height * sizeof(double));
    
    if (!image_real || !image_imag) {
        printf("内存分配失败\n");
        free(kspace_real);
        free(kspace_imag);
        return 1;
    }
    
    // 执行2D逆傅里叶变换
    calculate_2d_idft(kspace_real, kspace_imag, height, width, image_real, image_imag);
    
    printf("\n");
    
    // 保存还原的图像 (实部)
    save_bmp_grayscale("reconstructed_image.bmp", image_real, width, height);
    
    // 分析虚部 (理论上应该接近0)
    double max_imag = 0.0, avg_imag = 0.0;
    for (int i = 0; i < width * height; i++) {
        double abs_imag = fabs(image_imag[i]);
        if (abs_imag > max_imag) max_imag = abs_imag;
        avg_imag += abs_imag;
    }
    avg_imag /= (width * height);
    
    printf("\n=================================================\n");
    printf("  图像还原统计\n");
    printf("=================================================\n");
    printf("虚部分析 (理论上应接近0):\n");
    printf("  最大虚部: %.6e\n", max_imag);
    printf("  平均虚部: %.6e\n", avg_imag);
    
    // 显示还原图像的统计信息
    double min_real = image_real[0], max_real = image_real[0];
    double sum_real = 0.0;
    for (int i = 0; i < width * height; i++) {
        if (image_real[i] < min_real) min_real = image_real[i];
        if (image_real[i] > max_real) max_real = image_real[i];
        sum_real += image_real[i];
    }
    double avg_real = sum_real / (width * height);
    
    printf("\n还原图像统计:\n");
    printf("  最小值: %.6f\n", min_real);
    printf("  最大值: %.6f\n", max_real);
    printf("  平均值: %.6f\n", avg_real);
    
    // 显示还原图像的一小部分
    printf("\n还原图像 (左上角 8x8 区域):\n");
    for (int i = 0; i < 8 && i < height; i++) {
        for (int j = 0; j < 8 && j < width; j++) {
            printf("%6.2f ", image_real[i * width + j]);
        }
        printf("\n");
    }
    
    printf("\n=================================================\n");
    printf("输出文件:\n");
    printf("  - kspace_magnitude_spectrum.bmp  (K空间幅度谱)\n");
    printf("  - reconstructed_image.bmp        (还原图像)\n");
    printf("=================================================\n");
    
    // 释放内存
    free(kspace_real);
    free(kspace_imag);
    free(image_real);
    free(image_imag);
    
    return 0;
}
