#ifndef WAVHEAD_H
#define WAVHEAD_H

struct WavHead
{
    /*--------------------------------------------RIFF chunk 12 Byte-----------------------------------------*/
    char RIFF[4] = {'R','I','F','F'};    //头部分那个RIFF,4Byte
    long int RIFFSize;//存的是后面所有文件大小,4Byte
    char WAVE[4] = {'W','A','V','E'};//取值"WAVE",4Byte

    /*--------------------------------------------fmt chunk 24 Byte-----------------------------------------*/
    char FMT[4] = {'f','m','t',' '};//取值"fmt"，4Byte
    long int FMTSize;//chkLen,取值16或18，4Byte
    short int AudioFormat;//PCM格式时，取值1，2Byte
    short int Channel = 1;//声道数，取值1或2，2Byte
    long int SampleRate;//采样率,fre，4Byte
    long int ByteRate;//平均字节率，4Byte
    short int BlockAlign;//数据块对齐，非负短整数,2Byte
    short int BitPerSample;//采样位数（PCM才有），2Byte

    /*--------------------------------------------扩展格式块-----------------------------------------*/
    //short int extendSize;
    /*--------------------------------------------fact chunk 12 Byte-----------------------------------------*/
    char FACT[4] = {'f','a','c','t'};//4Byte
    long int FactSize = 4;//数据域长度，4Byte
    long int SamplePiontNum;//每声道采样总数，4Byte

    /*--------------------------------------------data chunk 8 Byte -----------------------------------------*/
    char DATA[4] = {'d','a','t','a'};//取值'data'，4Byte
    long int DATASize;//声音波形数据长度，4Byte
};
#endif // WAVHEAD_H
