#ifndef REAL_GAME_LIST_HEAD_FILE
#define REAL_GAME_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"

//��Ϸ��¼
class CRealJettonInfo : public CDialog
{
	//�ؼ�����
public:
	CSkinListCtrl					m_JettonList;					//��Ϸ�б�

	//��������
public:
	//���캯��
	CRealJettonInfo();
	//��������
	virtual ~CRealJettonInfo();

	//���غ���
protected:
	//�ؼ���
	virtual void DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ


	//���ܺ���
public:
	//�����б�
	void InserUser(tagJettonUser & ApplyUser);
	//ɾ���б�
	void DeleteUser(tagJettonUser & ApplyUser);
	//�����б�
	void UpdateUser( tagJettonUser & ApplyUser,bool bInsert=true );

	//�������
	bool FindUser(LPCTSTR lpszUserName);

	int  GetItemCount();
	//����б�
	void ClearAll();
	//�ַ�ת��
	CString LongNumberToString(LONGLONG lNumber);
	//��Ϣӳ��

	//��Ϣӳ��
protected:
	//λ�ñ仯
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
#endif