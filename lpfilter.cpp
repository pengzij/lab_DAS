// lpfilter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma warning(disable:4996)；
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
using namespace std;
int main()
{
    float* data = new float[1800000]();
    float* coeff = new float[10]();
    float* w = new float[5]();
    float* out = new float[1800000]();
    FILE* pfile = fopen("G:/10.24/[0]20201024163242_30KHz.bin", "rb");
    fread(data, sizeof(float), 1800000, pfile);
    for (int i = 0; i < 10; i++) {

        cout << "data" << data[i] << endl;
    }
    FILE* coefffile= fopen("F:/mystudy/c++/DAS New/LPFilterCoefficient_30KHz_2KHz.bin","rb");
    fread(coeff, sizeof(float), 10, coefffile);
    for (int i = 0; i < 10; i++) {

        cout << "coeff" << coeff[i] << endl;
    }
    FILE* wrpfile = fopen("F:/mystudy/c++/DAS New/111.bin", "wb");
    for (int n = 0; n < 1800000; n++)
    {
        w[0] = coeff[0] * (data[n] - coeff[1] * w[1] - coeff[2] * w[2] - coeff[3] * w[3] - coeff[4] * w[4]);
        //cout << "w[0]" << w[0] << endl;
        out[n] = coeff[5] * w[0] + coeff[6] * w[1] + coeff[7] * w[2] + coeff[8] * w[3]+coeff[9] * w[4];
        //cout << "out" << out[n] << endl;
        for (int i = 4; i > 0; i--) {
            w[i] = w[i - 1];
        }
        fwrite(&out[n], sizeof(float), 1, wrpfile);
    }
    cout << "out" << out[10] << endl;
    
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
