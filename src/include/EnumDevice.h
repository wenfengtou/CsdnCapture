// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� ENUMDEVICE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// ENUMDEVICE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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

//ע�⣺��ΪDShow��Ҫ�õ�COM�����Ե��øýӿ�ǰ�ⲿ��Ҫ����COM��ĳ�ʼ��������::CoInitialize(NULL);

//ö��ָ�����͵����вɼ��豸������
ENUMDEVICE_API HRESULT EnumDevice(CAPTURE_DEVICE_TYPE type, char * deviceList[], int nListLen, int & iNumCapDevices);
