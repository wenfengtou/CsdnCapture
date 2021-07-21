// SelectCaptureDeviceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "SelectCaptureDeviceDlg.h"

#include "EnumDevice.h"
#pragma comment(lib, "EnumDevice.lib")

// CSelectCaptureDeviceDlg �Ի���

IMPLEMENT_DYNAMIC(CSelectCaptureDeviceDlg, CDialog)

CSelectCaptureDeviceDlg::CSelectCaptureDeviceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCaptureDeviceDlg::IDD, pParent)
{

}

CSelectCaptureDeviceDlg::~CSelectCaptureDeviceDlg()
{
}

void CSelectCaptureDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectCaptureDeviceDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelectCaptureDeviceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_VIDEODEV, &CSelectCaptureDeviceDlg::OnCheckEnableVideodev)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_AUDIODEV, &CSelectCaptureDeviceDlg::OnCheckEnableAudiodev)
END_MESSAGE_MAP()

//Show list of devices  
void CSelectCaptureDeviceDlg::show_dshow_device()
{
#if 0
    AVFormatContext *pFmtCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options, "list_devices", "true", 0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    //printf("Device Info=============\n");
    avformat_open_input(&pFmtCtx, "video=dummy", iformat, &options);
    //printf("========================\n");
#endif

	::CoInitialize(NULL); //����DirectShow SDK��API��Ҫ�õ�COM��


    int iVideoCapDevNum = 0;
	int iAudioCapDevNum = 0;

	char * DevicesArray[20];
	for(int i=0; i<20; i++)
	{
		DevicesArray[i] = new char[256];
		memset(DevicesArray[i], 0, 256);
	}

	HRESULT hr;
	hr = EnumDevice(DSHOW_VIDEO_DEVICE, DevicesArray, sizeof(DevicesArray)/sizeof(DevicesArray[0]), iVideoCapDevNum);
	if(hr == S_OK)
	{
		for(int i=0; i<iVideoCapDevNum; i++)
		{
			CString strDevName = DevicesArray[i];
			((CComboBox*)GetDlgItem(IDC_COMBO_VIDEO_DEVICES))->AddString(strDevName);
		}
	}

	hr = EnumDevice(DSHOW_AUDIO_DEVICE, DevicesArray, sizeof(DevicesArray)/sizeof(DevicesArray[0]), iAudioCapDevNum);
	if(hr == S_OK)
	{
		for(int i=0; i<iAudioCapDevNum; i++)
		{
			CString strDevName = DevicesArray[i];
			((CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICES))->AddString(strDevName);
		}
	}

	for(int i=0; i<20; i++)
	{
		delete DevicesArray[i];
		DevicesArray[i] = NULL;
	}

	if(iVideoCapDevNum > 0)
	{
	  ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_VIDEODEV))->SetCheck(TRUE);
	}
	
	if(iAudioCapDevNum > 0)
	{
	   ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_AUDIODEV))->SetCheck(TRUE);
	}

	CoUninitialize();

}

// CSelectCaptureDeviceDlg ��Ϣ�������
BOOL CSelectCaptureDeviceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	show_dshow_device();

	((CComboBox*)GetDlgItem(IDC_COMBO_VIDEO_DEVICES))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICES))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSelectCaptureDeviceDlg::OnCheckEnableVideodev()
{
	BOOL bCapVideo = ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_VIDEODEV))->GetCheck();

	((CComboBox*)GetDlgItem(IDC_COMBO_VIDEO_DEVICES))->EnableWindow(bCapVideo ? TRUE : FALSE);
}

void CSelectCaptureDeviceDlg::OnCheckEnableAudiodev()
{
	BOOL bCapAudio = ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_AUDIODEV))->GetCheck();

	((CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICES))->EnableWindow(bCapAudio ? TRUE : FALSE);
}

void CSelectCaptureDeviceDlg::OnBnClickedOk()
{
	BOOL bCapVideo = ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_VIDEODEV))->GetCheck();
	BOOL bCapAudio = ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_AUDIODEV))->GetCheck();

	m_strVideoDevice = _T("");
	m_strAudioDevice = _T("");

	if(bCapVideo)
	{
		int nCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO_VIDEO_DEVICES))->GetCurSel();
		if(nCurSel >= 0)
		{
		   ((CComboBox*)GetDlgItem(IDC_COMBO_VIDEO_DEVICES))->GetLBText(nCurSel, m_strVideoDevice);
		}
	}

	if(bCapAudio)
	{
		int nCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICES))->GetCurSel();
		if(nCurSel >= 0)
		{
		   ((CComboBox*)GetDlgItem(IDC_COMBO_AUDIO_DEVICES))->GetLBText(nCurSel, m_strAudioDevice);
		}
	}

	if(m_strVideoDevice.IsEmpty() && m_strAudioDevice.IsEmpty())
	{
		MessageBox(_T("��û��ѡ���κ��豸"), _T("��ʾ"), MB_OK|MB_ICONWARNING);
		return;
	}

	OnOK();
}