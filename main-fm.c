/**
 * @file main-fm.c
 * @brief 调频信号生成器
 * 
 * 生成调频(FM)信号：载波频率随调制信号线性变化
 * 输出信号到文件并显示参数信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 生成调频信号
 * 
 * @param t 时间点数组
 * @param signal 输出信号数组
 * @param n 采样点数
 * @param fc 载波频率 (Hz)
 * @param fm 调制信号频率 (Hz)
 * @param beta 调制指数（频率偏移/调制频率）
 * @param fs 采样频率 (Hz)
 */
void generate_fm_signal(double *t, double *signal, int n, 
                        double fc, double fm, double beta, double fs) {
    double delta_f = beta * fm;  // 最大频率偏移
    
    for (int i = 0; i < n; i++) {
        t[i] = i / fs;
        // FM信号: s(t) = A*cos(2π*fc*t + β*sin(2π*fm*t))
        // 其中β为调制指数，决定频率偏移量
        signal[i] = cos(2.0 * M_PI * fc * t[i] + 
                       beta * sin(2.0 * M_PI * fm * t[i]));
    }
    
    printf("调频信号参数:\n");
    printf("  载波频率 fc = %.2f Hz\n", fc);
    printf("  调制频率 fm = %.2f Hz\n", fm);
    printf("  调制指数 β = %.2f\n", beta);
    printf("  最大频偏 Δf = %.2f Hz\n", delta_f);
    printf("  带宽估计 (Carson规则) ≈ %.2f Hz\n", 2 * (delta_f + fm));
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
 * @brief 保存信号到CSV文件
 */
void save_signal_to_csv(const char *filename, double *t, double *signal, int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建文件: %s\n", filename);
        return;
    }
    
    fprintf(fp, "Time,Signal\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%.6f,%.6f\n", t[i], signal[i]);
    }
    
    fclose(fp);
    printf("CSV数据已保存到: %s\n", filename);
}

/**
 * @brief FM解调 - 使用瞬时频率法
 * 
 * 通过计算信号的瞬时相位导数来恢复调制信号
 * 
 * @param signal 输入的FM信号
 * @param demod_signal 输出的解调信号
 * @param n 采样点数
 * @param fs 采样频率
 * @param fc 载波频率（用于去除直流分量）
 */
void fm_demodulate(double *signal, double *demod_signal, int n, double fs, double fc) {
    // 使用Hilbert变换的近似方法：通过相邻采样点计算瞬时频率
    // f_inst(t) = (1/2π) * dφ/dt
    
    for (int i = 1; i < n - 1; i++) {
        // 使用数值微分计算相位变化率
        // 通过反正切计算相位差
        double y1 = signal[i];
        double y2 = signal[i + 1];
        double y0 = signal[i - 1];
        
        // 使用中心差分近似计算导数
        double derivative = (y2 - y0) * fs / 2.0;
        
        // 计算瞬时频率（简化方法）
        // 对于小角度，可以用导数除以信号值的近似
        if (fabs(y1) > 1e-6) {
            demod_signal[i] = derivative / (2.0 * M_PI * sqrt(1.0 - y1 * y1 + 1e-10));
        } else {
            demod_signal[i] = 0.0;
        }
    }
    
    // 边界处理
    demod_signal[0] = demod_signal[1];
    demod_signal[n - 1] = demod_signal[n - 2];
    
    // 去除载波频率分量（高通滤波）
    for (int i = 0; i < n; i++) {
        demod_signal[i] -= fc;
    }
}

/**
 * @brief FM解调 - 使用相位鉴频器方法（改进版）
 * 
 * 使用Hilbert变换的离散近似来计算瞬时频率
 * 
 * @param signal 输入的FM信号
 * @param demod_signal 输出的解调信号
 * @param n 采样点数
 * @param fs 采样频率
 * @param fc 载波频率
 */
void fm_demodulate_phase_discriminator(double *signal, double *demod_signal, 
                                       int n, double fs, double fc) {
    // 使用复数解调方法的简化版本
    // 通过构造解析信号并计算相位导数
    
    double *phase = (double *)malloc(n * sizeof(double));
    
    // 计算瞬时相位（使用数值积分近似）
    phase[0] = 0.0;
    for (int i = 1; i < n; i++) {
        // 使用atan2计算相位差
        double dx = signal[i] - signal[i-1];
        
        // 通过差分近似瞬时频率
        // 使用正交分量近似（延迟产生90度相移）
        int delay = (int)(fs / (4.0 * fc));  // 90度延迟
        if (delay < 1) delay = 1;
        
        double in_phase = signal[i];
        double quadrature = (i >= delay) ? signal[i - delay] : 0.0;
        
        // 计算相位
        phase[i] = atan2(quadrature, in_phase);
        
        // 计算瞬时频率（相位对时间的导数）
        if (i > 0) {
            double dphase = phase[i] - phase[i-1];
            
            // 相位解包装
            while (dphase > M_PI) dphase -= 2.0 * M_PI;
            while (dphase < -M_PI) dphase += 2.0 * M_PI;
            
            demod_signal[i] = (dphase * fs) / (2.0 * M_PI);
        }
    }
    
    demod_signal[0] = demod_signal[1];
    
    free(phase);
}

/**
 * @brief FM解调 - 使用解析信号方法（最准确）
 * 
 * 通过Hilbert变换构造解析信号，然后计算瞬时频率
 * 这里使用简化的离散希尔伯特变换
 * 
 * @param signal 输入的FM信号
 * @param demod_signal 输出的解调信号（频率偏移）
 * @param n 采样点数  
 * @param fs 采样频率
 * @param fc 载波频率
 */
void fm_demodulate_analytic(double *signal, double *demod_signal, 
                            int n, double fs, double fc) {
    double *hilbert = (double *)malloc(n * sizeof(double));
    double *phase = (double *)malloc(n * sizeof(double));
    
    // 简化的Hilbert变换（90度相移）
    // 对于余弦信号，Hilbert变换产生正弦信号
    for (int i = 0; i < n; i++) {
        hilbert[i] = 0.0;
        
        // 使用有限长度的希尔伯特变换核
        for (int k = -20; k <= 20; k++) {
            if (k == 0) continue;
            int idx = i + k;
            if (idx >= 0 && idx < n) {
                // Hilbert变换核: h(n) = 2*sin^2(πn/2) / (πn)
                double h = (2.0 / (M_PI * k)) * sin(M_PI * k / 2.0) * sin(M_PI * k / 2.0);
                hilbert[i] += signal[idx] * h;
            }
        }
    }
    
    // 计算瞬时相位
    for (int i = 0; i < n; i++) {
        phase[i] = atan2(hilbert[i], signal[i]);
    }
    
    // 计算瞬时频率（相位的导数）
    for (int i = 1; i < n; i++) {
        double dphase = phase[i] - phase[i-1];
        
        // 相位解包装（unwrapping）
        while (dphase > M_PI) dphase -= 2.0 * M_PI;
        while (dphase < -M_PI) dphase += 2.0 * M_PI;
        
        // 瞬时频率 = (1/2π) * dφ/dt
        double inst_freq = (dphase * fs) / (2.0 * M_PI);
        
        // 减去载波频率得到调制信号
        demod_signal[i] = inst_freq - fc;
    }
    
    demod_signal[0] = demod_signal[1];
    
    free(hilbert);
    free(phase);
}

/**
 * @brief 简单的低通滤波器（移动平均）
 * 
 * @param input 输入信号
 * @param output 输出信号
 * @param n 信号长度
 * @param window_size 滤波窗口大小
 */
void lowpass_filter(double *input, double *output, int n, int window_size) {
    int half_window = window_size / 2;
    
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        int count = 0;
        
        for (int j = -half_window; j <= half_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < n) {
                sum += input[idx];
                count++;
            }
        }
        
        output[i] = sum / count;
    }
}

/**
 * @brief 去除信号的直流分量
 */
void remove_dc(double *signal, int n) {
    double mean = 0.0;
    for (int i = 0; i < n; i++) {
        mean += signal[i];
    }
    mean /= n;
    
    for (int i = 0; i < n; i++) {
        signal[i] -= mean;
    }
    
    printf("  去除直流分量: %.2f Hz\n", mean);
}

/**
 * @brief 计算解调误差
 */
double calculate_demod_error(double *demod_signal, int n, double fm, double beta, double fs) {
    // 理论调制信号：beta * fm * sin(2π * fm * t)
    double error_sum = 0.0;
    double signal_power = 0.0;
    
    // 首先估计实际的缩放因子
    double max_demod = 0.0;
    for (int i = 0; i < n; i++) {
        if (fabs(demod_signal[i]) > max_demod) {
            max_demod = fabs(demod_signal[i]);
        }
    }
    
    double expected_max = beta * fm;
    double scale_factor = (max_demod > 0) ? (expected_max / max_demod) : 1.0;
    
    printf("  理论最大频偏: %.2f Hz\n", expected_max);
    printf("  实际解调幅度: %.2f Hz\n", max_demod);
    printf("  缩放因子: %.4f\n", scale_factor);
    
    for (int i = 0; i < n; i++) {
        double t = i / fs;
        double expected = beta * fm * sin(2.0 * M_PI * fm * t);
        double actual = demod_signal[i] * scale_factor;
        double error = actual - expected;
        error_sum += error * error;
        signal_power += expected * expected;
    }
    
    double mse = error_sum / n;
    double snr = (error_sum > 0) ? 10.0 * log10(signal_power / error_sum) : 100.0;
    
    printf("\n解调性能:\n");
    printf("  均方误差 MSE = %.6f\n", mse);
    printf("  信噪比 SNR = %.2f dB\n", snr);
    
    return mse;
}

/**
 * @brief 计算并显示信号统计信息
 */
void print_signal_stats(double *signal, int n) {
    double max_val = signal[0];
    double min_val = signal[0];
    double sum = 0.0;
    
    for (int i = 0; i < n; i++) {
        if (signal[i] > max_val) max_val = signal[i];
        if (signal[i] < min_val) min_val = signal[i];
        sum += signal[i] * signal[i];
    }
    
    double rms = sqrt(sum / n);
    
    printf("\n信号统计:\n");
    printf("  最大值: %.6f\n", max_val);
    printf("  最小值: %.6f\n", min_val);
    printf("  RMS值: %.6f\n", rms);
    printf("  采样点数: %d\n", n);
}

int main() {
    // 信号参数设置
    double fs = 8000.0;        // 采样频率 8kHz
    double duration = 0.1;     // 信号持续时间 100ms
    double fc = 1000.0;        // 载波频率 1kHz
    double fm = 100.0;         // 调制信号频率 100Hz
    double beta = 5.0;         // 调制指数
    
    int n = (int)(fs * duration);  // 采样点数
    
    printf("=== 调频(FM)信号生成与解调系统 ===\n\n");
    printf("采样设置:\n");
    printf("  采样频率 fs = %.0f Hz\n", fs);
    printf("  信号时长 = %.3f s\n", duration);
    printf("  采样点数 = %d\n\n", n);
    
    // 分配内存
    double *t = (double *)malloc(n * sizeof(double));
    double *signal = (double *)malloc(n * sizeof(double));
    double *demod_signal = (double *)malloc(n * sizeof(double));
    double *demod_filtered = (double *)malloc(n * sizeof(double));
    double *original_modulating = (double *)malloc(n * sizeof(double));
    
    if (!t || !signal || !demod_signal || !demod_filtered || !original_modulating) {
        fprintf(stderr, "内存分配失败!\n");
        free(t);
        free(signal);
        free(demod_signal);
        free(demod_filtered);
        free(original_modulating);
        return 1;
    }
    
    // 生成调频信号
    generate_fm_signal(t, signal, n, fc, fm, beta, fs);
    
    // 保存原始FM信号
    save_signal_to_file("fm_signal.txt", t, signal, n);
    save_signal_to_csv("fm_signal.csv", t, signal, n);
    
    // 显示FM信号统计信息
    printf("\n=== 原始FM信号 ===");
    print_signal_stats(signal, n);
    
    // 执行FM解调
    printf("\n=== 执行FM解调 ===\n");
    printf("使用解析信号方法（Hilbert变换）...\n");
    fm_demodulate_analytic(signal, demod_signal, n, fs, fc);
    
    // 去除直流分量
    remove_dc(demod_signal, n);
    
    // 对解调信号进行低通滤波
    int filter_window = 15;  // 滤波窗口大小
    printf("应用低通滤波器 (窗口大小 = %d)...\n", filter_window);
    lowpass_filter(demod_signal, demod_filtered, n, filter_window);
    
    // 生成原始调制信号用于对比
    for (int i = 0; i < n; i++) {
        original_modulating[i] = beta * fm * sin(2.0 * M_PI * fm * t[i]);
    }
    
    // 保存解调信号
    save_signal_to_file("fm_demodulated.txt", t, demod_filtered, n);
    save_signal_to_csv("fm_demodulated.csv", t, demod_filtered, n);
    
    // 保存原始调制信号用于对比
    save_signal_to_file("fm_original_modulating.txt", t, original_modulating, n);
    
    // 显示解调信号统计
    printf("\n=== 解调后信号 ===");
    print_signal_stats(demod_filtered, n);
    
    // 计算解调误差
    calculate_demod_error(demod_filtered, n, fm, beta, fs);
    
    printf("\n=== 生成完成! ===\n");
    printf("\n输出文件:\n");
    printf("  fm_signal.txt/csv - 原始FM调制信号\n");
    printf("  fm_demodulated.txt/csv - 解调后的信号\n");
    printf("  fm_original_modulating.txt - 原始调制信号（理论值）\n");
    
    printf("\n可视化提示:\n");
    printf("  使用gnuplot对比:\n");
    printf("    gnuplot -e \"plot 'fm_signal.txt' with lines title 'FM Signal'\"\n");
    printf("    gnuplot -e \"plot 'fm_demodulated.txt' with lines title 'Demodulated', ");
    printf("'fm_original_modulating.txt' with lines title 'Original'\"\n");
    
    // 释放内存
    free(t);
    free(signal);
    free(demod_signal);
    free(demod_filtered);
    free(original_modulating);
    
    return 0;
}
