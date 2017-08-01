#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
}

//��������
CDlgCustomRule::~CDlgCustomRule()
{
}

//�ؼ���
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_1,m_CustomConfig.lApplyBankerCondition);
	DDX_Text(pDX,IDC_EDIT_2,m_CustomConfig.lBankerMAX);
	DDX_Text(pDX,IDC_EDIT_3,m_CustomConfig.lBankerAdd);
	DDX_Text(pDX,IDC_EDIT_4,m_CustomConfig.lBankerScoreMAX);
	DDX_Text(pDX,IDC_EDIT_5,m_CustomConfig.lBankerScoreAdd);
	DDX_Text(pDX,IDC_EDIT_6,m_CustomConfig.cbFreeTime);
	DDX_Text(pDX,IDC_EDIT_7,m_CustomConfig.cbBetTime);
	DDX_Text(pDX,IDC_EDIT_8,m_CustomConfig.cbEndTime);
	DDX_Text(pDX,IDC_EDIT_9,m_CustomConfig.StorageStart);
	DDX_Text(pDX,IDC_EDIT_10,m_CustomConfig.StorageDeduct);
	DDX_Text(pDX,IDC_EDIT_11,m_CustomConfig.nServiceCharge);
	DDX_Text(pDX,IDC_EDIT_12,m_CustomConfig.lAreaLimitScore);
	DDX_Text(pDX,IDC_EDIT_13,m_CustomConfig.lUserLimitScore);
	DDX_Check(pDX,IDC_CHECK1,m_CustomConfig.nEnableSysBanker);


	DDV_MinMaxLongLong(pDX,m_CustomConfig.lApplyBankerCondition, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.lBankerMAX, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.lBankerAdd, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.lBankerScoreMAX, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.lBankerScoreAdd, 0, LLONG_MAX);
	DDV_MinMaxByte(pDX,m_CustomConfig.cbFreeTime, 10, 255);
	DDV_MinMaxByte(pDX,m_CustomConfig.cbBetTime, 10, 255);
	DDV_MinMaxByte(pDX,m_CustomConfig.cbEndTime, 20, 255);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.StorageStart, -10000000000, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.StorageDeduct, 0, 1000);
	DDV_MinMaxInt(pDX,m_CustomConfig.nServiceCharge, 0, INT_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.lAreaLimitScore, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_CustomConfig.lUserLimitScore, 0, LLONG_MAX);

}

//��ʼ������
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//ȷ������
VOID CDlgCustomRule::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgCustomRule::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//��������
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//���ñ���
	m_wCustomSize=wCustomSize;
	m_pcbCustomRule=pcbCustomRule;

	//���ñ���
	ASSERT(m_pcbCustomRule);
	if( !m_pcbCustomRule ) return false;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
	if( pCustomConfig->lApplyBankerCondition != 0 )
		memcpy(&m_CustomConfig, pCustomConfig, sizeof(tagCustomConfig));

	//���½���
	if( m_hWnd )
		UpdateData(TRUE);

	return true;
}

//��������
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//���½���
	if( m_hWnd )
		UpdateData(TRUE);

	//���ñ���
	m_wCustomSize=wCustomSize;
	m_pcbCustomRule=pcbCustomRule;

	//���ñ���
	ASSERT(m_pcbCustomRule);
	if( !m_pcbCustomRule ) return true;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
	memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	return true;
}

//��������
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//���ñ���
	m_wCustomSize=wCustomSize;
	m_pcbCustomRule=pcbCustomRule;

	//���ñ���
	ASSERT(m_pcbCustomRule);
	if( !m_pcbCustomRule ) return true;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;

	m_CustomConfig.DefaultCustomRule();
	memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	//���½���
	if( m_hWnd )
		UpdateData(TRUE);

	return true;
}
