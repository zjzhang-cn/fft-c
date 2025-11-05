#!/usr/bin/env python3
"""
包络检波器结果可视化脚本
绘制 AM 信号、整流后信号、解调信号
"""

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import sys

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

def plot_envelope_detector_result(csv_file='envelope_detector_result.csv'):
    """绘制包络检波结果"""
    
    try:
        # 读取数据
        data = pd.read_csv(csv_file)
        t = data['time'].values
        am_signal = data['AM_Signal'].values
        demod_signal = data['Demodulated'].values
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        
        # 子图 1: AM 调制信号
        axes[0].plot(t * 1000, am_signal, 'b-', linewidth=0.8, label='AM 调制信号')
        axes[0].set_xlabel('时间 (ms)')
        axes[0].set_ylabel('幅度 (V)')
        axes[0].set_title('AM 调制信号')
        axes[0].grid(True, alpha=0.3)
        axes[0].legend()
        
        # 子图 2: AM 信号 + 解调信号叠加
        axes[1].plot(t * 1000, am_signal, 'b-', linewidth=0.5, alpha=0.5, label='AM 信号')
        axes[1].plot(t * 1000, demod_signal, 'r-', linewidth=2, label='解调信号（包络）')
        axes[1].set_xlabel('时间 (ms)')
        axes[1].set_ylabel('幅度 (V)')
        axes[1].set_title('包络检波过程')
        axes[1].grid(True, alpha=0.3)
        axes[1].legend()
        
        # 子图 3: 解调信号细节
        axes[2].plot(t * 1000, demod_signal, 'r-', linewidth=1.5, label='解调信号')
        
        # 叠加理论调制信号
        if len(t) > 10:
            # 估计调制频率
            fm_est = 500  # 假设已知
            theory = np.cos(2 * np.pi * fm_est * t)
            # 归一化
            theory = theory * (np.max(demod_signal) / np.max(theory))
            axes[2].plot(t * 1000, theory, 'g--', linewidth=1.5, alpha=0.7, label='理论调制信号')
        
        axes[2].set_xlabel('时间 (ms)')
        axes[2].set_ylabel('幅度 (V)')
        axes[2].set_title('解调信号与理论对比')
        axes[2].grid(True, alpha=0.3)
        axes[2].legend()
        
        plt.tight_layout()
        
        # 保存图片
        output_file = 'envelope_detector_result.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"图像已保存到: {output_file}")
        
        plt.show()
        
    except FileNotFoundError:
        print(f"错误: 找不到文件 {csv_file}")
        print("请先运行包络检波器程序生成数据文件")
        sys.exit(1)
    except Exception as e:
        print(f"错误: {e}")
        sys.exit(1)

def plot_rc_filter_response():
    """绘制 RC 滤波器的频率响应"""
    
    # 不同 RC 参数
    configs = [
        {'R': 1e3, 'C': 0.1e-6, 'label': 'R=1kΩ, C=0.1μF'},
        {'R': 47e3, 'C': 0.47e-6, 'label': 'R=47kΩ, C=0.47μF (最佳)'},
        {'R': 1e6, 'C': 1e-6, 'label': 'R=1MΩ, C=1μF'},
    ]
    
    # 频率范围
    f = np.logspace(0, 5, 1000)  # 1Hz 到 100kHz
    
    fig, axes = plt.subplots(2, 1, figsize=(10, 8))
    
    for config in configs:
        R = config['R']
        C = config['C']
        fc = 1 / (2 * np.pi * R * C)
        
        # 传递函数幅度
        H = 1 / np.sqrt(1 + (f / fc)**2)
        
        # 传递函数相位
        phase = -np.arctan(f / fc) * 180 / np.pi
        
        # 绘制幅度响应
        axes[0].semilogx(f, 20 * np.log10(H), linewidth=2, label=config['label'])
        
        # 绘制相位响应
        axes[1].semilogx(f, phase, linewidth=2, label=config['label'])
    
    # 幅度图
    axes[0].set_xlabel('频率 (Hz)')
    axes[0].set_ylabel('幅度 (dB)')
    axes[0].set_title('RC 低通滤波器幅度响应')
    axes[0].grid(True, which='both', alpha=0.3)
    axes[0].legend()
    axes[0].axhline(-3, color='r', linestyle='--', linewidth=1, alpha=0.5)
    axes[0].text(10, -3, '-3dB 截止频率', verticalalignment='bottom')
    
    # 相位图
    axes[1].set_xlabel('频率 (Hz)')
    axes[1].set_ylabel('相位 (度)')
    axes[1].set_title('RC 低通滤波器相位响应')
    axes[1].grid(True, which='both', alpha=0.3)
    axes[1].legend()
    
    plt.tight_layout()
    
    output_file = 'rc_filter_response.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"RC 滤波器响应图已保存到: {output_file}")
    
    plt.show()

def plot_diode_characteristic():
    """绘制二极管特性曲线"""
    
    # 电压范围
    v = np.linspace(-1, 1, 1000)
    
    # 二极管参数
    Is = 1e-12  # 饱和电流
    n = 1.5     # 理想因子
    Vt = 0.026  # 热电压
    Vd = 0.7    # 简化模型导通电压
    
    # 指数模型
    i_exp = np.zeros_like(v)
    for idx, v_val in enumerate(v):
        if v_val > 0.7:
            i_exp[idx] = Is * np.exp(0.7 / (n * Vt)) * (1.0 + (v_val - 0.7) / (n * Vt))
        elif v_val < -5.0:
            i_exp[idx] = -Is
        else:
            i_exp[idx] = Is * (np.exp(v_val / (n * Vt)) - 1.0)
    
    # 简化模型（整流器）
    v_rect = np.where(v > Vd, v - Vd, 0)
    
    # 绘图
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    # 子图 1: I-V 特性
    axes[0].plot(v, i_exp * 1e3, 'b-', linewidth=2, label='指数模型')
    axes[0].set_xlabel('电压 (V)')
    axes[0].set_ylabel('电流 (mA)')
    axes[0].set_title('二极管 I-V 特性曲线')
    axes[0].grid(True, alpha=0.3)
    axes[0].legend()
    axes[0].axhline(0, color='k', linewidth=0.5)
    axes[0].axvline(0, color='k', linewidth=0.5)
    
    # 子图 2: 整流器模型
    axes[1].plot(v, v, 'g--', linewidth=1.5, alpha=0.5, label='理想二极管')
    axes[1].plot(v, v_rect, 'r-', linewidth=2, label=f'简化模型 (Vd={Vd}V)')
    axes[1].set_xlabel('输入电压 (V)')
    axes[1].set_ylabel('输出电压 (V)')
    axes[1].set_title('二极管整流特性')
    axes[1].grid(True, alpha=0.3)
    axes[1].legend()
    axes[1].axhline(0, color='k', linewidth=0.5)
    axes[1].axvline(0, color='k', linewidth=0.5)
    
    plt.tight_layout()
    
    output_file = 'diode_characteristic.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"二极管特性曲线已保存到: {output_file}")
    
    plt.show()

if __name__ == '__main__':
    print("包络检波器可视化工具")
    print("=" * 50)
    
    if len(sys.argv) > 1:
        if sys.argv[1] == '--rc':
            print("\n绘制 RC 滤波器频率响应...")
            plot_rc_filter_response()
        elif sys.argv[1] == '--diode':
            print("\n绘制二极管特性曲线...")
            plot_diode_characteristic()
        elif sys.argv[1] == '--all':
            print("\n绘制所有图表...")
            plot_envelope_detector_result()
            plot_rc_filter_response()
            plot_diode_characteristic()
        else:
            print("\n绘制包络检波结果...")
            plot_envelope_detector_result(sys.argv[1])
    else:
        print("\n绘制包络检波结果...")
        plot_envelope_detector_result()
        
    print("\n完成！")
