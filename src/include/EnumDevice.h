// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 ENUMDEVICE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// ENUMDEVICE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef ENUMDEVICE_EXPORTS
#define ENUMDEVICE_API __declspec(dllexport)
#else
#define ENUMDEVICE_API __declspec(dllimport)
#endif



enum CAPTURE_DEVICE_TYPE
{
	DSHOW_VIDEO_DEVICE = 0,
	DSHOW_AUDIO_DEVICE,
};

//注意：因为DShow需要用到COM，所以调用该接口前外部需要调用COM库的初始化函数：::CoInitialize(NULL);

//枚举指定类型的所有采集设备的名称
ENUMDEVICE_API HRESULT EnumDevice(CAPTURE_DEVICE_TYPE type, char * deviceList[], int nListLen, int & iNumCapDevices);
