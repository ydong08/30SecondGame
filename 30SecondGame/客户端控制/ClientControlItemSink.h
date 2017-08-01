#pragma once
#include "../��Ϸ�ͻ���/ClientControl.h"


// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	BYTE m_cbWinArea;				//Ӯ������
	BYTE m_cbExcuteTimes;			//ִ�д���

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//������
	virtual bool __cdecl ReqResult(const void * pBuffer);
	//���¿��
	virtual void __cdecl UpDateSystemStorage(CMD_S_SystemStorage * pStorage);
	virtual BOOL OnInitDialog();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void OnBnClickedResetStorage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	virtual void OnCancel();

public:
	afx_msg void OnTimer(UINT nIDEvent);
};
