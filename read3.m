%%读取光纤段数据
clear all;
close all;
fclose all;
n =10; %F:\mystudy\matlab\data\D1_C1_P21_20200818150800.bin
%flag=fopen('G:\12.26\D1_C1_P10_20211226094400.bin','rb');
%flag=fopen('G:\12.15不同采样率低频成分实验数据\after\[8]20211215191815_10KHz.bin','rb');
flag=fopen('C:\DAS\data\[9]20220423202428_30KHz.bin','rb');  %%F:\mystudy\c++\butterworthFilter\butterworthFilter\D1_C1_P21_20200818150800.bin
datax=fread(flag,'float32');
%datax = interp(datax, 3);
fclose(flag);
plot(datax);
dataright = datax();
% L=fix(length(datax)/n);%数据发送不全，最后一个包数据不齐，没有21个
% data1=datax(1:L*n,1);
% data2=reshape(data1,n,L);%把数据分成每21个区的。
% dataright = data2(1,:);
Region = 1;
 %% 频谱分析
% showpeak = 8;
% target2 = real_ph(:, showpeak);
Fs = 30000;
target2 = dataright;
n = length(target2);
yy = fft(target2);


t = (0:length(yy)-1)*Fs/length(yy);
fshift = (-n/2:n/2-1)*(Fs/n);
yshift = fftshift(yy);
realy=2*abs(yy(1:n/2+1))/n;
realf=(0:n/2)*(Fs/n);
figure(3);
plot(realf,realy);
%axis([0 60 -inf inf]);
set(gca,'FontSize',24);
xlabel('频率(Hz)');
ylabel('频谱 dB rad/Hz');
title('');
%% 
fl = filter(dataright, HPFilter_3Hz);
%%
d = designfilt('highpassiir', 'StopbandFrequency', 3, 'PassbandFrequency', 5, 'StopbandAttenuation', 60, 'PassbandRipple', 1, 'SampleRate', 30000);
output = filter(d, dataright);
%%
Hd = HPFilter_3Hz;
output=filter(Hd,dataright);
%%
%%读取光纤段数据
clear all;
close all;
fclose all;
n=8; 
flag=fopen('G:\12.16\D1_C1_P8_20211216161700.bin','rb');  %%文件地址+文件名
data=fread(flag,'single');
fclose(flag);
data1=reshape(data,n,length(data)/n);
data1=data1';
aa=data1(:,2);
% aa=smooth(smooth(aa));
plot(aa);
%%
aa=data2(Region,:);%选择通道数
fileaa=fopen('F:\mystudy\c++\DAS New\shui_ting_data\oneCH.bin','wb');
fwrite(fileaa,aa,'float32');
%%   unwraping（相位解缠）
real_ph=datax;
%real_ph=unwrap(data2,pi,2);
% k=0;
% for j = 1:(length(ph(:,1)))
%     for i = 1:(length(ph(1,:))-1)
%         if(ph(j,i+1) - ph(j,i) < -pi)
%             k=k+1;
%             real_ph(j,i+1) = ph(j,i+1) + 2 * k * pi;
%         end
%         if(ph(j,i+1) - ph(j,i) > pi)
%             k=k-1;
%             real_ph(j,i+1) = ph(j,i+1) + 2 * k * pi;
%         end
%         if(ph(j,i+1) - ph(j,i) >= -pi && ph(j,i+1) - ph(j,i) <= pi)
%             real_ph(j,i+1) = ph(j,i+1) + 2 * k * pi;
%         end
%     end
% end
%% 滤波(ButterWorth Filter)  (初步确定是双线性变换法设计数字滤波器)
fre=5;%5hz高通
Fs=10000;
res=1.5;
c=tan(pi*fre/Fs);
a1=1/(1+res*c+c*c);
a2=-2*a1;
a3=a1;
b1=2*(c*c-1)*a1;
b2=(1-res*c+c*c)*a1;
%% 滤波
fdatool;

%%
%11k 采样率  1.5k低通 4阶
FilterCoeffnum=[  0.0048243433577162273 , 0.019297373430864909       ,   0.028946060146297362         ,  0.019297373430864909  , 0.0048243433577162273 ];
FilterCoeffden=[ 1,  -2.3695130071820381           ,     2.3139884144158804           ,-1.0546654058785678        ,     0.18737949236818496    ];
FilterCoeff=[FilterCoeffden  FilterCoeffnum];
    data = datax';
    
    len=length(data);
    
    
    w1=0;
    w2=0;
    w3=0;
    w4=0;
    
%%
  
    
    for i= 1:len
        w0(i)=FilterCoeffden(1)*(data(i)-FilterCoeffden(2)*w1-FilterCoeffden(3)*w2-FilterCoeffden(4)*w3-FilterCoeffden(5)*w4);
        out(i)=FilterCoeffnum(1)*w0(i)+FilterCoeffnum(2)*w1+FilterCoeffnum(3)*w2 - FilterCoeffnum(4)*w3 - FilterCoeffnum(5)*w4;
        w4=w3;
        w3=w2;
        w2=w1;
        w1=w0(i);
    end
%% 生成低通滤波器系数文件
%低通 4阶
FilterCoeffnum=[ 0.093980851433794449,0.3759234057351778  , 0.56388510860276664   ,0.3759234057351778   ,  0.093980851433794449   ];
FilterCoeffden=[ 1, -0.00000000000000027600464381419494 ,  0.48602882206826958 , -0.000000000000000057882412619964346 ,   0.017664800872441901  ];
FilterCoeff=[FilterCoeffden  FilterCoeffnum];
fid=fopen('F:\mystudy\c++\DAS New\LPFilterCoefficient_40KHz_10kHz.bin','wb');
fwrite(fid,FilterCoeff,'float32');
 fclose(fid);
%%
% single section 
% 生成IIR 高通滤波器系数文件生成

%通过fdatool;指令生成需要的滤波器参数，需要转换成单部分系数(convert to single section)，将生成的
%filter Coefficients copy，  2阶
FilterCoeff=[   0.99944479381674312 ,-1.9988895876334862    ,  0.99944479381674312  , -1.9988892793795567     ,   0.99888989588741606 ];
%1 * outdata(i)=FilterCoeff(0) * data(i) + FilterCoeff(1) * data(i - 1) + FilterCoeff(2) * data(i-2) - FilterCoeff*outdata(i-1)-b2*outdata(i-2);


 fid=fopen('F:\mystudy\c++\DAS New\ButtorWorthFilterCoefficient_40KHz_5Hz.bin','wb');
 fwrite(fid,FilterCoeff,'float32');
 fclose(fid);
 %%
% 生成IIR 高通滤波器系数文件生成
% muti section
IIR_B = [
  
  0.9999354491354,                 0,                 0 , ...
               1,   -1.999999957805,                 1 , ...
     0.9998162471308,                 0,                 0, ...
                   1,   -1.999999969674,                 1 , ...
     0.9997250330635,                 0,                 0 , ...
                   1,   -1.999999986461,                 1 , ...
     0.999675675306,                 0,                 0 , ...
                   1,    -1.99999999833,                 1 , ...
                   1,                 0,                 0 
];

IIR_A = [
  
  1,                 0,                 0 , ...
  1,   -1.999870801381,   0.9998709529678  , ...
  1,                 0,                 0 , ...
  1,   -1.999632409251,    0.9996325489521, ...
  1,                 0,                 0  , ...
  1,   -1.999449997906,   0.9994501208124 , ...
   1,                 0,                 0 , ...
   1,   -1.99935129426,   0.9993514052946 , ...
    1,                 0,                 0  
];
IIR_FilterCoeff = [IIR_B, IIR_A];
fid=fopen('F:\mystudy\c++\DAS New\ChebysheyIIFilterCoefficient_Order8_4Section_30KHz_1Hz.bin','wb');
 fwrite(fid,IIR_FilterCoeff,'float32');
 fclose(fid);


%通过fdatool;指令生成需要的滤波器参数，需要转换成单部分系数(convert to single section)，将生成的
%%
 for xx = 1:21
    data = real_ph(xx,:);
    data=data-data(1);
    len=length(data);
    tic
    outdata(1)=0;
    outdata(2)=0;%a1*data(2)+a2*data(1)-b1*outdata(1);
    for i=3:len
        outdata(i)=a1*data(i)+a2*data(i-1)+a3*data(i-2)-b1*outdata(i-1)-b2*outdata(i-2);
    end
    toc
    FiltPhi(xx,:) = outdata;
 end
 %%
 for xx=1:21
    FiltPhi2(xx,:)=FiltPhi(xx,2000:end); 
    
 end
 FiltPhi3=FiltPhi2-mean(FiltPhi2(Region,:));


figure(1);
plot(FiltPhi3(Region,:));%real_ph(Region,:)

figure(2);
plot(real_ph(Region,:));
%%
out = dataright;
Fs=30000;
figure(3);
plot(out);
%%fft
%信号采样率
N=length(out);
n=0:N-1;
ft=fft(out,N);
aft=abs(ft);
f=n*Fs/N;

figure(5);
Z = aft(1:N/2);
plot(f(1:N/2),aft(1:N/2));
%Z=abs(Sot(N0:N0+Nwid-1));
Z=Z/max(Z);
Z=20*log10(Z+1e-6);
figure(2);
plot(f(1:N/2), Z);
%生成音频文件 

%%
out = dataright;
Fs=1000;
figure(3);
plot(out);
%%fft
%信号采样率
N=length(out);
n=0:N-1;
ft=fft(out,N);
aft=abs(ft);
f=n*Fs/N;
figure(5);
Z = aft(1:N/2);
plot(f(1:N/2),aft(1:N/2));
%Z=abs(Sot(N0:N0+Nwid-1));
Z=Z/max(Z);
Z=20*log10(Z+1e-6);
figure(2);
plot(f(1:N/2), Z);
%生成音频文件 
%%

%x=0:1/fs:1;
fileName='output4.wav';
Region1 = 4;
audiowrite(fileName,out,Fs);





% x=0:1/Fs:1;
% fileName='F:\mystudy\c++\DAS New\shui_ting_data\output.wav';
% audiowrite(fileName,datax(Region1,:),Fs);
%reshaped数据存入bin文件
% data3=zeros(1,n*L);
% for i=1:1:10
%     data3(
% end
% %data3=[data2(1,1:L),data2(2,1:L),data2(3,1:L),data2(4,1:L),data2(5,1:L),data2(6,1:L)];
% file_wr=fopen('reshaped.bin','wb');
% fwrite(file_wr,data3,'float32');
% fclose(file_wr);
% %测试
% file_wr_test=fopen('reshaped.bin','rb');
% reshapedData=fread(file_wr_test,'single');
% fclose(file_wr_test);
% plot(reshapedData);
% %data2=data1';
% %aa=data1(:,3)