#!/usr/bin/env python3
"""
AM调制解调可视化脚本
绘制AM信号的调制过程和解调结果
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

# 设置中文字体支持
plt.rcParams['font.sans-serif'] = ['DejaVu Sans', 'Arial Unicode MS', 'SimHei']
plt.rcParams['axes.unicode_minus'] = False

def plot_am_modulation(csv_file='am_signal.csv', output_prefix='am_modulation'):
    """绘制AM调制过程"""
    
    if not os.path.exists(csv_file):
        print(f"错误: 文件 {csv_file} 不存在")
        return
    
    # 读取数据
    data = pd.read_csv(csv_file)
    
    # 创建图形
    fig, axes = plt.subplots(3, 1, figsize=(14, 10))
    
    # 调制信号
    axes[0].plot(data['Time'] * 1000, data['Modulating'], 'b-', linewidth=1.5)
    axes[0].set_title('Modulating Signal m(t)', fontsize=14, fontweight='bold')
    axes[0].set_ylabel('Amplitude', fontsize=12)
    axes[0].grid(True, alpha=0.3)
    axes[0].set_xlim(data['Time'].min() * 1000, data['Time'].max() * 1000)
    
    # 载波信号
    axes[1].plot(data['Time'] * 1000, data['Carrier'], 'r-', linewidth=0.8, alpha=0.7)
    axes[1].set_title('Carrier Signal c(t)', fontsize=14, fontweight='bold')
    axes[1].set_ylabel('Amplitude', fontsize=12)
    axes[1].grid(True, alpha=0.3)
    axes[1].set_xlim(data['Time'].min() * 1000, data['Time'].max() * 1000)
    
    # AM信号
    axes[2].plot(data['Time'] * 1000, data['AM_Signal'], 'g-', linewidth=0.8)
    # 叠加包络线
    envelope_upper = 1 + 0.8 * data['Modulating']  # 假设调制指数0.8
    envelope_lower = -(1 + 0.8 * data['Modulating'])
    axes[2].plot(data['Time'] * 1000, envelope_upper, 'k--', 
                linewidth=1.5, alpha=0.6, label='Envelope')
    axes[2].plot(data['Time'] * 1000, envelope_lower, 'k--', 
                linewidth=1.5, alpha=0.6)
    axes[2].set_title('AM Modulated Signal s(t) = [1 + μ·m(t)]·cos(2πfct)', 
                     fontsize=14, fontweight='bold')
    axes[2].set_xlabel('Time (ms)', fontsize=12)
    axes[2].set_ylabel('Amplitude', fontsize=12)
    axes[2].grid(True, alpha=0.3)
    axes[2].legend(loc='upper right')
    axes[2].set_xlim(data['Time'].min() * 1000, data['Time'].max() * 1000)
    
    plt.tight_layout()
    plt.savefig(f'{output_prefix}.png', dpi=300, bbox_inches='tight')
    print(f"调制过程图已保存: {output_prefix}.png")
    plt.close()

def plot_am_demodulation(csv_file='am_demodulated.csv', output_prefix='am_demodulation'):
    """绘制AM解调结果对比"""
    
    if not os.path.exists(csv_file):
        print(f"错误: 文件 {csv_file} 不存在")
        return
    
    # 读取数据
    data = pd.read_csv(csv_file)
    
    # 创建图形
    fig, axes = plt.subplots(4, 1, figsize=(14, 12))
    
    time_ms = data['Time'] * 1000
    
    # 原始调制信号
    axes[0].plot(time_ms, data['Original'], 'k-', linewidth=2, label='Original Signal')
    axes[0].set_title('Original Modulating Signal', fontsize=14, fontweight='bold')
    axes[0].set_ylabel('Amplitude', fontsize=12)
    axes[0].grid(True, alpha=0.3)
    axes[0].legend(loc='upper right')
    axes[0].set_xlim(time_ms.min(), time_ms.max())
    
    # 包络检波解调
    axes[1].plot(time_ms, data['Original'], 'k-', linewidth=1.5, 
                alpha=0.5, label='Original')
    axes[1].plot(time_ms, data['Envelope'], 'r-', linewidth=1.5, 
                alpha=0.8, label='Envelope Detection')
    axes[1].set_title('Envelope Detection Demodulation', fontsize=14, fontweight='bold')
    axes[1].set_ylabel('Amplitude', fontsize=12)
    axes[1].grid(True, alpha=0.3)
    axes[1].legend(loc='upper right')
    axes[1].set_xlim(time_ms.min(), time_ms.max())
    
    # Hilbert变换解调
    axes[2].plot(time_ms, data['Original'], 'k-', linewidth=1.5, 
                alpha=0.5, label='Original')
    axes[2].plot(time_ms, data['Hilbert'], 'g-', linewidth=1.5, 
                alpha=0.8, label='Hilbert Transform')
    axes[2].set_title('Hilbert Transform Demodulation', fontsize=14, fontweight='bold')
    axes[2].set_ylabel('Amplitude', fontsize=12)
    axes[2].grid(True, alpha=0.3)
    axes[2].legend(loc='upper right')
    axes[2].set_xlim(time_ms.min(), time_ms.max())
    
    # 相干解调
    axes[3].plot(time_ms, data['Original'], 'k-', linewidth=1.5, 
                alpha=0.5, label='Original')
    axes[3].plot(time_ms, data['Coherent'], 'b-', linewidth=1.5, 
                alpha=0.8, label='Coherent Detection')
    axes[3].set_title('Coherent (Synchronous) Demodulation', fontsize=14, fontweight='bold')
    axes[3].set_xlabel('Time (ms)', fontsize=12)
    axes[3].set_ylabel('Amplitude', fontsize=12)
    axes[3].grid(True, alpha=0.3)
    axes[3].legend(loc='upper right')
    axes[3].set_xlim(time_ms.min(), time_ms.max())
    
    plt.tight_layout()
    plt.savefig(f'{output_prefix}.png', dpi=300, bbox_inches='tight')
    print(f"解调对比图已保存: {output_prefix}.png")
    plt.close()

def plot_demodulation_comparison(csv_file='am_demodulated.csv', 
                                output_file='am_demod_comparison.png'):
    """绘制所有解调方法在同一图上的对比"""
    
    if not os.path.exists(csv_file):
        print(f"错误: 文件 {csv_file} 不存在")
        return
    
    data = pd.read_csv(csv_file)
    
    plt.figure(figsize=(14, 7))
    time_ms = data['Time'] * 1000
    
    plt.plot(time_ms, data['Original'], 'k-', linewidth=2.5, 
            label='Original Signal', alpha=0.8)
    plt.plot(time_ms, data['Envelope'], 'r--', linewidth=1.5, 
            label='Envelope Detection', alpha=0.7)
    plt.plot(time_ms, data['Hilbert'], 'g--', linewidth=1.5, 
            label='Hilbert Transform', alpha=0.7)
    plt.plot(time_ms, data['Coherent'], 'b--', linewidth=1.5, 
            label='Coherent Detection', alpha=0.7)
    
    plt.title('AM Demodulation Methods Comparison', fontsize=16, fontweight='bold')
    plt.xlabel('Time (ms)', fontsize=13)
    plt.ylabel('Amplitude', fontsize=13)
    plt.legend(loc='upper right', fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.xlim(time_ms.min(), time_ms.max())
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"解调方法对比图已保存: {output_file}")
    plt.close()

def plot_modulation_index_comparison():
    """绘制不同调制指数的对比"""
    
    files = [
        ('am_signal_m30.csv', 'μ = 0.3 (30%)'),
        ('am_signal.csv', 'μ = 0.8 (80%)'),
        ('am_signal_m100.csv', 'μ = 1.0 (100%)'),
        ('am_signal_m120.csv', 'μ = 1.2 (120% - Over-modulation)')
    ]
    
    available_files = [(f, label) for f, label in files if os.path.exists(f)]
    
    if not available_files:
        print("警告: 未找到不同调制指数的测试文件")
        return
    
    fig, axes = plt.subplots(len(available_files), 1, 
                            figsize=(14, 3 * len(available_files)))
    
    if len(available_files) == 1:
        axes = [axes]
    
    for idx, (filename, label) in enumerate(available_files):
        data = pd.read_csv(filename)
        time_ms = data['Time'] * 1000
        
        axes[idx].plot(time_ms, data['AM_Signal'], 'b-', linewidth=0.8)
        axes[idx].set_title(f'AM Signal - {label}', fontsize=12, fontweight='bold')
        axes[idx].set_ylabel('Amplitude', fontsize=11)
        axes[idx].grid(True, alpha=0.3)
        axes[idx].set_xlim(time_ms.min(), time_ms.max())
        
        if idx == len(available_files) - 1:
            axes[idx].set_xlabel('Time (ms)', fontsize=11)
    
    plt.tight_layout()
    plt.savefig('am_modulation_index_comparison.png', dpi=300, bbox_inches='tight')
    print(f"调制指数对比图已保存: am_modulation_index_comparison.png")
    plt.close()

def calculate_and_plot_spectrum(csv_file='am_signal.csv', 
                               output_file='am_spectrum.png'):
    """计算并绘制AM信号的频谱"""
    
    if not os.path.exists(csv_file):
        print(f"错误: 文件 {csv_file} 不存在")
        return
    
    data = pd.read_csv(csv_file)
    
    # 提取信号
    am_signal = data['AM_Signal'].values
    time = data['Time'].values
    
    # 计算采样频率
    fs = 1.0 / (time[1] - time[0])
    n = len(am_signal)
    
    # FFT
    fft_result = np.fft.fft(am_signal)
    frequencies = np.fft.fftfreq(n, 1/fs)
    
    # 只保留正频率部分
    positive_freq_idx = frequencies >= 0
    frequencies = frequencies[positive_freq_idx]
    magnitude = np.abs(fft_result[positive_freq_idx]) / n * 2
    
    # 转换为dB
    magnitude_db = 20 * np.log10(magnitude + 1e-10)
    
    # 绘制频谱
    fig, axes = plt.subplots(2, 1, figsize=(14, 10))
    
    # 线性尺度
    axes[0].plot(frequencies / 1000, magnitude, 'b-', linewidth=1)
    axes[0].set_title('AM Signal Spectrum (Linear Scale)', fontsize=14, fontweight='bold')
    axes[0].set_xlabel('Frequency (kHz)', fontsize=12)
    axes[0].set_ylabel('Magnitude', fontsize=12)
    axes[0].grid(True, alpha=0.3)
    axes[0].set_xlim(0, min(frequencies.max() / 1000, 20))
    
    # 对数尺度
    axes[1].plot(frequencies / 1000, magnitude_db, 'r-', linewidth=1)
    axes[1].set_title('AM Signal Spectrum (dB Scale)', fontsize=14, fontweight='bold')
    axes[1].set_xlabel('Frequency (kHz)', fontsize=12)
    axes[1].set_ylabel('Magnitude (dB)', fontsize=12)
    axes[1].grid(True, alpha=0.3)
    axes[1].set_xlim(0, min(frequencies.max() / 1000, 20))
    axes[1].set_ylim(magnitude_db.max() - 80, magnitude_db.max() + 5)
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"频谱图已保存: {output_file}")
    plt.close()

def main():
    """主函数"""
    
    print("=== AM调制解调可视化 ===\n")
    
    # 检查必需的文件
    if not os.path.exists('am_signal.csv'):
        print("错误: 未找到 am_signal.csv")
        print("请先运行: ./am_signal 或 ./test_am.sh")
        sys.exit(1)
    
    # 绘制调制过程
    print("绘制AM调制过程...")
    plot_am_modulation()
    
    # 绘制解调结果
    if os.path.exists('am_demodulated.csv'):
        print("绘制AM解调结果...")
        plot_am_demodulation()
        print("绘制解调方法对比...")
        plot_demodulation_comparison()
    
    # 绘制频谱
    print("计算并绘制频谱...")
    calculate_and_plot_spectrum()
    
    # 绘制不同调制指数的对比
    print("绘制调制指数对比...")
    plot_modulation_index_comparison()
    
    print("\n=== 可视化完成 ===")
    print("\n生成的图片文件:")
    print("  - am_modulation.png: AM调制过程")
    print("  - am_demodulation.png: 解调结果详细对比")
    print("  - am_demod_comparison.png: 解调方法总对比")
    print("  - am_spectrum.png: AM信号频谱")
    print("  - am_modulation_index_comparison.png: 不同调制指数对比")

if __name__ == '__main__':
    main()
