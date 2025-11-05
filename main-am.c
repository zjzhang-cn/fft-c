/**
 * @file main-am.c
 * @brief AM调幅信号生成与解调器
 * 
 * 实现幅度调制(AM)信号的生成和解调：
 * - AM调制：载波幅度随调制信号变化
 * - 包络检波解调：提取调制信号
 * - 相干解调：同步检波方法
 * 
 * AM信号公式：s(t) = [A + m(t)] * cos(2π*fc*t)
 * 其中：A为载波幅度，m(t)为调制信号，fc为载波频率
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 生成AM调制信号
 * 
 * @param t 时间点数组（输出）
 * @param modulating 调制信号数组（输出）
 * @param carrier 载波信号数组（输出）
 * @param am_signal AM调制信号数组（输出）
 * @param n 采样点数
 * @param fc 载波频率 (Hz)
 * @param fm 调制信号频率 (Hz)
 * @param fs 采样频率 (Hz)
 * @param modulation_index 调制指数 (0-1，通常取0.5-0.8)
 */
void generate_am_signal(double *t, double *modulating, double *carrier, 
                        double *am_signal, int n, 
                        double fc, double fm, double fs, double modulation_index) {
    // 调制指数检查
    if (modulation_index > 1.0) {
        printf("警告: 调制指数 %.2f > 1.0，会产生过调制失真\n", modulation_index);
    }
    
    for (int i = 0; i < n; i++) {
        t[i] = i / fs;
        
        // 调制信号（可以是任意基带信号，这里使用简单的正弦波）
        modulating[i] = cos(2.0 * M_PI * fm * t[i]);
        
        // 载波信号
        carrier[i] = cos(2.0 * M_PI * fc * t[i]);
        
        // AM信号：s(t) = [A + m*m(t)] * cos(2π*fc*t)
        // 归一化后：s(t) = [1 + μ*m(t)] * cos(2π*fc*t)
        // 其中 μ 是调制指数
        am_signal[i] = (1.0 + modulation_index * modulating[i]) * carrier[i];
    }
    
    printf("AM调制信号参数:\n");
    printf("  载波频率 fc = %.2f Hz\n", fc);
    printf("  调制频率 fm = %.2f Hz\n", fm);
    printf("  采样频率 fs = %.2f Hz\n", fs);
    printf("  调制指数 μ = %.2f\n", modulation_index);
    printf("  调制度 = %.1f%%\n", modulation_index * 100);
    
    // 计算带宽（根据AM信号特性）
    double bandwidth = 2.0 * fm;
    printf("  信号带宽 = %.2f Hz\n", bandwidth);
}

/**
 * @brief 生成复杂调制信号（多频率叠加）
 * 
 * @param t 时间点数组（输出）
 * @param modulating 调制信号数组（输出）
 * @param carrier 载波信号数组（输出）
 * @param am_signal AM调制信号数组（输出）
 * @param n 采样点数
 * @param fc 载波频率 (Hz)
 * @param fs 采样频率 (Hz)
 * @param modulation_index 调制指数
 */
void generate_am_signal_complex(double *t, double *modulating, double *carrier, 
                                double *am_signal, int n, 
                                double fc, double fs, double modulation_index) {
    // 多频率调制信号（模拟语音或音乐）
    double freq1 = 300.0;  // 基频
    double freq2 = 500.0;  // 二次谐波
    double freq3 = 800.0;  // 三次谐波
    
    for (int i = 0; i < n; i++) {
        t[i] = i / fs;
        
        // 复杂调制信号（三个频率分量的叠加）
        modulating[i] = 0.5 * cos(2.0 * M_PI * freq1 * t[i]) +
                       0.3 * cos(2.0 * M_PI * freq2 * t[i]) +
                       0.2 * cos(2.0 * M_PI * freq3 * t[i]);
        
        // 归一化到 [-1, 1]
        modulating[i] = modulating[i] / 1.0;
        
        // 载波信号
        carrier[i] = cos(2.0 * M_PI * fc * t[i]);
        
        // AM信号
        am_signal[i] = (1.0 + modulation_index * modulating[i]) * carrier[i];
    }
    
    printf("复杂AM调制信号参数:\n");
    printf("  载波频率 fc = %.2f Hz\n", fc);
    printf("  调制信号包含频率: %.0f, %.0f, %.0f Hz\n", freq1, freq2, freq3);
    printf("  采样频率 fs = %.2f Hz\n", fs);
    printf("  调制指数 μ = %.2f\n", modulation_index);
}

/**
 * @brief AM包络检波解调
 * 
 * 使用包络检波器提取AM信号的调制信号
 * 原理：通过整流 + 低通滤波提取包络
 * 
 * @param am_signal 输入的AM信号
 * @param demod_signal 输出的解调信号
 * @param n 采样点数
 * @param fs 采样频率
 */
void am_demodulate_envelope(double *am_signal, double *demod_signal, int n, double fs) {
    // 方法1：简单的包络检波（全波整流 + 低通滤波）
    
    // 步骤1：全波整流
    double *rectified = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        rectified[i] = fabs(am_signal[i]);
    }
    
    // 步骤2：低通滤波器（简单移动平均滤波器）
    // 滤波器窗口大小应该足够去除载波频率，但保留调制信号
    int window_size = (int)(fs / 1000.0);  // 根据采样率自适应
    if (window_size < 3) window_size = 3;
    if (window_size % 2 == 0) window_size++;  // 确保为奇数
    
    int half_window = window_size / 2;
    
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        int count = 0;
        
        for (int j = -half_window; j <= half_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < n) {
                sum += rectified[idx];
                count++;
            }
        }
        
        demod_signal[i] = sum / count;
    }
    
    // 步骤3：去除直流分量（减去平均值）
    double dc_offset = 0.0;
    for (int i = 0; i < n; i++) {
        dc_offset += demod_signal[i];
    }
    dc_offset /= n;
    
    for (int i = 0; i < n; i++) {
        demod_signal[i] -= dc_offset;
    }
    
    free(rectified);
    printf("包络检波解调完成（窗口大小=%d）\n", window_size);
}

/**
 * @brief AM包络检波解调（改进版 - 使用Hilbert变换近似）
 * 
 * @param am_signal 输入的AM信号
 * @param demod_signal 输出的解调信号
 * @param n 采样点数
 * @param fs 采样频率
 */
void am_demodulate_envelope_hilbert(double *am_signal, double *demod_signal, 
                                    int n, double fs) {
    // 使用解析信号方法计算包络
    // 包络 = sqrt(I^2 + Q^2)
    // 其中 I 是原信号，Q 是 Hilbert 变换
    
    // 简化方法：使用局部最大值插值
    for (int i = 1; i < n - 1; i++) {
        // 计算包络（使用相邻样本的平方和的平方根近似）
        double val = am_signal[i] * am_signal[i];
        double deriv = (am_signal[i+1] - am_signal[i-1]) / 2.0;
        demod_signal[i] = sqrt(val + deriv * deriv);
    }
    
    demod_signal[0] = demod_signal[1];
    demod_signal[n-1] = demod_signal[n-2];
    
    // 低通滤波平滑
    int window = 5;
    double *temp = (double *)malloc(n * sizeof(double));
    memcpy(temp, demod_signal, n * sizeof(double));
    
    for (int i = window; i < n - window; i++) {
        double sum = 0.0;
        for (int j = -window; j <= window; j++) {
            sum += temp[i + j];
        }
        demod_signal[i] = sum / (2 * window + 1);
    }
    
    // 去直流
    double dc = 0.0;
    for (int i = 0; i < n; i++) dc += demod_signal[i];
    dc /= n;
    for (int i = 0; i < n; i++) demod_signal[i] -= dc;
    
    free(temp);
    printf("Hilbert变换包络检波解调完成\n");
}

/**
 * @brief AM相干解调（同步检波）
 * 
 * 使用本地载波与接收信号相乘，然后低通滤波
 * 需要载波同步（频率和相位）
 * 
 * @param am_signal 输入的AM信号
 * @param demod_signal 输出的解调信号
 * @param n 采样点数
 * @param fc 载波频率
 * @param fs 采样频率
 * @param phase_offset 相位偏移（用于模拟相位误差）
 */
void am_demodulate_coherent(double *am_signal, double *demod_signal, 
                           int n, double fc, double fs, double phase_offset) {
    // 步骤1：生成本地载波
    double *local_carrier = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        double t = i / fs;
        local_carrier[i] = 2.0 * cos(2.0 * M_PI * fc * t + phase_offset);
    }
    
    // 步骤2：混频（相乘）
    double *mixed = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        mixed[i] = am_signal[i] * local_carrier[i];
    }
    
    // 步骤3：低通滤波（去除2fc成分）
    int window_size = (int)(fs / (fc / 10.0));
    if (window_size < 5) window_size = 5;
    if (window_size % 2 == 0) window_size++;
    
    int half_window = window_size / 2;
    
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        int count = 0;
        
        for (int j = -half_window; j <= half_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < n) {
                sum += mixed[idx];
                count++;
            }
        }
        
        demod_signal[i] = sum / count;
    }
    
    // 去直流
    double dc = 0.0;
    for (int i = 0; i < n; i++) dc += demod_signal[i];
    dc /= n;
    for (int i = 0; i < n; i++) demod_signal[i] -= dc;
    
    free(local_carrier);
    free(mixed);
    printf("相干解调完成（相位偏移=%.2f°，窗口大小=%d）\n", 
           phase_offset * 180.0 / M_PI, window_size);
}

/**
 * @brief 计算信号的信噪比（SNR）
 */
double calculate_snr(double *original, double *recovered, int n) {
    double signal_power = 0.0;
    double noise_power = 0.0;
    
    for (int i = 0; i < n; i++) {
        signal_power += original[i] * original[i];
        double error = original[i] - recovered[i];
        noise_power += error * error;
    }
    
    signal_power /= n;
    noise_power /= n;
    
    if (noise_power < 1e-10) return 100.0;  // 避免除零
    
    double snr = 10.0 * log10(signal_power / noise_power);
    return snr;
}

/**
 * @brief 保存信号到文本文件
 */
void save_signal_to_file(const char *filename, double *t, double *signal, int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建文件: %s\n", filename);
        return;
    }
    
    fprintf(fp, "# Time(s)\tSignal\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%.6f\t%.6f\n", t[i], signal[i]);
    }
    
    fclose(fp);
    printf("信号已保存到: %s\n", filename);
}

/**
 * @brief 保存多列信号到CSV文件
 */
void save_signals_to_csv(const char *filename, double *t, 
                         double *sig1, double *sig2, double *sig3, double *sig4,
                         const char *name1, const char *name2, 
                         const char *name3, const char *name4,
                         int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建文件: %s\n", filename);
        return;
    }
    
    // 写入表头
    fprintf(fp, "Time,%s,%s,%s,%s\n", name1, name2, name3, name4);
    
    // 写入数据
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f\n", 
                t[i], sig1[i], sig2[i], sig3[i], sig4[i]);
    }
    
    fclose(fp);
    printf("CSV数据已保存到: %s\n", filename);
}

/**
 * @brief 保存解调结果到文件
 */
void save_demodulated_signals(const char *filename, double *t,
                              double *original, double *envelope, 
                              double *hilbert, double *coherent, int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建文件: %s\n", filename);
        return;
    }
    
    fprintf(fp, "Time,Original,Envelope,Hilbert,Coherent\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f\n", 
                t[i], original[i], envelope[i], hilbert[i], coherent[i]);
    }
    
    fclose(fp);
    printf("解调结果已保存到: %s\n", filename);
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[]) {
    printf("=== AM调幅信号生成与解调系统 ===\n\n");
    
    // 信号参数
    double fc = 10000.0;     // 载波频率 10kHz
    double fm = 1000.0;      // 调制信号频率 1kHz
    double fs = 100000.0;    // 采样频率 100kHz（满足Nyquist定理）
    double duration = 0.01;  // 信号持续时间 10ms
    double modulation_index = 0.8;  // 调制指数 80%
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-fc") == 0 && i + 1 < argc) {
            fc = atof(argv[++i]);
        } else if (strcmp(argv[i], "-fm") == 0 && i + 1 < argc) {
            fm = atof(argv[++i]);
        } else if (strcmp(argv[i], "-fs") == 0 && i + 1 < argc) {
            fs = atof(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            duration = atof(argv[++i]);
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            modulation_index = atof(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("用法: %s [选项]\n", argv[0]);
            printf("选项:\n");
            printf("  -fc <Hz>    载波频率 (默认: 10000)\n");
            printf("  -fm <Hz>    调制频率 (默认: 1000)\n");
            printf("  -fs <Hz>    采样频率 (默认: 100000)\n");
            printf("  -d <秒>     持续时间 (默认: 0.01)\n");
            printf("  -m <0-1>    调制指数 (默认: 0.8)\n");
            printf("  -h          显示帮助\n");
            return 0;
        }
    }
    
    // 计算采样点数
    int n = (int)(duration * fs);
    
    printf("系统参数:\n");
    printf("  采样点数: %d\n", n);
    printf("  持续时间: %.3f ms\n\n", duration * 1000);
    
    // 分配内存
    double *t = (double *)malloc(n * sizeof(double));
    double *modulating = (double *)malloc(n * sizeof(double));
    double *carrier = (double *)malloc(n * sizeof(double));
    double *am_signal = (double *)malloc(n * sizeof(double));
    double *demod_envelope = (double *)malloc(n * sizeof(double));
    double *demod_hilbert = (double *)malloc(n * sizeof(double));
    double *demod_coherent = (double *)malloc(n * sizeof(double));
    
    if (!t || !modulating || !carrier || !am_signal || 
        !demod_envelope || !demod_hilbert || !demod_coherent) {
        fprintf(stderr, "内存分配失败\n");
        return 1;
    }
    
    // 生成AM信号
    printf("\n--- 步骤1: 生成AM调制信号 ---\n");
    generate_am_signal(t, modulating, carrier, am_signal, n, 
                      fc, fm, fs, modulation_index);
    
    // 保存AM信号
    save_signal_to_file("am_signal.txt", t, am_signal, n);
    save_signals_to_csv("am_signal.csv", t, modulating, carrier, am_signal, am_signal,
                       "Modulating", "Carrier", "AM_Signal", "AM_Signal", n);
    
    // AM解调 - 包络检波
    printf("\n--- 步骤2: AM解调（包络检波法）---\n");
    am_demodulate_envelope(am_signal, demod_envelope, n, fs);
    
    // AM解调 - Hilbert变换
    printf("\n--- 步骤3: AM解调（Hilbert变换法）---\n");
    am_demodulate_envelope_hilbert(am_signal, demod_hilbert, n, fs);
    
    // AM解调 - 相干解调
    printf("\n--- 步骤4: AM解调（相干解调法）---\n");
    double phase_offset = 0.0;  // 理想情况：无相位偏移
    am_demodulate_coherent(am_signal, demod_coherent, n, fc, fs, phase_offset);
    
    // 保存解调结果
    save_demodulated_signals("am_demodulated.csv", t, modulating, 
                            demod_envelope, demod_hilbert, demod_coherent, n);
    
    // 计算各种解调方法的SNR
    printf("\n--- 步骤5: 性能评估 ---\n");
    double snr_envelope = calculate_snr(modulating, demod_envelope, n);
    double snr_hilbert = calculate_snr(modulating, demod_hilbert, n);
    double snr_coherent = calculate_snr(modulating, demod_coherent, n);
    
    printf("解调性能对比:\n");
    printf("  包络检波法 SNR = %.2f dB\n", snr_envelope);
    printf("  Hilbert变换法 SNR = %.2f dB\n", snr_hilbert);
    printf("  相干解调法 SNR = %.2f dB\n", snr_coherent);
    
    // 释放内存
    free(t);
    free(modulating);
    free(carrier);
    free(am_signal);
    free(demod_envelope);
    free(demod_hilbert);
    free(demod_coherent);
    
    printf("\n=== 处理完成 ===\n");
    printf("生成的文件:\n");
    printf("  - am_signal.txt: AM信号数据\n");
    printf("  - am_signal.csv: AM信号CSV格式\n");
    printf("  - am_demodulated.csv: 解调结果对比\n");
    
    return 0;
}
