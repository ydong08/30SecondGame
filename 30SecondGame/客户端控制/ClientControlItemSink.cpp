// ClientControlItemSink.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg �Ի���

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	m_cbWinArea=0;				//Ӯ������
	m_cbExcuteTimes=0;			//ִ�д���
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnExcute)
	ON_BN_CLICKED(IDC_BUTTON_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BUTTON_SYN,OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnCancel)
	ON_BN_CLICKED(IDC_BUTTON_Reset_STOAGE,OnBnClickedResetStorage)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CClientControlItemSinkDlg ��Ϣ�������

//��ʼ��
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	IClientControlDlg::OnInitDialog();

	SetDlgItemText(IDC_STATIC_TIMES,TEXT("���ƾ���:"));
	SetDlgItemText(IDC_STATIC_AREA, TEXT("�������:"));
	SetDlgItemText(IDC_STATIC_NOTIC,TEXT("����˵��:"));
	SetDlgItemText(IDC_STATIC_TEXT, TEXT("������Ӯ���Ʊ���Ϸ�����Ʋ�������!"));
	SetDlgItemText(IDC_RADIO_LT,TEXT("�ж���"));
	SetDlgItemText(IDC_RADIO_LC,TEXT("  ��  "));		
	SetDlgItemText(IDC_RADIO_LB,TEXT("������"));
	SetDlgItemText(IDC_RADIO_CC,TEXT("  ƽ  "));
	SetDlgItemText(IDC_RADIO_CB,TEXT("ͬ��ƽ"));
	SetDlgItemText(IDC_RADIO_RT,TEXT("ׯ����"));
	SetDlgItemText(IDC_RADIO_RC,TEXT("  ׯ  "));
	SetDlgItemText(IDC_RADIO_RB,TEXT("ׯ����"));

	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));


	SetDlgItemText(IDC_BUTTON_RESET,TEXT("ȡ������"));
	SetDlgItemText(IDC_BUTTON_SYN,TEXT("����"));
	SetDlgItemText(IDC_BUTTON_OK,TEXT("ִ��"));
	SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("ȡ��"));

	return TRUE; 
}

void CClientControlItemSinkDlg::OnReSet()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_RESET_CONTROL;
	adminReq.nServalValue = 332333111;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnRefresh()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_PRINT_SYN;
	adminReq.nServalValue = 332333111;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}


void CClientControlItemSinkDlg::OnExcute()
{
	CMD_C_AdminReq adminReq;
	BYTE cbIndex=0;
	int nCheckID;
	for(cbIndex=0;cbIndex<8;cbIndex++)
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_LT+cbIndex))->GetCheck())
		{
			nCheckID=cbIndex+1;
		}
	}

	int nSelect=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();

	if(nCheckID>0&&nSelect>=0)
	{
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		adminReq.cbExtendData[0]=(BYTE)nCheckID;
		adminReq.cbExtendData[1]=(BYTE)nSelect+1;
		m_cbWinArea=adminReq.cbExtendData[0];
		m_cbExcuteTimes=adminReq.cbExtendData[1];

		adminReq.nServalValue = 332333111;

		CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	}
	else
	{
		AfxMessageBox(TEXT("��ѡ���ܿش����Լ��ܿ�����!"));
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CClientControlItemSinkDlg::OnBnClickedResetStorage()
{
	CString strStorage=TEXT(""),strDeduct=TEXT("");
	GetDlgItemText(IDC_EDIT1,strStorage);
	//ȥ���ո�
	strStorage.TrimLeft();
	strStorage.TrimRight();

	GetDlgItemText(IDC_EDIT2,strDeduct);
	//ȥ���ո�
	strDeduct.TrimLeft();
	strDeduct.TrimRight();
	LONGLONG lStorage=0L,lDeduct=0L;
	lStorage = StrToInt(strStorage);
	lDeduct = StrToInt(strDeduct);

	if(lDeduct < 0L || lDeduct > 99L)
	{
		//SetDlgItemText(IDC_STATIC_INFO,TEXT("����ʧ�ܣ�˥��ֵ����0��99"));
		AfxMessageBox(TEXT("����ʧ�ܣ�˥��ֵ����0��99֮��"));
		return;
	}

	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.lStorage=lStorage;
	adminReq.lStorageDeduct=lDeduct;

	//g_pGameClientDlg->OnAdminRestStorage((WPARAM)&adminReq,0);

	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	pGameFrameEngine->SendSocketData(SUB_C_ADMIN_RESET_STORAGE,&adminReq,sizeof(adminReq));;
	//if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_RESET_STORAGE,(WPARAM)&adminReq,0);
}

bool CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{

	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{

	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				switch(m_cbWinArea)
				{
				case 0:str.Format(TEXT("���ܿ�����"));break;
				case 1:str.Format(TEXT("�����ѽ���,ʤ������:�ж���,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 2:str.Format(TEXT("�����ѽ���,ʤ������:  ��  ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 3:str.Format(TEXT("�����ѽ���,ʤ������:������,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 4:str.Format(TEXT("�����ѽ���,ʤ������:  ƽ  ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 5:str.Format(TEXT("�����ѽ���,ʤ������:ͬ��ƽ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 6:str.Format(TEXT("�����ѽ���,ʤ������:ׯ����,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 7:str.Format(TEXT("�����ѽ���,ʤ������:  ׯ  ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 8:str.Format(TEXT("�����ѽ���,ʤ������:ׯ����,ִ�д���:%d"),m_cbExcuteTimes);break;
				default:break;
				}
			}
			else
			{
				str.Format(TEXT("����ʧ��!"));
				m_cbExcuteTimes=0;
				m_cbWinArea=0;
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("���������ѽ���!");
				for(int nIndex=0;nIndex<8;nIndex++)
					((CButton*)GetDlgItem(IDC_RADIO_LT+nIndex))->SetCheck(0);
				if(m_cbExcuteTimes>0)
					((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(-1);
				m_cbWinArea=0;
				m_cbExcuteTimes=0;
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("������ͬ�������ѽ���!");
				for(int nIndex=0;nIndex<8;nIndex++)
					((CButton*)GetDlgItem(IDC_RADIO_LT+nIndex))->SetCheck(0);

				m_cbWinArea=pResult->cbExtendData[0];
				m_cbExcuteTimes=pResult->cbExtendData[1];
				if(m_cbWinArea>0&&m_cbWinArea<=8)
					((CButton*)GetDlgItem(IDC_RADIO_LT+m_cbWinArea-1))->SetCheck(1);
				if(m_cbExcuteTimes>0&&m_cbExcuteTimes<=5)
					((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes-1);

				SetTimer(10,2000,0);
			}
			else
			{
				str.Format(TEXT("����ʧ��!"));
			}
			break;
		}

	default: break;
	}

	SetWindowText(str);
	return true;
}

//���¿��
void __cdecl CClientControlItemSinkDlg::UpDateSystemStorage(CMD_S_SystemStorage * pStorage)
{
	CString str;
	str.Format(TEXT("��ǰϵͳ���:%I64d   ˥��ֵ:%I64d"),pStorage->lStorage,pStorage->lStorageDeduct);
	SetDlgItemText(IDC_STATIC_SYS_STORAGE,str);
}


void CClientControlItemSinkDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	ShowWindow(SW_HIDE);
	//	CDialog::OnCancel();
}

void CClientControlItemSinkDlg::OnTimer(UINT nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	KillTimer(10);
	CMD_S_CommandResult Result;
	Result.cbAckType=ACK_SET_WIN_AREA;
	Result.cbResult=CR_ACCEPT;
	ReqResult(&Result);
	CDialog::OnTimer(nIDEvent);
}

HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_SYS_STORAGE)
	{
		pDC->SetTextColor(RGB(255,10,10)); 
	}
	return hbr;
}
