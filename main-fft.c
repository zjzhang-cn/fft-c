#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * 计算离散傅里叶变换 (DFT)
 * @param x 输入信号的实部数组
 * @param N 信号长度
 * @param X_real 输出信号频域的实部数组
 * @param X_imag 输出信号频域的虚部数组
 */
void calculate_dft(double* x, int N, double* X_real, double* X_imag) {
    for (int k = 0; k < N; k++) {
        X_real[k] = 0.0;
        X_imag[k] = 0.0;
        for (int n = 0; n < N; n++) {
            double angle = 2.0 * M_PI * k * n / N;
            X_real[k] += x[n] * cos(angle);
            X_imag[k] -= x[n] * sin(angle);
        }
    }
}

/**
 * 计算频谱 (幅度谱) 和相位谱
 * @param X_real 频域实部
 * @param X_imag 频域虚部
 * @param N 信号长度
 * @param magnitude 输出幅度谱数组
 * @param phase 输出相位谱数组 (弧度)
 */
void calculate_spectrum_and_phase(double* X_real, double* X_imag, int N, double* magnitude, double* phase) {
    for (int k = 0; k < N; k++) {
        // 幅度 = sqrt(实部^2 + 虚部^2)
        magnitude[k] = sqrt(X_real[k] * X_real[k] + X_imag[k] * X_imag[k]);

        // 相位 = atan2(虚部, 实部)，范围 (-PI, PI]
        phase[k] = atan2(X_imag[k], X_real[k]);
    }
}
int main() {
    int N = 8; // 采样点数量(增加采样点以更好地表示正弦波)
    double fs = 8.0; // 采样频率 (Hz)
    double f = 2.0; // 信号频率 2Hz
    double phase_shift = M_PI / 4.0; // 初始相位 45度 (π/4 弧度)

    // 动态生成2Hz正弦波采样数据,初始相位45度
    double *x = (double *)malloc(N * sizeof(double));
    if (!x) {
        printf("内存分配失败\n");
        return 1;
    }
    
    // 生成正弦波: x[n] = sin(2*π*f*n/fs + phase_shift)
    for (int n = 0; n < N; n++) {
        x[n] = sin(2.0 * M_PI * f * n / fs + phase_shift);
    }

    // 分配内存用于存储频域结果
    double *X_real = (double *)malloc(N * sizeof(double));
    double *X_imag = (double *)malloc(N * sizeof(double));
    double *magnitude = (double *)malloc(N * sizeof(double));
    double *phase = (double *)malloc(N * sizeof(double));

    if (!X_real || !X_imag || !magnitude || !phase) {
        printf("内存分配失败\n");
        free(x);
        return 1;
    }

    // 1. 执行 DFT
    calculate_dft(x, N, X_real, X_imag);

    // 2. 计算幅度和相位
    calculate_spectrum_and_phase(X_real, X_imag, N, magnitude, phase);

    // 输出结果
    printf("信号: %.1f Hz 正弦波, 初始相位: 45度 (π/4)\n", f);
    printf("采样频率: %.1f Hz\n", fs);
    printf("信号长度 N = %d\n\n", N);
    printf("频率点 k | 频率 (Hz) | 幅度谱 Mag[k] | 相位谱 Phase[k] (度)\n");
    printf("--------------------------------------------------------------------\n");
    for (int k = 0; k < N; k++) {
        double freq = k * fs / N; // 对应的实际频率
        double phase_deg = phase[k] * 180.0 / M_PI; // 将弧度转换为度数
        printf("%8d | %10.2f | %14.4f | %20.2f\n", k, freq, magnitude[k], phase_deg);
    }

    // 释放内存
    free(x);
    free(X_real);
    free(X_imag);
    free(magnitude);
    free(phase);

    return 0;
}