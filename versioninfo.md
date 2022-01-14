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
* 显示波形数据与实际不符

### 2021.11.21 v4.1 修复实时显示曲线数据不符合真实解调数据问题

* QT 中信号和槽函数机制，发送的信号中携带的参数不能是引用，只能是实参。
* 数据异常的问题是，原本程序思路将解调数据存储到std::queue中，因为不能发送引用，因此发送指针，然后在显示文件中读取队列的数据来更新波形。但是实际发现传过来的队列中数据错误，因为使用queue中会出现解调线程对其写入和显示进程读取，进而导致线程冲突，或者单纯是因为信号的参数使用queue无法实现目标功能，因此将queue改成自写的CirQueue类，该类的本质是采用数组实现，因此可以实现目标要求。
* 显示逻辑优化，曲线长度和窗口初始长度相同，滑轮可以放大缩小，右键还原缩放，暂停和开始按键不变，鼠标选中显示点坐标依旧。
* 但是由于显示数据量较大，加上刷新率为50Hz，因此波形刷新时，不建议使用鼠标选中显示功能会出现明显卡顿，建议在暂停后进行坐标点查看。

存在问题：

* 原始数据模式还未修改和优化。
* 解调速率目前是以解调后速度计算的，修改成读取原始通道数据。
* 点击停止解调程序未响应，可能是内存泄漏。

### 2021.11.26 v4.1 修复关闭解调未响应的bug，新增存储原始数据关闭显示，显示串口速率

* 存储原始通道数据时，关闭显示进程
* 主窗口右下角速率更新成读取原始通道数据速率（无论是否开启解调）

[![on8BcR.png](https://z3.ax1x.com/2021/11/28/on8BcR.png)](https://imgtu.com/i/on8BcR)
* 存在问题，目前程序根据30kHz实现显示，还未测试不同采样率的情况显示是否异常（主要是当采样率低于30kHz）

## 2021.12.13 v0.4.1 实现不同采样率显示
* 实现不同采样率显示，已经测试10kHz和30kHz。
* 后续需要将数据写入函数由fwrite改成通过字节流写入。
* 将QTimer的类型设置为Qt::PreciseTimer，可以提高定时器的准确性（待测试）http://www.qtcn.org/bbs/read-htm-tid-58314.html
* 解调数据存储队列，再主窗口中初始化，然后通过引用作为实参给予解调线程和显示线程，注意线程锁。

## 2021.12.23 v0.4.3 重载存储函数和文件名按时间更新函数（未长时间测试）
* 存储函数修改为ofstream的字节流存储
* 如果一分钟内实际解调的数据量小于1分钟该有的长度（frequency * 60 * 4），仪表的输出数据速度可能会波动导致的。继续在该文件中存储，直到存储到正确的长度，才会执行文件修改，获取文件名是通过计时器驱动，如果数据传输过快，还没达到下一分钟就完成了正确长度的数据的存储，那么继续在该文件中存储数据，直到时间到达60s，更换文件存储。
* 后续修改程序架构，读取串口数据和解调分线程执行，尝试实现40kHz解调。

## 2021.1.4 v0.4.4 重构读取、解调、存储程序架构（短期测试成功）
* 实现读取FPGA原始通道数据、解调数据、存储数据三线程分离。
* 测试40kHz解调和存储无问题，解调上限未测试。

存在问题：
* 目前的滤波功能存在滤波后数据出现频率倍频，因此显示波形可能与实际信号有差别，但是因此建议存储数据关闭滤波，不影响存储数据。
* 显示波形长度会突然增加然后慢慢缩短，再突然增加以此循环
* 由于目前对Config文件中解调部分设置只针对于存储数据，因此需要考虑显示波形的滤波问题，可能出现显示波形不需要低通滤波的情况，目前的想法是不需要显示低通滤波时，将对应的文件名留空。
* 还需要长时间测试,测试各存储队列是否溢出
* 删除不再使用的函数

## 2022.1.10 v0.4.4 修改配置文件读取滤波文件逻辑，测试溢出无问题
* 滤波器文件留空或者是不存在文件，则显示波形不执行该滤波（存储文件也只能存储无该滤波的数据）
* 6小时运行测试未发现队列溢出等bug
* 设置主窗口置顶，波形显示窗口默认最大化，且关闭其自带的关闭按钮(防止用户手滑把窗口关了)。

## 2022.1.14 v0.4.5 修改显示波形的缩放方法，解决显示波形延迟
* 波形未暂停时，无法对波形进行任何操作。当波形暂停时可以进行操作。
* 鼠标滚轮改变Y轴缩放，crtl + 鼠标滚轮改变X轴缩放， 鼠标右键重置缩放。