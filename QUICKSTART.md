# DTMF 快速开始指南

## 1. 编译

```bash
make
```

## 2. 运行示例

### 基础示例

```bash
# 播放单个按键
./dtmf 5

# 播放电话号码
./dtmf 13800138000

# 播放按键序列
./dtmf 123456789
```

### 特殊字符示例

```bash
# 播放 * 号（需要引号）
./dtmf "*"

# 播放 # 号（需要引号）
./dtmf "#"

# 播放完整序列（需要引号）
./dtmf "*123#"
```

## 3. 常见使用场景

### 模拟电话拨号

```bash
# 拨打本地号码
./dtmf 12345678

# 拨打手机号码
./dtmf 13912345678

# 拨打带区号的号码
./dtmf 02112345678
```

### 模拟语音菜单选择

```bash
# 选择 1（中文服务）
./dtmf 1

# 选择 9（人工服务）
./dtmf 9

# 输入分机号
./dtmf 8888
```

### 测试所有按键

```bash
# 测试所有数字
./dtmf 0123456789

# 测试所有按键（包括特殊字符）
./dtmf "123456789*0#"
```

## 4. 输出示例

运行 `./dtmf 123` 会产生如下输出：

```
播放DTMF音到设备: plughw:1,0
按键序列: 123

[1/3] 播放按键 '1' (697 Hz + 1209 Hz)... 完成 [识别: '1' ✓]
[2/3] 播放按键 '2' (697 Hz + 1336 Hz)... 完成 [识别: '2' ✓]
[3/3] 播放按键 '3' (697 Hz + 1477 Hz)... 完成 [识别: '3' ✓]

播放完成！

DTMF标准频率表：
        1209Hz  1336Hz  1477Hz
 697Hz    1       2       3
 770Hz    4       5       6
 852Hz    7       8       9
 941Hz    *       0       #
```

## 5. Makefile 命令

```bash
# 编译程序
make

# 清理编译文件
make clean

# 运行测试
make test

# 显示帮助
make help

# 显示版本
make version

# 安装到系统（需要 sudo）
sudo make install

# 从系统卸载（需要 sudo）
sudo make uninstall

# 编译调试版本
make debug
```

## 6. 故障排除

### 没有声音

检查音频设备配置，修改 `main-dtmf.c` 中的：

```c
const char* audio_device = "plughw:1,0";  // 修改为你的设备
```

查看可用设备：

```bash
aplay -l
```

### 编译失败

确保已安装依赖：

```bash
# Ubuntu/Debian
sudo apt-get install gcc alsa-utils

# Fedora/RHEL
sudo dnf install gcc alsa-utils
```

## 7. 更多信息

详细文档请参阅 [README.md](README.md)
