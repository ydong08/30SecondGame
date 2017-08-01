#ifndef USER_JETTON_MANAGER_HEAD_FILE
#define USER_JETTON_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

class CUserJettonManager
{
protected:
	SCORE						m_lAllJettonScore[AREA_MAX];				//总下注数
	SCORE						m_lUserJettonScore[GAME_PLAYER][AREA_MAX];	//个人总注
	SCORE						m_lUserFleeScore;								//玩家逃跑分数，应算给庄家

public:
	CUserJettonManager();
	virtual ~CUserJettonManager();

public:
	void Reset();
	SCORE GetAllJettonScore(BYTE bArea=AREA_RESERVE);
	SCORE GetUserJettonScore(WORD wChairID,BYTE bArea=AREA_RESERVE);
	INT GetUserJettonCount(WORD wChairID,BYTE bSkipArea=AREA_RESERVE);
	bool SetUserJettonScore(WORD wChairID,BYTE bArea,SCORE lScore);
	bool ModifyUserJettonScore(WORD wChairID,BYTE bArea,SCORE lScore);
	bool SetAllJettonScore(BYTE bArea,SCORE lScore);
	bool ModifyAllJettonScore(BYTE bArea,SCORE lScore);
	bool CopyAllJettonScore(SCORE lCopyScore[],WORD wCopySize);
	bool CopyUserJettonScore(WORD wChairID,SCORE lCopyScore[],WORD wCopySize);
	void ResetUserJettonScore(WORD wChairID=INVALID_CHAIR);
	void ModifyUserFleeScore(SCORE lScore);
	SCORE GetUserFleeScore();
};



#endif