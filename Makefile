# Makefile for DTMF Signal Generator & Analyzer

# 编译器
CC = gcc

# 编译标志
CFLAGS = -Wall -Wextra -O2 -std=c99

# 链接库
LDFLAGS = -lm

# 目标文件
TARGET = dtmf
TARGET_FFT2D = fft2d
TARGET_KSPACE = kspace_to_image
TARGET_FM = fm_signal
TARGET_AM = am_signal
TARGET_ENVELOPE = envelope_detector

# 源文件
SOURCES = main-dtmf.c

# 对象文件
OBJECTS = $(SOURCES:.c=.o)

# 默认目标
.PHONY: all
all: $(TARGET) $(TARGET_FFT2D) $(TARGET_KSPACE) $(TARGET_FM) $(TARGET_AM) $(TARGET_ENVELOPE)

# 编译目标
$(TARGET): $(SOURCES)
	@echo "正在编译 DTMF 信号生成器..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@echo "编译完成！使用 './$(TARGET) <按键>' 运行程序"

# 编译2D FFT程序
$(TARGET_FFT2D): main-fft2d.c
	@echo "正在编译 2D FFT 程序..."
	$(CC) $(CFLAGS) -o $(TARGET_FFT2D) main-fft2d.c $(LDFLAGS)
	@echo "编译完成！使用 './$(TARGET_FFT2D)' 运行程序"

# 编译K空间还原程序
$(TARGET_KSPACE): kspace_to_image.c
	@echo "正在编译 K空间还原程序..."
	$(CC) $(CFLAGS) -o $(TARGET_KSPACE) kspace_to_image.c $(LDFLAGS)
	@echo "编译完成！使用 './$(TARGET_KSPACE) [kspace_data.bin]' 运行程序"

# 编译FM信号生成与解调程序
$(TARGET_FM): main-fm.c
	@echo "正在编译 FM 信号生成与解调程序..."
	$(CC) $(CFLAGS) -o $(TARGET_FM) main-fm.c $(LDFLAGS)
	@echo "编译完成！使用 './$(TARGET_FM)' 运行程序"

# 编译AM信号生成与解调程序
$(TARGET_AM): main-am.c
	@echo "正在编译 AM 信号生成与解调程序..."
	$(CC) $(CFLAGS) -o $(TARGET_AM) main-am.c $(LDFLAGS)
	@echo "编译完成！使用 './$(TARGET_AM)' 运行程序"

# 编译包络检波器
$(TARGET_ENVELOPE): envelope_detector.c
	@echo "正在编译包络检波器..."
	$(CC) $(CFLAGS) -o $(TARGET_ENVELOPE) envelope_detector.c $(LDFLAGS)
	@echo "编译完成！使用 './$(TARGET_ENVELOPE)' 运行程序"

# 编译对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理编译文件
.PHONY: clean
clean:
	@echo "清理编译文件..."
	rm -f $(TARGET) $(TARGET_FFT2D) $(TARGET_KSPACE) $(TARGET_FM) $(TARGET_AM) $(TARGET_ENVELOPE) $(OBJECTS)
	rm -f *.o *.bmp *.txt *.csv *.bin *.wav *.png
	@echo "清理完成！"

# 测试运行
.PHONY: test
test: $(TARGET)
	@echo "运行测试..."
	@echo "\n=== 测试 1: 单个按键 ==="
	./$(TARGET) 5
	@echo "\n=== 测试 2: 多个按键 ==="
	./$(TARGET) 123
	@echo "\n=== 测试 3: 特殊字符 ==="
	./$(TARGET) "*0#"

# 安装（可选）
.PHONY: install
install: $(TARGET)
	@echo "安装 DTMF 到 /usr/local/bin/"
	sudo cp $(TARGET) /usr/local/bin/
	@echo "安装完成！"

# 卸载
.PHONY: uninstall
uninstall:
	@echo "卸载 DTMF..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "卸载完成！"

# 帮助信息
.PHONY: help
help:
	@echo "DTMF 信号生成器 Makefile"
	@echo ""
	@echo "可用目标："
	@echo "  make          - 编译程序（默认）"
	@echo "  make all      - 编译程序"
	@echo "  make clean    - 清理编译文件"
	@echo "  make test     - 编译并运行测试"
	@echo "  make install  - 安装到系统（需要 sudo）"
	@echo "  make uninstall- 从系统卸载（需要 sudo）"
	@echo "  make help     - 显示此帮助信息"
	@echo ""
	@echo "使用方法："
	@echo "  ./$(TARGET) <按键序列>"
	@echo ""
	@echo "示例："
	@echo "  ./$(TARGET) 5          - 播放按键 5"
	@echo "  ./$(TARGET) 123        - 播放按键序列 1-2-3"
	@echo "  ./$(TARGET) \"*123#\"    - 播放 *-1-2-3-#"

# 调试版本
.PHONY: debug
debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)
	@echo "调试版本编译完成！"

# 版本信息
.PHONY: version
version:
	@echo "DTMF 信号生成器 v1.0"
	@echo "编译器: $(CC)"
	@$(CC) --version | head -n 1
