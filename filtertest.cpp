#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
using namespace std;

const float pi = 3.14159;
const int N = 1000;
const float NUM[17] = {
     0.9911850868805,   -7.929461129839,    27.75306504154,   -55.50607138771,
      69.38256477825,   -55.50607138771,    27.75306504154,   -7.929461129839,
     0.9911850868805,    7.982272314081,    27.87608294746,   -55.62877789674,
      69.38217627045,   -55.38305407073,    27.63035794074,   -7.876960753541,
     0.9824478764544
};


int cnt[10];
float signal[N];
float RealPhReg[10][18];
float RealPhOut[10][18];
int main()
{
    float t;
     for (int i=0; i<1000; i++)
        {
            t=i/1000.0;
            signal[i]=sin(2*pi*0.5*t) + sin(2*pi*1.5*t);
        }

    
    //memset(signal, 1, sizeof signal);
    memset(RealPhOut, 0, sizeof RealPhOut);
    memset(RealPhReg, 0, sizeof RealPhOut);
    memset(cnt, 0, sizeof cnt);
    int n = N;
    
    while(n--)
    {
        for(int i = 0; i < 1; i ++ )
        {
            for(int j = 0; j < 8; j++)
            {
                RealPhReg[i][j] = RealPhReg[i][j + 1];
                cout << RealPhReg[i][j] << endl;
            }
            RealPhReg[i][8] = signal[n];
            cnt[i] ++;
            float res = 0;
            if(cnt[i] > 9){
                    //实际的FilterCoeff(3) = 1  因为 该参数不变，因此不存储在滤波器文件中。
                    //滤波，1 * outdata(i)=FilterCoeff(0) * data(i) + FilterCoeff(1) * data(i - 1) + FilterCoeff(2) * data(i-2) - FilterCoeff(3)*outdata(i-1)-FilterCoeff(4)*outdata(i-2);
                        for(int j = 0; j < 9; j++)
                        {
                            RealPhOut[i][j] = RealPhOut[i][j + 1];
                        }
                    //此处的程序已经改成和上面滤波的符号相同，因此以后生成的滤波器系数文件 只需要直接粘贴matlab里fdatools生成的。
                        
                        for(int j = 0; j < 9; j ++)
                            res += NUM[j] * RealPhOut[i][8 - j];
                        for(int j = 9; j < 17; j++)
                            res -= NUM[j] * RealPhOut[i][16 - j];     
                        RealPhOut[i][8] = res;
                        //cout << res << endl;
                    }
        }
    }
    return 0;
}