#ifndef REAL_GAME_LIST_HEAD_FILE
#define REAL_GAME_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"

//游戏记录
class CRealJettonInfo : public CDialog
{
	//控件变量
public:
	CSkinListCtrl					m_JettonList;					//游戏列表

	//函数定义
public:
	//构造函数
	CRealJettonInfo();
	//析构函数
	virtual ~CRealJettonInfo();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息


	//功能函数
public:
	//插入列表
	void InserUser(tagJettonUser & ApplyUser);
	//删除列表
	void DeleteUser(tagJettonUser & ApplyUser);
	//更新列表
	void UpdateUser( tagJettonUser & ApplyUser,bool bInsert=true );

	//查找玩家
	bool FindUser(LPCTSTR lpszUserName);

	int  GetItemCount();
	//清空列表
	void ClearAll();
	//字符转换
	CString LongNumberToString(LONGLONG lNumber);
	//消息映射

	//消息映射
protected:
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
#endif