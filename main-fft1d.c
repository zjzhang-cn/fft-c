#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

int main() {
    int M = 16;  // 图像行数
    int N = 16;  // 图像列数
    
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
    
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            x_real[i * N + j] = 
                amp1 * sin(2.0 * M_PI * fx1 * j / N) * sin(2.0 * M_PI * fy1 * i / M) +
                amp2 * sin(2.0 * M_PI * fx2 * j / N) * sin(2.0 * M_PI * fy2 * i / M) +
                amp3 * sin(2.0 * M_PI * fx3 * j / N) * sin(2.0 * M_PI * fy3 * i / M);
            x_imag[i * N + j] = 0.0;  // 输入为实数信号
        }
    }
    
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