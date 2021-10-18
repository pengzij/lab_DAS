#ifndef _CU3AD_H
#define _CU3AD_H
/*此SDK内部采用 unicode字符； 使用此SDK的基本流程(并非固定流程)
  1.CU3_Init(...);//初次使用SDK时，初始化SDK内部资源
  2.CU3_OpenDev(...);//打开设备
  3.CU3_SetSamParam(...);//设置采样参数
  4.CU3_StartAD(...);//开始采样
  5.CU3_ReadADRecords(...);
  6.CU3_StopAD(...);
  7.CU3_CloseDev(...);
  8.CU3_Release(...);//不再使用SDK时，释放SDK内部资源
  其中:控制过程可以和采集过程可以用两个线程
*/

#define CU3API _stdcall

const int CU3_MAX_CHAN_COUNT = 16;
struct CU3_RECORD_META
{
    WORD nSwitchNum;//分组                4
    DWORD iRecordSn;//组内记录号(从1开始) 2
    int nChannelCount;                   // 4
    int nRecordLenOfChans[CU3_MAX_CHAN_COUNT];   //64
};

//采样参数
struct CU3_SAM_PARA
{
	int nBitCount;
	double nFreq;
	int nChannelCount;
	int nRecordLenOfChans[CU3_MAX_CHAN_COUNT];
};

const int DPI_RAW_ReadSumSize = 0, DPI_RAW_ReadSumErrorCount = 1;
const int DPI_ADSumErrorCount = 0x100;

//初始化SDK内部资源
extern "C" bool CU3API CU3_Init();

//释放SDK内部资源
extern "C" void CU3API CU3_Release();

/*返回错误信息
  hDev:设备句柄，如果设备句柄无效，返回-1.
  strError:错误信息缓冲区
  nSize:错误信息缓冲区可以容纳的字符数（缓冲区大小）
  返回值:当strError=NULL时，返回需要的缓冲区大小；
  如果缓冲区足够大, 则返回实际大小;如果缓冲区不够大返回-1*/
extern "C" int CU3API CU3_GetLastError(HANDLE hDev, TCHAR* strError, int nSize);

/*打开设备
  strDevID:设备ID
  返回值:成功时返回非空(NULL), 失败返回NULL*/
extern "C" HANDLE CU3API CU3_OpenDev(LPCTSTR strDevID);

//关闭采集卡
extern "C" bool CU3API CU3_CloseDev(HANDLE hDev);

//获取参数
extern "C" int CU3API CU3_GetParam(HANDLE hDev, int nIndex, void* buffer, int len);

//设置参数
extern "C" bool CU3API CU3_SetParam(HANDLE hDev, int nIndex, const void* buffer, int len);

//设置采集参数
extern "C" bool CU3API CU3_SetSamParam(HANDLE hDev, const CU3_SAM_PARA *pSamParam);

//初始化AD采样(此函数已作废,保留此函数仅为接口兼容)
extern "C" bool CU3API CU3_InitAD(HANDLE hDev);

//启动AD采样
extern "C" bool CU3API CU3_StartAD(HANDLE hDev);

//停止AD采样
extern "C" bool CU3API CU3_StopAD(HANDLE hDev);

/*读取采样记录
  hDev:设备句柄
  buffer: 记录数据缓冲区   
  nRecordCount:希望读取的记录数
  recordMetas:记录元信息
  返回值: >=0时表示实际读取到的记录数，<0 表示失败*/
extern "C" int CU3API CU3_ReadADRecords(HANDLE hDev, BYTE* buffer, int nRecordCount, CU3_RECORD_META* recordMetas);

#endif
