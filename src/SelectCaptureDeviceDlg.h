#pragma once


// CSelectCaptureDeviceDlg �Ի���

class CSelectCaptureDeviceDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectCaptureDeviceDlg)

public:
	CSelectCaptureDeviceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelectCaptureDeviceDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SELECT_DEVICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	void show_dshow_device();

	afx_msg void OnCheckEnableVideodev();
	afx_msg void OnCheckEnableAudiodev();

public:
	CString  m_strVideoDevice;
	CString  m_strAudioDevice;
};
