/**
 * @file envelope_detector.c
 * @brief 包络检波电路的 C 语言模拟（二极管 + RC 低通滤波器）
 * 
 * 实现真实的包络检波电路模拟，包括：
 * - 二极管整流特性（非线性器件）
 * - RC 低通滤波器（充放电过程）
 * - 电路参数优化
 * 
 * 电路结构：
 *   AM信号 → 二极管 → RC滤波器 → 输出
 *                     ┌─R─┐
 *                     │   │
 *                     C   负载
 *                     │   │
 *                     └───┘
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 二极管特性模型（指数模型）
 * 
 * 使用肖克利方程（Shockley equation）模拟二极管的 I-V 特性：
 * I = Is * (exp(V/(n*Vt)) - 1)
 * 
 * @param v_in 输入电压 (V)
 * @param Is 反向饱和电流 (A)，典型值 1e-12
 * @param n 理想因子，典型值 1-2
 * @param Vt 热电压，室温下约 26mV
 * @return 二极管电流 (A)
 */
double diode_current(double v_in, double Is, double n, double Vt) {
    if (v_in > 0.7) {
        // 防止指数溢出，高电压时使用线性近似
        return Is * exp(0.7 / (n * Vt)) * (1.0 + (v_in - 0.7) / (n * Vt));
    } else if (v_in < -5.0) {
        // 反向偏置截止
        return -Is;
    } else {
        return Is * (exp(v_in / (n * Vt)) - 1.0);
    }
}

/**
 * @brief 简化的二极管模型（分段线性）
 * 
 * 更简单高效的模型：
 * - 正向偏置（V > Vd）：导通，V_out = V_in - Vd
 * - 反向偏置（V < 0）：截止，V_out = 0
 * 
 * @param v_in 输入电压 (V)
 * @param Vd 二极管导通电压，典型值 0.6-0.7V
 * @return 输出电压 (V)
 */
double diode_rectifier(double v_in, double Vd) {
    if (v_in > Vd) {
        return v_in - Vd;  // 正向导通，压降 Vd
    } else {
        return 0.0;  // 反向截止或未导通
    }
}

/**
 * @brief RC 低通滤波器（一阶）
 * 
 * 电路微分方程：
 * RC * dV_out/dt = V_in - V_out
 * 
 * 离散化（一阶欧拉法）：
 * V_out[n] = V_out[n-1] + (dt/(RC)) * (V_in[n] - V_out[n-1])
 * 
 * 或等效为：
 * V_out[n] = α * V_in[n] + (1-α) * V_out[n-1]
 * 其中 α = dt/(RC + dt)
 * 
 * @param v_in 输入电压数组
 * @param v_out 输出电压数组
 * @param n 采样点数
 * @param R 电阻值 (Ω)
 * @param C 电容值 (F)
 * @param fs 采样频率 (Hz)
 */
void rc_lowpass_filter(double *v_in, double *v_out, int n, 
                       double R, double C, double fs) {
    double dt = 1.0 / fs;  // 采样间隔
    double tau = R * C;     // 时间常数
    double alpha = dt / (tau + dt);  // 滤波系数
    
    // 初始条件
    v_out[0] = v_in[0];
    
    // 递推计算
    for (int i = 1; i < n; i++) {
        v_out[i] = alpha * v_in[i] + (1.0 - alpha) * v_out[i - 1];
    }
    
    // 计算截止频率
    double fc = 1.0 / (2.0 * M_PI * tau);
    printf("RC滤波器参数:\n");
    printf("  R = %.0f Ω\n", R);
    printf("  C = %.2e F (%.2f μF)\n", C, C * 1e6);
    printf("  时间常数 τ = RC = %.2e s (%.2f ms)\n", tau, tau * 1000);
    printf("  截止频率 fc = 1/(2πRC) = %.2f Hz\n", fc);
    printf("  滤波系数 α = %.6f\n", alpha);
}

/**
 * @brief 改进的 RC 滤波器（使用梯形积分法，更精确）
 * 
 * 使用梯形法离散化：
 * V_out[n] - V_out[n-1] = (dt/(2RC)) * (V_in[n] + V_in[n-1] - V_out[n] - V_out[n-1])
 * 
 * 整理得：
 * V_out[n] = (a*V_in[n] + a*V_in[n-1] + b*V_out[n-1]) / (1 + a)
 * 其中 a = dt/(2RC), b = 1 - a
 * 
 * @param v_in 输入电压数组
 * @param v_out 输出电压数组
 * @param n 采样点数
 * @param R 电阻值 (Ω)
 * @param C 电容值 (F)
 * @param fs 采样频率 (Hz)
 */
void rc_lowpass_filter_trapezoidal(double *v_in, double *v_out, int n, 
                                    double R, double C, double fs) {
    double dt = 1.0 / fs;
    double tau = R * C;
    double a = dt / (2.0 * tau);
    
    // 初始条件
    v_out[0] = v_in[0];
    
    // 梯形法递推
    for (int i = 1; i < n; i++) {
        v_out[i] = (a * v_in[i] + a * v_in[i-1] + (1.0 - a) * v_out[i-1]) / (1.0 + a);
    }
}

/**
 * @brief 包络检波器（二极管 + RC 滤波器）
 * 
 * 完整的包络检波电路模拟，包括：
 * 1. 二极管整流
 * 2. RC 低通滤波（提取包络）
 * 3. 直流分量去除
 * 
 * @param am_signal 输入的 AM 调制信号
 * @param demod_signal 输出的解调信号
 * @param n 采样点数
 * @param fs 采样频率 (Hz)
 * @param R 电阻值 (Ω)，建议 10kΩ - 100kΩ
 * @param C 电容值 (F)，建议 0.1μF - 10μF
 * @param Vd 二极管导通电压 (V)，典型 0.6-0.7V
 */
void envelope_detector(double *am_signal, double *demod_signal, int n,
                       double fs, double R, double C, double Vd) {
    printf("\n=== 包络检波器电路模拟 ===\n");
    
    // 步骤 1: 二极管整流
    double *rectified = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        rectified[i] = diode_rectifier(am_signal[i], Vd);
    }
    printf("✓ 二极管整流完成（Vd = %.2f V）\n", Vd);
    
    // 步骤 2: RC 低通滤波
    rc_lowpass_filter(rectified, demod_signal, n, R, C, fs);
    printf("✓ RC 低通滤波完成\n");
    
    // 步骤 3: 去除直流分量
    double dc_offset = 0.0;
    for (int i = 0; i < n; i++) {
        dc_offset += demod_signal[i];
    }
    dc_offset /= n;
    
    for (int i = 0; i < n; i++) {
        demod_signal[i] -= dc_offset;
    }
    printf("✓ 直流分量去除完成（DC = %.4f V）\n", dc_offset);
    
    free(rectified);
    printf("=========================\n\n");
}

/**
 * @brief 包络检波器（改进版，使用梯形法）
 */
void envelope_detector_improved(double *am_signal, double *demod_signal, int n,
                                double fs, double R, double C, double Vd) {
    printf("\n=== 包络检波器电路模拟（改进版）===\n");
    
    // 步骤 1: 二极管整流
    double *rectified = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        rectified[i] = diode_rectifier(am_signal[i], Vd);
    }
    printf("✓ 二极管整流完成（Vd = %.2f V）\n", Vd);
    
    // 步骤 2: RC 低通滤波（梯形法）
    rc_lowpass_filter_trapezoidal(rectified, demod_signal, n, R, C, fs);
    printf("✓ RC 低通滤波完成（梯形积分法）\n");
    
    // 步骤 3: 去除直流分量
    double dc_offset = 0.0;
    for (int i = 0; i < n; i++) {
        dc_offset += demod_signal[i];
    }
    dc_offset /= n;
    
    for (int i = 0; i < n; i++) {
        demod_signal[i] -= dc_offset;
    }
    printf("✓ 直流分量去除完成（DC = %.4f V）\n", dc_offset);
    
    free(rectified);
    printf("===================================\n\n");
}

/**
 * @brief 自动计算最佳 RC 参数
 * 
 * 设计准则：
 * 1. 截止频率应远低于载波频率（fc_RC << fc_carrier）
 * 2. 截止频率应高于调制频率（fc_RC > fm_modulating）
 * 3. 典型选择：fc_RC ≈ 5-10 倍调制频率
 * 
 * @param fc_carrier 载波频率 (Hz)
 * @param fm_modulating 调制信号频率 (Hz)
 * @param R_out 输出电阻值 (Ω)
 * @param C_out 输出电容值 (F)
 */
void calculate_optimal_rc(double fc_carrier, double fm_modulating, 
                         double *R_out, double *C_out) {
    // 目标截止频率：约为调制频率的 5-10 倍
    double fc_target = 7.0 * fm_modulating;
    
    // 确保截止频率远低于载波频率
    if (fc_target > fc_carrier / 20.0) {
        fc_target = fc_carrier / 20.0;
    }
    
    // 计算所需时间常数
    double tau = 1.0 / (2.0 * M_PI * fc_target);
    
    // 选择标准电阻值（E12系列）
    double R_standard[] = {1e3, 2.2e3, 3.3e3, 4.7e3, 6.8e3, 
                          10e3, 22e3, 33e3, 47e3, 68e3, 100e3};
    int n_R = sizeof(R_standard) / sizeof(R_standard[0]);
    
    // 选择合适的电阻值（中等阻值，10k-100k）
    double R = 47e3;  // 47kΩ 是常用值
    
    // 计算所需电容值
    double C = tau / R;
    
    // 标准电容值（μF）
    double C_standard[] = {0.1e-6, 0.22e-6, 0.47e-6, 1e-6, 
                          2.2e-6, 4.7e-6, 10e-6};
    int n_C = sizeof(C_standard) / sizeof(C_standard[0]);
    
    // 选择最接近的标准电容值
    double C_selected = C_standard[0];
    double min_diff = fabs(C - C_selected);
    for (int i = 1; i < n_C; i++) {
        double diff = fabs(C - C_standard[i]);
        if (diff < min_diff) {
            min_diff = diff;
            C_selected = C_standard[i];
        }
    }
    
    *R_out = R;
    *C_out = C_selected;
    
    double fc_actual = 1.0 / (2.0 * M_PI * R * C_selected);
    
    printf("\n=== RC 参数计算 ===\n");
    printf("载波频率 fc = %.2f Hz\n", fc_carrier);
    printf("调制频率 fm = %.2f Hz\n", fm_modulating);
    printf("目标截止频率 = %.2f Hz\n", fc_target);
    printf("\n推荐参数:\n");
    printf("  R = %.0f Ω (%.0f kΩ)\n", R, R / 1000);
    printf("  C = %.2e F (%.2f μF)\n", C_selected, C_selected * 1e6);
    printf("  实际截止频率 = %.2f Hz\n", fc_actual);
    printf("  τ = RC = %.2e s (%.2f ms)\n", R * C_selected, R * C_selected * 1000);
    printf("==================\n\n");
}

/**
 * @brief 生成简单的 AM 信号用于测试
 */
void generate_test_am_signal(double *t, double *am_signal, int n,
                             double fc, double fm, double fs, double mod_index) {
    for (int i = 0; i < n; i++) {
        t[i] = i / fs;
        double modulating = cos(2.0 * M_PI * fm * t[i]);
        double carrier = cos(2.0 * M_PI * fc * t[i]);
        am_signal[i] = (1.0 + mod_index * modulating) * carrier;
    }
}

/**
 * @brief 保存信号到 CSV 文件
 */
void save_to_csv(const char *filename, double *t, double *signal1, 
                 double *signal2, int n, const char *name1, const char *name2) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    fprintf(fp, "time,%s,%s\n", name1, name2);
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%.8f,%.8f,%.8f\n", t[i], signal1[i], signal2[i]);
    }
    
    fclose(fp);
    printf("数据已保存到 %s\n", filename);
}

/**
 * @brief 主函数 - 包络检波器演示
 */
int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("  包络检波电路模拟器\n");
    printf("  二极管 + RC 低通滤波器\n");
    printf("========================================\n\n");
    
    // 信号参数
    double fc = 10000.0;      // 载波频率 10kHz
    double fm = 500.0;        // 调制频率 500Hz
    double fs = 100000.0;     // 采样频率 100kHz
    double mod_index = 0.8;   // 调制指数
    double duration = 0.01;   // 持续时间 10ms
    
    int n = (int)(duration * fs);
    
    printf("信号参数:\n");
    printf("  载波频率 fc = %.0f Hz\n", fc);
    printf("  调制频率 fm = %.0f Hz\n", fm);
    printf("  采样频率 fs = %.0f Hz\n", fs);
    printf("  调制指数 μ = %.2f\n", mod_index);
    printf("  采样点数 n = %d\n\n", n);
    
    // 分配内存
    double *t = (double *)malloc(n * sizeof(double));
    double *am_signal = (double *)malloc(n * sizeof(double));
    double *demod_basic = (double *)malloc(n * sizeof(double));
    double *demod_improved = (double *)malloc(n * sizeof(double));
    
    // 生成 AM 信号
    printf("生成 AM 信号...\n");
    generate_test_am_signal(t, am_signal, n, fc, fm, fs, mod_index);
    printf("✓ AM 信号生成完成\n\n");
    
    // 计算最佳 RC 参数
    double R_opt, C_opt;
    calculate_optimal_rc(fc, fm, &R_opt, &C_opt);
    
    // 电路参数
    double Vd = 0.7;  // 二极管导通电压
    
    // 方法 1: 基本包络检波器（欧拉法）
    printf("--- 方法 1: 基本包络检波器 ---\n");
    envelope_detector(am_signal, demod_basic, n, fs, R_opt, C_opt, Vd);
    
    // 方法 2: 改进包络检波器（梯形法）
    printf("--- 方法 2: 改进包络检波器 ---\n");
    envelope_detector_improved(am_signal, demod_improved, n, fs, R_opt, C_opt, Vd);
    
    // 保存结果
    save_to_csv("envelope_detector_result.csv", t, am_signal, demod_basic, 
                n, "AM_Signal", "Demodulated");
    
    // 计算解调质量指标
    printf("\n=== 性能分析 ===\n");
    
    // 计算原始调制信号
    double *original_mod = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        original_mod[i] = cos(2.0 * M_PI * fm * t[i]);
    }
    
    // 归一化解调信号
    double max_demod = 0.0;
    for (int i = 0; i < n; i++) {
        if (fabs(demod_improved[i]) > max_demod) {
            max_demod = fabs(demod_improved[i]);
        }
    }
    for (int i = 0; i < n; i++) {
        demod_improved[i] /= max_demod;
    }
    
    // 计算误差
    double mse = 0.0;
    for (int i = 0; i < n; i++) {
        double error = demod_improved[i] - original_mod[i];
        mse += error * error;
    }
    mse /= n;
    double rmse = sqrt(mse);
    
    printf("均方根误差 RMSE = %.6f\n", rmse);
    printf("==================\n\n");
    
    // 测试不同的 RC 参数
    printf("\n=== 测试不同 RC 参数的影响 ===\n");
    
    // 参数组 1: R 太小（时间常数太小，滤波不足）
    printf("\n测试 1: R = 1kΩ, C = 0.1μF (τ = 0.1ms, fc ≈ 1.6kHz)\n");
    envelope_detector(am_signal, demod_basic, n, fs, 1e3, 0.1e-6, Vd);
    
    // 参数组 2: R 太大（时间常数太大，响应慢）
    printf("\n测试 2: R = 1MΩ, C = 1μF (τ = 1s, fc ≈ 0.16Hz)\n");
    envelope_detector(am_signal, demod_basic, n, fs, 1e6, 1e-6, Vd);
    
    // 参数组 3: 最佳参数
    printf("\n测试 3: 最佳参数\n");
    envelope_detector(am_signal, demod_basic, n, fs, R_opt, C_opt, Vd);
    
    // 清理
    free(t);
    free(am_signal);
    free(demod_basic);
    free(demod_improved);
    free(original_mod);
    
    printf("\n包络检波器模拟完成！\n");
    printf("========================================\n");
    
    return 0;
}
