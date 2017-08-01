#include "StdAfx.h"
#include "Resource.h"
#include "RealJettonInfo.h"


BEGIN_MESSAGE_MAP(CRealJettonInfo, CDialog)
END_MESSAGE_MAP()

#define AREA_COUNT 8

//���캯��
CRealJettonInfo::CRealJettonInfo() : CDialog(IDD_DLG_JETTON_INFO)
{
}

//��������
CRealJettonInfo::~CRealJettonInfo()
{
}

//�ؼ���
void CRealJettonInfo::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_JETTON_INFO, m_JettonList);
}


//��ʼ������
BOOL CRealJettonInfo::OnInitDialog()
{
	__super::OnInitDialog();

	//������ɫ
	m_JettonList.SetTextColor(RGB(52,87,130));
	m_JettonList.SetBkColor(RGB(255,255,255));
	m_JettonList.SetTextBkColor(RGB(255,255,255));

	//�����б�
	m_JettonList.InsertColumn(0,TEXT("�����Ϣ"),LVCFMT_LEFT,60);
	m_JettonList.InsertColumn(1,TEXT("�м�"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(2,TEXT("ƽ"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(3,TEXT("ׯ��"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(4,TEXT("������"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(5,TEXT("ׯ����"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(6,TEXT("ͬ��ƽ"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(7,TEXT("�ж���"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(8,TEXT("ׯ����"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(9,TEXT("��ע"),LVCFMT_LEFT,90);

	//��������Ϣ
	m_JettonList.SetRedraw(TRUE);

	ListView_SetExtendedListViewStyle
		(m_JettonList.m_hWnd, LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT|LVS_EX_FLATSB | LVS_EX_GRIDLINES );

	return FALSE;
}


int  CRealJettonInfo::GetItemCount()
{
	return m_JettonList.GetItemCount();
}
//�����б�
void CRealJettonInfo::InserUser(tagJettonUser & ApplyUser)
{
	//��������
	TCHAR szBuffer[128]=TEXT("");
	LONGLONG lAllJetton=0;

	//��Ϸ���
	INT nItemIndex=m_JettonList.InsertItem(m_JettonList.GetItemCount(),ApplyUser.strUserName);
	for(int i=0;i<AREA_COUNT;i++)
	{
		//�����ע���
		memset(szBuffer,0,sizeof(szBuffer));
		_stprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),LongNumberToString(ApplyUser.lUserJettonScore[i]));
		m_JettonList.SetItemText(nItemIndex,i+1,szBuffer);
		lAllJetton += ApplyUser.lUserJettonScore[i];
	}
	_stprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),LongNumberToString(lAllJetton));
	m_JettonList.SetItemText(nItemIndex,AREA_COUNT+1,szBuffer);
	UpdateData(FALSE);
	return;
}
//�����б�
void CRealJettonInfo::UpdateUser( tagJettonUser & ApplyUser ,bool bInsert )
{
	//�������
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)ApplyUser.strUserName;

	//��������
	int nItem = m_JettonList.FindItem( &lvFindInfo );

	//ɾ������
	if ( nItem != -1 ) 
	{
		TCHAR szBuffer[128]=TEXT("");
		LONGLONG lAllJetton=0;

		for(int i=0;i<AREA_COUNT;i++)
		{
			//�����ע���
			memset(szBuffer,0,sizeof(szBuffer));
			_stprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),LongNumberToString(ApplyUser.lUserJettonScore[i]));
			m_JettonList.SetItemText(nItem,i+1,szBuffer);
			lAllJetton += ApplyUser.lUserJettonScore[i];
		}
		_stprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),LongNumberToString(lAllJetton));
		m_JettonList.SetItemText(nItem,AREA_COUNT+1,szBuffer);
	}
	else
	{
		if (bInsert)
		{

			//��������
			TCHAR szBuffer[128]=TEXT("");
			LONGLONG lAllJetton=0;
			//��Ϸ���
			INT nItemIndex=m_JettonList.InsertItem(m_JettonList.GetItemCount(),ApplyUser.strUserName);
			for(int i=0;i<AREA_COUNT;i++)
			{
				//�����ע���
				memset(szBuffer,0,sizeof(szBuffer));
				_stprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),LongNumberToString(ApplyUser.lUserJettonScore[i]));
				m_JettonList.SetItemText(nItemIndex,i+1,szBuffer);
				lAllJetton += ApplyUser.lUserJettonScore[i];
			}
			_stprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),LongNumberToString(lAllJetton));
			m_JettonList.SetItemText(nItemIndex,AREA_COUNT+1,szBuffer);
		}
	}
	UpdateData(FALSE);
}

//�������
bool CRealJettonInfo::FindUser(LPCTSTR lpszUserName)
{
	//�������
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags=LVFI_STRING;
	lvFindInfo.psz=lpszUserName;

	//��������
	int nItem=m_JettonList.FindItem(&lvFindInfo);

	//���ؽ��
	return -1==nItem ? false : true;
}


//ɾ���б�
void CRealJettonInfo::DeleteUser(tagJettonUser & ApplyUser)
{
	//�������
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)ApplyUser.strUserName;

	//��������
	int nItem = m_JettonList.FindItem( &lvFindInfo );

	//ɾ������
	if ( nItem != -1 ) m_JettonList.DeleteItem( nItem );
	UpdateData(FALSE);
}

//����б�
void CRealJettonInfo::ClearAll()
{
	m_JettonList.DeleteAllItems();
}
CString CRealJettonInfo::LongNumberToString(LONGLONG lNumber)
{
	CString strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
	if (lNumber==0) strNumber=TEXT("0");
	int nNumberCount=0;
	LONGLONG lTmpNumber=lNumber;
	if (lNumber<0) lNumber=-lNumber;
	while (lNumber>0)
	{
		strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if (nNumberCount==4)
		{
			if(strNumber.GetLength() > 1)
				strTmpNumber2 += (TEXT(",") +strNumber);
			strNumber=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		lNumber/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		if(strNumber.GetLength() > 1)
			strTmpNumber2 += (TEXT(",") +strNumber);
		strNumber=strTmpNumber2;
	}

	if (lTmpNumber<0) strNumber=TEXT("-")+strNumber;
	return strNumber;
}
