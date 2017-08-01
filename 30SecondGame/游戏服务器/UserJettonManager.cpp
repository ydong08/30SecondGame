#include "Stdafx.h"
#include "UserJettonManager.h"

CUserJettonManager::CUserJettonManager()
{
	Reset();
}

CUserJettonManager::~CUserJettonManager()
{

}

void CUserJettonManager::Reset()
{
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));
	m_lUserFleeScore=0ll;
}

SCORE CUserJettonManager::GetAllJettonScore(BYTE bArea)
{
	if (bArea<0 || (bArea>=AREA_MAX && bArea!=AREA_RESERVE))
	{
		return 0ll;
	}

	if (bArea==AREA_RESERVE)
	{
		SCORE llAllJettonScore=0ll;
		for (BYTE nAreaIndex=0;nAreaIndex<AREA_MAX;nAreaIndex++)
		{
			if (m_lAllJettonScore[nAreaIndex]==0ll) continue;
			llAllJettonScore+=m_lAllJettonScore[nAreaIndex];
		}
		return llAllJettonScore;
	}
	
	return m_lAllJettonScore[bArea];
}

SCORE CUserJettonManager::GetUserJettonScore(WORD wChairID,BYTE bArea)
{
	if (bArea<0 || (bArea>=AREA_MAX && bArea!=AREA_RESERVE) || wChairID==INVALID_CHAIR)
	{
		return 0ll;
	}
	
	if (bArea==AREA_RESERVE)
	{
		SCORE llUserJettonScore=0ll;
		for (BYTE nAreaIndex=0;nAreaIndex<AREA_MAX;nAreaIndex++)
		{
			if (m_lUserJettonScore[wChairID][nAreaIndex]==0ll) continue;

			llUserJettonScore+=m_lUserJettonScore[wChairID][nAreaIndex];
		}
		return llUserJettonScore;
	}

	return m_lUserJettonScore[wChairID][bArea];
}

bool CUserJettonManager::SetUserJettonScore(WORD wChairID,BYTE bArea,SCORE lScore)
{
	if (bArea<0 || (bArea>=AREA_MAX && bArea!=AREA_RESERVE)  || wChairID==INVALID_CHAIR || lScore<0)
	{
		return false;
	}

	m_lUserJettonScore[wChairID][bArea] = lScore;
	return true;
}

bool CUserJettonManager::ModifyUserJettonScore(WORD wChairID,BYTE bArea,SCORE lScore)
{
	if (bArea<0 || (bArea>=AREA_MAX && bArea!=AREA_RESERVE)  || wChairID==INVALID_CHAIR)
	{
		return false;
	}

	m_lUserJettonScore[wChairID][bArea] += lScore;
	return true;
}

INT CUserJettonManager::GetUserJettonCount(WORD wChairID,BYTE bSkipArea)
{
	if (wChairID==INVALID_CHAIR)
	{
		return 0;
	}

	INT nJettonCount = 0;

	for (BYTE nAreaIndex=AREA_BEGIN;nAreaIndex<AREA_MAX;nAreaIndex++)
	{
		if (bSkipArea!=AREA_RESERVE && bSkipArea==nAreaIndex)
		{
			continue;
		}
		if (GetUserJettonScore(wChairID,nAreaIndex)>0ll)
		{
			nJettonCount++;
		}
	}
	return nJettonCount;
}

bool CUserJettonManager::SetAllJettonScore(BYTE bArea,SCORE lScore)
{
	if (bArea<0 || (bArea>=AREA_MAX && bArea!=AREA_RESERVE)  || lScore<0)
	{
		return false;
	}

	m_lAllJettonScore[bArea] = lScore;
	return true;
}

bool CUserJettonManager::ModifyAllJettonScore(BYTE bArea,SCORE lScore)
{
	if (bArea<0 || (bArea>=AREA_MAX && bArea!=AREA_RESERVE) )
	{
		return false;
	}

	m_lAllJettonScore[bArea] += lScore;
	return true;
}

bool CUserJettonManager::CopyAllJettonScore(SCORE lCopyScore[],WORD wCopySize)
{
	if (wCopySize>sizeof(m_lAllJettonScore))
	{
		return false;
	}
	CopyMemory(lCopyScore,m_lAllJettonScore,wCopySize);
	return true;
}

bool CUserJettonManager::CopyUserJettonScore(WORD wChairID,SCORE lCopyScore[],WORD wCopySize)
{
	if (wCopySize>sizeof(m_lUserJettonScore[wChairID]) || wChairID==INVALID_CHAIR)
	{
		return false;
	}
	CopyMemory(lCopyScore,m_lUserJettonScore[wChairID],wCopySize);
	return true;
}

void CUserJettonManager::ResetUserJettonScore(WORD wChairID)
{
	if (wChairID==INVALID_CHAIR)
	{
		ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));
	}
	else
	{
		ZeroMemory(m_lUserJettonScore[wChairID],sizeof(m_lUserJettonScore[wChairID]));
	}
}

void CUserJettonManager::ModifyUserFleeScore(SCORE lScore)
{
	m_lUserFleeScore+=lScore;
}

SCORE CUserJettonManager::GetUserFleeScore()
{
	return m_lUserFleeScore;
}