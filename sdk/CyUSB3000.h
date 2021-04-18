#ifndef _CyUSB300_H
#define _CyUSB300_H

const int CyUSB3000_MAX_DEV_COUNT = 64;
///read flag /////
const UINT damShallowRead = 0x0001;
///write flag 
const UINT damShallowWrite = 0x0001;
///param index
const int  DPI_ReadSumSize = 0, DPI_ReadSumErrorCount = 1, DPI_BufSizeOfRead=2, DPI_FastBufSizeOfRead=3;
typedef TCHAR CyUSB3000_DevID[128];
#define CYUAPI _stdcall
//得到错误信息
extern "C" int CYUAPI CyUSB3000_GetLastError(HANDLE hDev, TCHAR* strError, int nSize);
//列举设备
extern "C" int CYUAPI CyUSB3000_EnumDevices(CyUSB3000_DevID *strDevIDs, int nCount);
//创建设备句柄, 失败返回NULL
extern "C" HANDLE CYUAPI CyUSB3000_CreateDevice(DWORD dwReserve);
//销毁设备句柄
extern "C" bool  CYUAPI CyUSB3000_DestroyDevice(HANDLE hDev);
//获取设备参数
extern "C" int  CYUAPI CyUSB3000_GetParam(HANDLE hDev, int index, void* buffer, int len);
//设置设备参数
extern "C" bool  CYUAPI CyUSB3000_SetParam(HANDLE hDev, int index, const void* buffer, int len);
//打开设备
extern "C" bool  CYUAPI CyUSB3000_OpenDevice(HANDLE hDev, LPCTSTR strDevID, DWORD dwMode);
//关闭设备
extern "C" bool CYUAPI CyUSB3000_CloseDevice(HANDLE hDev);
//从设备中读取数据
extern "C" int CYUAPI CyUSB3000_Read(HANDLE hDev, void* buffer, int len, UINT uFlag);
//往设备中写入数据
extern "C" int CYUAPI CyUSB3000_Write(HANDLE hDev, const void* buffer, int len, UINT uFlag);

#endif