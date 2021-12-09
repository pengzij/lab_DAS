## 2021.4.25 v0.1.0 上传github的第一个版本 ##
1. 目前已经实现数据的30kHz（32kHz）一下的正常解调, 40KHz以下可以存储原始通道数据解调依然报错。并且修改了滤波函数的执行符号问题，之后的滤波器文件可以直接通过matlab中的fdatools生成的系数直接使用，高通滤波不需要手动更改符号，该版本以前的程序该处程序还未修改（即只有12楼的为最新程序，8楼和3楼的都是未更新程序，因此如果更新程序后需要把对应的(C:/DAS/ButtorWorthFilterCoefficient/)该文件夹一起复制并覆盖。 
## 2021.4.28 v0.1.1 分支开发 简化用户使用流程，减少软件启动需要点击的按键 ##
### 功能变更：
1. 主界面添加快速启动按钮，可实现一键解调。
2. 关闭空白的波形显示弹窗。
### 逻辑
1. 关闭无用按键
2. 调用plot中的峰值确认函数
3. 在sendPeakPosData()函数中结束if语句中加入发送 signal，该信号直接联通开始解调按键的槽函数。 
## 其他
1. DAS_New.pro 文件中加入 UI_DIR=./UI 实现ui_xxx.h文件存储在源程序文件夹UI。因为在添加UI组件时出现QT无法自动复写ui_xxx.h,因此可能需要手动复写。

## 2021.6.28 v0.1.1 源码打包成exe
### 功能修改
1. 将demodu中qDebug() << "SEND";通过signal发送倒mainwindow中的slot函数showSendText(int sendsize)，来实现SEND信息在mainwindow中的statetext中不断显示，由此来判断软件开启是否成功。
2. 将源码打包成  ~\lab_DAS_v0.1.1\demo\DAS_v0.1.1.exe文件。
## 2021.7.14 v0.2.1 新疆特供版本 加入1hz 8阶高通滤波器
1. 采样率为1kHz


## 2021.10.13  config文本中加入滤波器参数带宽设置， 加入debug模式。

* 测试新滤波器是否正常。
* 测试滤波器参数是否正常


修改后配置文件：

```txt
[FPGACard]
PortName = 1;          
BaudRate = 38400;
Freqency = 1000;
ChannelLength = 6000;
ChannelCount = 3 ;
BitCount = 16;
PackagePerRead = 10;

[Peak]
StartPoint=500;
CH1ThreadHold = 8500;
CH2ThreadHold = 8500;
CH3ThreadHold = 8500;
FBGInterval = 30;                   

[Demodulation]

CaculationPhase = true;
Filter = false;
UnWrap = false;
UnWrapFilter = false;

[Server]
IP = 127.0.0.1;
PORT = 5050;

[Filter]
HighPassCutoff = 1;
LowPassCutoff = 3000; 

[DataProcess]
IsSend = true;
IsSave = true;
SavePath = C:/DAS/data;
WaveLength = 4000;
ValueMin = -2;
ValueMax = 2;

[Program]
DebugMode = true;
```
------
* debug模式，软件可以在本地PC运行，屏蔽采集卡通信失败，强制进入解调功能，采集卡传输数据改成读取本地文件。

## 2021.10.18 v0.2.2 config文件中可以配置debug模式，滤波器开关，滤波器文件选择。
* 滤波器实现函数改回原版，通过简单的多项式实现高通滤波（滤波器参数转为single），之前测试的多section滤波器失败。
* config中添加滤波器参数包含是否开启滤波器（开启滤波最必开启高通滤波），是否独立开启低通滤波器，
* config中添加debug模式开关，用于在无仪表可用时进行调试，通过读取本地原始通道数据模拟FPGA读取数据。
C:/DAS/Config.txt
```
[FPGACard]
PortName = 2;          
BaudRate = 38400;
Freqency = 30000;
ChannelLength = 6000;
ChannelCount = 3 ;
BitCount = 16;
PackagePerRead = 10;

[Peak]
StartPoint=500;
CH1ThreadHold = 8500;
CH2ThreadHold = 8500;
CH3ThreadHold = 8500;
FBGInterval = 30;                   

[Demodulation]
CaculationPhase = true;
Filter = true;
UnWrap = true;
LpFilter = true;

[Server]
IP = 127.0.0.1;
PORT = 5050;

[Filter]
HighPassCutoff = ButtorWorthFilterCoefficient_30KHz_5Hz.bin;
LowPassCutoff = LPFilterCoefficient_30KHz_8KHz.bin; 

[DataProcess]
IsSend = true;
IsSave = true;
SavePath = C:/DAS/data;
WaveLength = 4000;
ValueMin = -2;
ValueMax = 2;

[Program]
DebugMode = true;
```
C:/DAS/debug.txt
```
peaknum = 30;
```

debug.txt 中配置debug模式下peaknum的值，需要和本地读取通道数的peaknum相同。

* config文件中的是否解调选项：CaculationPhase， 是否滤波： Filter， 是否解缠绕： unwrap， 是否低通滤波： LpFilter, 高通滤波器文件名： HighPassCutoff， 低通滤波器文件名： LowPassCutoff。

### 目前存在问题：
* debug模式下点停止解调程序无法正常关闭，会未响应，原因未知。
* debug模式下数据的解调和解缠绕后的数据都存在很多跳变无法消除，但是非debug模式下，相同的代码解调数据正常，原因未知。
### 后续：
* 先尝试实现解调数据实时波形图
* 主界面加上实时解调数据速率

## 2021.10.28 v0.3.1 添加实时显示曲线，实时解调速率显示，取消原本文本框的send消息
[![ISVMk9.png](https://z3.ax1x.com/2021/10/31/ISVMk9.png)](https://imgtu.com/i/ISVMk9)

* 加入实时解调波形，显示数据范围小于250，增长数组大小，窗口显示定位不动，大于250小于6000，增长数组大小，窗口最右端始终定位最新数据,大于6000，显示数组大小固定，加入新元素后将弹出最老元素，窗口最右端始终定位到第6000个元素位置。
* 波形窗口左下角区域用于选择显示的peaknum（从1开始），改变显示区域图像会清空，中下按键用于暂停和开始波形显示，设置按键还没开发功能。
* 图像中，鼠标选中的图像点，会显示对应坐标，鼠标拖拽和点击都会暂停显示窗口的自动定位，鼠标拖拽可以改变显示曲线的xy轴，滑动滚轮进行显示范围的整体缩放，点击鼠标右键回到原始缩放，鼠标的操作暂停定位后，停止操作1s后，重新开始自动定位。

* 主窗口右下角添加实时解调速率显示框。