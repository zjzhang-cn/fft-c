#!/usr/bin/env python3
"""
FM信号可视化脚本
绘制原始FM信号、解调信号和理论调制信号的对比
"""

import numpy as np
import matplotlib.pyplot as plt

# 读取数据
def load_data(filename):
    data = np.loadtxt(filename, skiprows=1)
    return data[:, 0], data[:, 1]

# 加载信号数据
t_fm, fm_signal = load_data('fm_signal.txt')
t_demod, demod_signal = load_data('fm_demodulated.txt')
t_orig, orig_signal = load_data('fm_original_modulating.txt')

# 创建图形
fig, axes = plt.subplots(3, 1, figsize=(12, 10))

# 绘制原始FM信号
axes[0].plot(t_fm * 1000, fm_signal, 'b-', linewidth=0.8)
axes[0].set_xlabel('时间 (ms)')
axes[0].set_ylabel('幅度')
axes[0].set_title('调频(FM)信号 - fc=1000Hz, fm=100Hz, β=5')
axes[0].grid(True, alpha=0.3)
axes[0].set_xlim([0, 100])

# 绘制解调信号 vs 原始调制信号
axes[1].plot(t_demod * 1000, demod_signal, 'r-', linewidth=1.2, label='解调信号', alpha=0.7)
axes[1].plot(t_orig * 1000, orig_signal, 'g--', linewidth=1.5, label='理论调制信号')
axes[1].set_xlabel('时间 (ms)')
axes[1].set_ylabel('频率偏移 (Hz)')
axes[1].set_title('FM解调结果对比')
axes[1].legend(loc='upper right')
axes[1].grid(True, alpha=0.3)
axes[1].set_xlim([0, 100])

# 绘制误差
error = demod_signal - orig_signal
axes[2].plot(t_demod * 1000, error, 'm-', linewidth=0.8)
axes[2].set_xlabel('时间 (ms)')
axes[2].set_ylabel('误差 (Hz)')
axes[2].set_title('解调误差')
axes[2].grid(True, alpha=0.3)
axes[2].set_xlim([0, 100])

# 添加统计信息
mse = np.mean(error**2)
rmse = np.sqrt(mse)
max_error = np.max(np.abs(error))
axes[2].text(0.02, 0.95, f'MSE: {mse:.2f}\nRMSE: {rmse:.2f}\n最大误差: {max_error:.2f} Hz',
             transform=axes[2].transAxes, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

plt.tight_layout()
plt.savefig('fm_demodulation_result.png', dpi=150, bbox_inches='tight')
print("图形已保存到: fm_demodulation_result.png")
plt.show()
