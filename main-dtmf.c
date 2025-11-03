#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * 将double类型音频数据转换为16位PCM格式
 * @param sample 音频样本值 (-1.0 到 1.0)
 * @return 16位PCM值
 */
int16_t double_to_pcm16(double sample) {
    // 限制在 -1.0 到 1.0 范围内
    if (sample > 1.0) sample = 1.0;
    if (sample < -1.0) sample = -1.0;
    // 转换为16位整数
    return (int16_t)(sample * 32767.0);
}

/**
 * 播放音频数据
 * @param x 音频样本数组
 * @param N 样本数量
 * @param fs 采样频率
 * @param device 音频设备 (例如: "hw:0,0" 或 "plughw:1,0", NULL使用默认设备)
 * @return 1表示成功，0表示失败
 */
int play_audio(double* x, int N, double fs __attribute__((unused)), const char* device) {
    char cmd[256];
    
    // 构建aplay命令
    if (device != NULL) {
        // 指定设备
        snprintf(cmd, sizeof(cmd), "aplay -D %s -f S16_LE -r 8000 -c 1 2>/dev/null", device);
    } else {
        // 使用默认设备
        snprintf(cmd, sizeof(cmd), "aplay -f S16_LE -r 8000 -c 1 2>/dev/null");
    }
    
    // 使用管道将数据发送给aplay命令
    FILE *pipe = popen(cmd, "w");
    if (!pipe) {
        printf("错误: 无法打开音频设备。请确保安装了 alsa-utils (aplay命令)\n");
        return 0;
    }
    
    // 转换并写入PCM数据
    for (int n = 0; n < N; n++) {
        int16_t pcm_sample = double_to_pcm16(x[n]);
        fwrite(&pcm_sample, sizeof(int16_t), 1, pipe);
    }
    
    pclose(pipe);
    return 1;
}

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

/**
 * DTMF双音频识别函数
 * @param magnitude 幅度谱数组
 * @param N 信号长度
 * @param fs 采样频率
 * @return 识别出的按键字符，如果无法识别返回 '?'
 */
char detect_dtmf(double* magnitude, int N, double fs) {
    // DTMF标准频率
    double low_freqs[] = {697, 770, 852, 941};   // 低频组
    double high_freqs[] = {1209, 1336, 1477};    // 高频组
    char dtmf_table[4][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}
    };
    
    double freq_resolution = fs / N;
    double threshold = 5.0; // 幅度阈值
    
    // 检测低频分量
    int low_idx = -1;
    double max_low_mag = 0;
    for (int i = 0; i < 4; i++) {
        int k = (int)(low_freqs[i] / freq_resolution + 0.5); // 频率对应的索引
        if (k < N && magnitude[k] > threshold && magnitude[k] > max_low_mag) {
            max_low_mag = magnitude[k];
            low_idx = i;
        }
    }
    
    // 检测高频分量
    int high_idx = -1;
    double max_high_mag = 0;
    for (int i = 0; i < 3; i++) {
        int k = (int)(high_freqs[i] / freq_resolution + 0.5); // 频率对应的索引
        if (k < N && magnitude[k] > threshold && magnitude[k] > max_high_mag) {
            max_high_mag = magnitude[k];
            high_idx = i;
        }
    }
    
    // 如果检测到两个频率分量，返回对应按键
    if (low_idx >= 0 && high_idx >= 0) {
        return dtmf_table[low_idx][high_idx];
    }
    
    return '?'; // 无法识别
}

/**
 * 根据按键获取DTMF频率
 * @param key 按键字符
 * @param f_low 输出低频分量
 * @param f_high 输出高频分量
 * @return 1表示成功，0表示无效按键
 */
int get_dtmf_frequencies(char key, double* f_low, double* f_high) {
    // DTMF标准频率
    double low_freqs[] = {697, 770, 852, 941};   // 低频组
    double high_freqs[] = {1209, 1336, 1477};    // 高频组
    char dtmf_table[4][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}
    };
    
    // 查找按键对应的频率
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            if (dtmf_table[i][j] == key) {
                *f_low = low_freqs[i];
                *f_high = high_freqs[j];
                return 1; // 成功
            }
        }
    }
    
    return 0; // 无效按键
}
int main(int argc, char *argv[]) {
    double duration = 0.5; // 信号时长 500ms (播放需要更长的时间才能听清)
    double fs = 8000.0; // 采样频率 8kHz (DTMF标准采样率)
    int N = (int)(duration * fs); // 采样点数量
    
    // 指定音频输出设备
    // NULL = 默认设备
    // "hw:0,0" = Card 0, Device 0 (HDA Intel PCH - ALC3246 Analog)
    // "plughw:1,0" = Card 1, Device 0 (Jabra EVOLVE 30 II)
    // "hw:0,3" = Card 0, Device 3 (HDMI 0)
    const char* audio_device = "plughw:1,0";  // 使用 Jabra 耳机
    // const char* audio_device = "hw:0,0";   // 使用内置扬声器
    // const char* audio_device = NULL;       // 使用默认设备
    
    // DTMF (双音多频) 信号
    // 从命令行参数获取按键序列
    const char* dtmf_keys = "1";    // 默认按键序列
    
    if (argc > 1) {
        // 使用第一个命令行参数作为DTMF按键序列
        dtmf_keys = argv[1];
    } else {
        printf("用法: %s <DTMF按键序列>\n", argv[0]);
        printf("有效按键: 0-9, *, #\n");
        printf("示例: %s 123      (播放 1-2-3)\n", argv[0]);
        printf("示例: %s \"*123#\"  (播放 *-1-2-3-#)\n", argv[0]);
        printf("未提供参数，使用默认按键 '%s'\n\n", dtmf_keys);
    }
    
    printf("播放DTMF音到设备: %s\n", audio_device ? audio_device : "默认设备");
    printf("按键序列: %s\n\n", dtmf_keys);
    
    // 遍历按键序列，依次播放每个按键
    for (int i = 0; dtmf_keys[i] != '\0'; i++) {
        char dtmf_key = dtmf_keys[i];
        double f_low, f_high;   // 低频和高频分量
        double A = 1.0;         // 幅度
        
        // 根据按键获取对应的DTMF频率
        if (!get_dtmf_frequencies(dtmf_key, &f_low, &f_high)) {
            printf("警告: 跳过无效的DTMF按键 '%c'\n", dtmf_key);
            continue;
        }
        
        /*
         * DTMF频率表：
         *        1209Hz  1336Hz  1477Hz
         * 697Hz    1       2       3
         * 770Hz    4       5       6
         * 852Hz    7       8       9
         * 941Hz    *       0       #
         */
        
        // 动态生成DTMF信号采样数据
        double *x = (double *)malloc(N * sizeof(double));
        if (!x) {
            printf("内存分配失败\n");
            return 1;
        }
        
        // 生成DTMF双音频信号: x[n] = sin(2π*f_low*n/fs) + sin(2π*f_high*n/fs)
        for (int n = 0; n < N; n++) {
            x[n] = A * sin(2.0 * M_PI * f_low * n / fs) +
                   A * sin(2.0 * M_PI * f_high * n / fs);
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

        // 3. 识别DTMF按键
        char detected_key = detect_dtmf(magnitude, N, fs);

        // 4. 播放DTMF音
        printf("[%d/%ld] 播放按键 '%c' (%.0f Hz + %.0f Hz)...", 
               i + 1, strlen(dtmf_keys), dtmf_key, f_low, f_high);
        fflush(stdout);
        
        if (play_audio(x, N, fs, audio_device)) {
            printf(" 完成 [识别: '%c' %s]\n", detected_key, 
                   (detected_key == dtmf_key) ? "✓" : "✗");
        } else {
            printf(" 失败\n");
        }
        
        // 释放当前按键的内存
        free(x);
        free(X_real);
        free(X_imag);
        free(magnitude);
        free(phase);
        
        // 在按键之间添加短暂停顿（100ms）
        if (dtmf_keys[i + 1] != '\0') {
            struct timespec ts = {0, 100000000}; // 100ms = 100,000,000 纳秒
            nanosleep(&ts, NULL);
        }
    }
    
    printf("\n播放完成！\n");
    printf("\nDTMF标准频率表：\n");
    printf("        1209Hz  1336Hz  1477Hz\n");
    printf(" 697Hz    1       2       3\n");
    printf(" 770Hz    4       5       6\n");
    printf(" 852Hz    7       8       9\n");
    printf(" 941Hz    *       0       #\n");

    return 0;
}