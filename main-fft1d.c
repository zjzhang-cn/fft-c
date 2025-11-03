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
    int N = 64; // 采样点数量(增加采样点以更好地表示多个频率)
    double fs = 64.0; // 采样频率 (Hz)
    
    // 定义多个频率成分
    double freqs[] = {5.0, 10.0, 15.0}; // 三个频率: 5Hz, 10Hz, 15Hz
    double amps[] = {1.0, 0.5, 0.3};    // 各频率对应的振幅
    double phases[] = {0.0, M_PI/4.0, M_PI/2.0}; // 各频率的初始相位
    int num_freqs = 3; // 频率成分数量

    // 动态生成复合信号采样数据
    double *x = (double *)malloc(N * sizeof(double));
    if (!x) {
        printf("内存分配失败\n");
        return 1;
    }
    
    // 生成复合信号: x[n] = sum(A_i * sin(2*π*f_i*n/fs + phase_i))
    for (int n = 0; n < N; n++) {
        x[n] = 0.0;
        for (int i = 0; i < num_freqs; i++) {
            x[n] += amps[i] * sin(2.0 * M_PI * freqs[i] * n / fs + phases[i]);
        }
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
    printf("复合信号包含 %d 个频率成分:\n", num_freqs);
    for (int i = 0; i < num_freqs; i++) {
        printf("  频率 %d: %.1f Hz, 振幅: %.2f, 初始相位: %.2f 度\n", 
               i+1, freqs[i], amps[i], phases[i] * 180.0 / M_PI);
    }
    printf("采样频率: %.1f Hz\n", fs);
    printf("信号长度 N = %d\n\n", N);
    printf("频率点 k | 频率 (Hz) | 幅度谱 Mag[k] | 相位谱 Phase[k] (度)\n");
    printf("--------------------------------------------------------------------\n");
    for (int k = 0; k < N/2 + 1; k++) { // 只显示前半部分(正频率)
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