#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrameSink.h"

//�Զ�������
struct tagCustomConfig
{
	//��ׯ��Ϣ
	LONGLONG						lApplyBankerCondition;			//��������
	LONGLONG						lBankerMAX;						//���ׯ����
	LONGLONG						lBankerAdd;						//ׯ��������
	LONGLONG						lBankerScoreMAX;				//ׯ��Ǯ
	LONGLONG						lBankerScoreAdd;				//ׯ��Ǯ��ʱ,��ׯ������
	BOOL							nEnableSysBanker;				//ϵͳ��ׯ

	//ʱ��
	BYTE							cbFreeTime;						//����ʱ��
	BYTE							cbBetTime;						//��עʱ��
	BYTE							cbEndTime;						//����ʱ��

	//���
	LONGLONG						StorageStart;					//��������ÿ���ӵĿ����ֵ����ȡʧ�ܰ� 0 ����
	LONGLONG						StorageDeduct;					//ÿ����Ϸ������۳��Ŀ���������ȡʧ�ܰ� 1.00 ����

	//�����
	int								nServiceCharge;					//�����

	//��������
	LONGLONG						lAreaLimitScore;				//��������
	LONGLONG						lUserLimitScore;				//��������


	//���캯��
	tagCustomConfig()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		lApplyBankerCondition = 10000000;
		lBankerMAX = 10;
		lBankerAdd = 10;
		lBankerScoreMAX = 100000000;
		lBankerScoreAdd = 10;
		nEnableSysBanker = TRUE;

		cbFreeTime = 10;
		cbBetTime = 15;
		cbEndTime = 20;

		StorageStart = 0;
		StorageDeduct = 1;

		nServiceCharge = 0;

		lAreaLimitScore = 1000000000;
		lUserLimitScore = 100000000;
	}
};


class CDlgCustomRule : public CDialog
{
	//���ñ���
protected:
	WORD							m_wCustomSize;						//���ô�С
	LPBYTE							m_pcbCustomRule;					//���û���

	//���ýṹ
protected:
	tagCustomConfig					m_CustomConfig;						//�Զ�����

	//��������
public:
	//���캯��
	CDlgCustomRule();
	//��������
	virtual ~CDlgCustomRule();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���ܺ���
public:
	//��������
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//��������
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//Ĭ������
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);

	DECLARE_MESSAGE_MAP()
};
//////////////////////////////////////////////////////////////////////////

#endif