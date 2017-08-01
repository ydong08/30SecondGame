#include "StdAfx.h"
#include "Resource.h"
#include "RealJettonInfo.h"


BEGIN_MESSAGE_MAP(CRealJettonInfo, CDialog)
END_MESSAGE_MAP()

#define AREA_COUNT 8

//构造函数
CRealJettonInfo::CRealJettonInfo() : CDialog(IDD_DLG_JETTON_INFO)
{
}

//析构函数
CRealJettonInfo::~CRealJettonInfo()
{
}

//控件绑定
void CRealJettonInfo::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_JETTON_INFO, m_JettonList);
}


//初始化函数
BOOL CRealJettonInfo::OnInitDialog()
{
	__super::OnInitDialog();

	//设置颜色
	m_JettonList.SetTextColor(RGB(52,87,130));
	m_JettonList.SetBkColor(RGB(255,255,255));
	m_JettonList.SetTextBkColor(RGB(255,255,255));

	//插入列表
	m_JettonList.InsertColumn(0,TEXT("玩家信息"),LVCFMT_LEFT,60);
	m_JettonList.InsertColumn(1,TEXT("闲家"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(2,TEXT("平"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(3,TEXT("庄家"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(4,TEXT("闲天王"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(5,TEXT("庄天王"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(6,TEXT("同点平"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(7,TEXT("闲对子"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(8,TEXT("庄对子"),LVCFMT_LEFT,80);
	m_JettonList.InsertColumn(9,TEXT("总注"),LVCFMT_LEFT,90);

	//插入新信息
	m_JettonList.SetRedraw(TRUE);

	ListView_SetExtendedListViewStyle
		(m_JettonList.m_hWnd, LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT|LVS_EX_FLATSB | LVS_EX_GRIDLINES );

	return FALSE;
}


int  CRealJettonInfo::GetItemCount()
{
	return m_JettonList.GetItemCount();
}
//插入列表
void CRealJettonInfo::InserUser(tagJettonUser & ApplyUser)
{
	//变量定义
	TCHAR szBuffer[128]=TEXT("");
	LONGLONG lAllJetton=0;

	//游戏玩家
	INT nItemIndex=m_JettonList.InsertItem(m_JettonList.GetItemCount(),ApplyUser.strUserName);
	for(int i=0;i<AREA_COUNT;i++)
	{
		//玩家下注金币
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
//更新列表
void CRealJettonInfo::UpdateUser( tagJettonUser & ApplyUser ,bool bInsert )
{
	//构造变量
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)ApplyUser.strUserName;

	//查找子项
	int nItem = m_JettonList.FindItem( &lvFindInfo );

	//删除子项
	if ( nItem != -1 ) 
	{
		TCHAR szBuffer[128]=TEXT("");
		LONGLONG lAllJetton=0;

		for(int i=0;i<AREA_COUNT;i++)
		{
			//玩家下注金币
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

			//变量定义
			TCHAR szBuffer[128]=TEXT("");
			LONGLONG lAllJetton=0;
			//游戏玩家
			INT nItemIndex=m_JettonList.InsertItem(m_JettonList.GetItemCount(),ApplyUser.strUserName);
			for(int i=0;i<AREA_COUNT;i++)
			{
				//玩家下注金币
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

//查找玩家
bool CRealJettonInfo::FindUser(LPCTSTR lpszUserName)
{
	//构造变量
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags=LVFI_STRING;
	lvFindInfo.psz=lpszUserName;

	//查找子项
	int nItem=m_JettonList.FindItem(&lvFindInfo);

	//返回结果
	return -1==nItem ? false : true;
}


//删除列表
void CRealJettonInfo::DeleteUser(tagJettonUser & ApplyUser)
{
	//构造变量
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)ApplyUser.strUserName;

	//查找子项
	int nItem = m_JettonList.FindItem( &lvFindInfo );

	//删除子项
	if ( nItem != -1 ) m_JettonList.DeleteItem( nItem );
	UpdateData(FALSE);
}

//清空列表
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
