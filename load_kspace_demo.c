/**
 * K空间数据加载和重建示例
 * 演示如何从保存的K空间数据重建图像
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 从main-fft2d.c复制必要的函数声明
int load_kspace_binary(const char* filename, double** real, double** imag, int* width, int* height);
void calculate_2d_idft(double* X_real, double* X_imag, int M, int N, double* x_real, double* x_imag);
int save_bmp_grayscale(const char* filename, double* data, int width, int height);

int main() {
    printf("K空间数据加载和重建示例\n");
    printf("========================================\n\n");
    
    // 加载K空间数据
    double *X_real = NULL;
    double *X_imag = NULL;
    int width, height;
    
    printf("正在加载K空间数据...\n");
    if (load_kspace_binary("kspace_data.bin", &X_real, &X_imag, &width, &height) != 0) {
        printf("加载失败！\n");
        return 1;
    }
    
    printf("\n执行逆傅里叶变换重建图像...\n");
    
    // 分配内存用于重建图像
    double *restored_real = (double *)malloc(width * height * sizeof(double));
    double *restored_imag = (double *)malloc(width * height * sizeof(double));
    
    if (!restored_real || !restored_imag) {
        printf("内存分配失败\n");
        free(X_real);
        free(X_imag);
        return 1;
    }
    
    // 执行2D逆傅里叶变换
    calculate_2d_idft(X_real, X_imag, height, width, restored_real, restored_imag);
    
    // 保存重建的图像
    printf("\n保存重建图像...\n");
    save_bmp_grayscale("reconstructed_from_kspace.bmp", restored_real, width, height);
    
    printf("\n重建完成！\n");
    printf("从K空间数据成功重建了图像: reconstructed_from_kspace.bmp\n");
    
    // 释放内存
    free(X_real);
    free(X_imag);
    free(restored_real);
    free(restored_imag);
    
    return 0;
}
