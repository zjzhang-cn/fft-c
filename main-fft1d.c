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
 * 计算离散傅里叶逆变换 (IDFT)
 * @param X_real 输入频域信号的实部数组
 * @param X_imag 输入频域信号的虚部数组
 * @param N 信号长度
 * @param x 输出时域信号数组
 */
void calculate_idft(double* X_real, double* X_imag, int N, double* x) {
    for (int n = 0; n < N; n++) {
        x[n] = 0.0;
        for (int k = 0; k < N; k++) {
            double angle = 2.0 * M_PI * k * n / N;
            // IDFT: 注意符号与DFT相反 (+ 代替 -)
            x[n] += X_real[k] * cos(angle) - X_imag[k] * sin(angle);
        }
        // 归一化
        x[n] /= N;
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
    int N = 32; // 采样点数量
    double fs = 32.0; // 采样频率 (Hz)
    
    // 第一个正弦波参数
    double f1 = 2.0; // 信号频率 2Hz
    double phase1 = M_PI / 4.0; // 初始相位 45度 (π/4 弧度)
    
    // 第二个正弦波参数
    double f2 = 7.0; // 信号频率 7Hz
    double phase2 = M_PI / 2.0; // 初始相位 90度 (π/2 弧度)

    // 动态生成两个正弦波的叠加信号
    double *x = (double *)malloc(N * sizeof(double));
    if (!x) {
        printf("内存分配失败\n");
        return 1;
    }
    
    // 生成叠加信号: x[n] = sin(2*π*f1*n/fs + phase1) + sin(2*π*f2*n/fs + phase2)
    for (int n = 0; n < N; n++) {
        x[n] = sin(2.0 * M_PI * f1 * n / fs + phase1) + 
               sin(2.0 * M_PI * f2 * n / fs + phase2);
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

    // 输出频域结果
    printf("信号: %.1f Hz 正弦波 (初始相位: 45度) + %.1f Hz 正弦波 (初始相位: 90度)\n", f1, f2);
    printf("采样频率: %.1f Hz\n", fs);
    printf("信号长度 N = %d\n\n", N);
    printf("=== DFT 结果 ===\n");
    printf("频率点 k | 频率 (Hz) | 幅度谱 Mag[k] | 相位谱 Phase[k] (度)\n");
    printf("--------------------------------------------------------------------\n");
    for (int k = 0; k < N; k++) {
        double freq = k * fs / N; // 对应的实际频率
        double phase_deg = phase[k] * 180.0 / M_PI; // 将弧度转换为度数
        printf("%8d | %10.2f | %14.4f | %20.2f\n", k, freq, magnitude[k], phase_deg);
    }

    // 3. 执行 IDFT 还原信号
    double *x_restored = (double *)malloc(N * sizeof(double));
    if (!x_restored) {
        printf("内存分配失败\n");
        free(x);
        free(X_real);
        free(X_imag);
        free(magnitude);
        free(phase);
        return 1;
    }

    calculate_idft(X_real, X_imag, N, x_restored);

    // 4. 计算还原误差
    double max_error = 0.0;
    double avg_error = 0.0;
    printf("\n=== IDFT 还原结果 ===\n");
    printf("采样点 n | 原始信号 x[n] | 还原信号 x'[n] | 误差\n");
    printf("------------------------------------------------------------\n");
    for (int n = 0; n < N; n++) {
        double error = fabs(x[n] - x_restored[n]);
        avg_error += error;
        if (error > max_error) {
            max_error = error;
        }
        printf("%8d | %15.10f | %16.10f | %.3e\n", n, x[n], x_restored[n], error);
    }
    avg_error /= N;

    printf("\n=== 还原精度统计 ===\n");
    printf("最大误差: %.10e\n", max_error);
    printf("平均误差: %.10e\n", avg_error);
    printf("还原精度: %.6f%%\n", 100.0 * (1.0 - avg_error));

    // 释放内存
    free(x);
    free(X_real);
    free(X_imag);
    free(magnitude);
    free(phase);
    free(x_restored);

    return 0;
}