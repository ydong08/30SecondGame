#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FREE					99									//����ʱ��
#define IDI_PLACE_JETTON			100									//��עʱ��
#define IDI_DISPATCH_CARD			301									//����ʱ��
#define IDI_PLACE_JETTON_BUFFER		302									//����ʱ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_PALY_BET,OnPlayBet)
	ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_MESSAGE(IDM_ADMIN_COMMDN,OnAdminCommand)
	ON_MESSAGE(IDM_PLAY_SOUND,OnPlaySound)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientEngine::CGameClientEngine() : CGameFrameEngine()
{
	//������Ϣ
	m_lPlayBetScore = 0L;			
	m_lPlayFreeSocre = 0L;		

	//��ע��
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	ZeroMemory(m_lUserJettonDetail,sizeof(m_lUserJettonDetail));

	//ׯ����Ϣ
	m_wBankerUser = INVALID_CHAIR;
	m_lBankerScore = 0l;
	m_lBankerWinScore = 0l;
	m_wBankerTime = 0;

	//ϵͳ��ׯ
	m_bEnableSysBanker = false;

	//״̬����
	m_bMeApplyBanker = false;
	m_bBackGroundSound = false;

	//������Ϣ
	m_lAreaLimitScore = 0L;	
	m_lApplyBankerCondition = 0l;


	return;
}

//��������
CGameClientEngine::~CGameClientEngine()
{
}

//��ʼ����
bool CGameClientEngine::OnInitGameEngine()
{
	//���ñ���
	SetWindowText(TEXT("�ټ�����Ϸ  --  Ver��6.6.1.0"));

	//��������
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	if (m_pIClientKernel!=NULL) m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	//����ͼ��
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//��������
	//m_DTSDBackground.InitDirectSound(this);
	//m_dwBackID = m_DTSDBackground.Create(TEXT("BACK_GROUND"));

	m_PlaceBetArray.RemoveAll();


	//��������
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if ( pGlobalUnits->m_bAllowBackGroundSound )
	{
		m_bBackGroundSound = true;
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
	}

	return true;
}

//���ÿ��
bool CGameClientEngine::OnResetGameEngine()
{
	//������Ϣ
	m_lPlayBetScore = 0L;			
	m_lPlayFreeSocre = 0L;		

	//��ע��
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));

	ZeroMemory(m_lUserJettonDetail,sizeof(m_lUserJettonDetail));
	//ׯ����Ϣ
	m_wBankerUser = INVALID_CHAIR;
	m_lBankerScore = 0l;
	m_lBankerWinScore = 0l;
	m_wBankerTime = 0;

	//ϵͳ��ׯ
	m_bEnableSysBanker = false;

	//״̬����
	m_bMeApplyBanker = false;

	//������Ϣ
	m_lAreaLimitScore = 0L;	
	m_lApplyBankerCondition = 0l;

	return true;
}
//ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//ʱ����Ϣ
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	if ((wClockID == IDI_PLACE_JETTON)&&(nElapse == 0))
	{
		//���ù��
		m_GameClientView.SetCurrentBet(0L);

		//��ֹ��ť
		/*m_GameClientView.m_btBet500000.EnableWindow(FALSE);		
		m_GameClientView.m_btBet1000.EnableWindow(FALSE);		
		m_GameClientView.m_btBet10000.EnableWindow(FALSE);	
		m_GameClientView.m_btBet100000.EnableWindow(FALSE);
		m_GameClientView.m_btBet1000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet5000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet10000000.EnableWindow(FALSE);*/
		m_GameClientView.m_btPngBet1000.EnableButton(false);
			m_GameClientView.m_btPngBet10000.EnableButton(false);
			m_GameClientView.m_btPngBet50000.EnableButton(false);
			m_GameClientView.m_btPngBet100000.EnableButton(false);
			m_GameClientView.m_btPngBet500000.EnableButton(false);
			m_GameClientView.m_btPngBet1000000.EnableButton(false);
			m_GameClientView.m_btPngBet5000000.EnableButton(false);
			m_GameClientView.m_btPngBet10000000.EnableButton(false);
		//ׯ�Ұ�ť
		/*m_GameClientView.m_btApplyBanker.EnableWindow( FALSE );
		m_GameClientView.m_btCancelBanker.EnableWindow( FALSE );*/
		m_GameClientView.m_btPngApplyBanker.EnableButton( false );
		m_GameClientView.m_btPngCancelBanker.EnableButton( false );

		return false;
	}

	if ( CGlobalUnits::GetInstance()->m_bAllowSound && wClockID == IDI_PLACE_JETTON && nElapse<=5 ) 
	{
		
		//PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_WARIMG"));
	}

	if((wClockID == IDI_DISPATCH_CARD) && (nElapse==6) ) 
	{
		m_GameClientView.FlexAnimation(enFlexDealCrad, false);
		return true;
	}
	if((wClockID == IDI_DISPATCH_CARD) && (nElapse==0) ) 
	{
		return false;
	}

	return true;
}

//�Թ�״̬
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:		//��Ϸ����
		{
			return OnSubGameFree(pData,wDataSize);
		}
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_PLACE_JETTON:	//�û���ע
		{
			return OnSubPlaceBet(pData,wDataSize);
		}
	case SUB_S_APPLY_BANKER:	//������ׯ
		{
			return OnSubUserApplyBanker(pData, wDataSize);
		}
	case SUB_S_CANCEL_BANKER:	//ȡ����ׯ
		{
			return OnSubUserCancelBanker(pData, wDataSize);
		}
	case SUB_S_CHANGE_BANKER:	//�л�ׯ��
		{
			return OnSubChangeBanker(pData, wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			while( m_PlaceBetArray.GetCount() > 0 )
			{
				CMD_S_PlaceBet &PlaceBet = m_PlaceBetArray[0];
				PlaceBet.cbAndroidUser = 2;
				OnSubPlaceBet(&PlaceBet,sizeof(PlaceBet));
				m_PlaceBetArray.RemoveAt(0);
			}
			KillTimer(IDI_PLACE_JETTON_BUFFER);
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_SEND_RECORD:		//��Ϸ��¼
		{
			return OnSubGameRecord(pData,wDataSize);
		}
	case SUB_S_PLACE_JETTON_FAIL:	//��עʧ��
		{
			return OnSubPlaceBetFail(pData,wDataSize);
		}
	case SUB_S_AMDIN_COMMAND:
		{
			return OnSubReqResult(pData,wDataSize);
		}
	case SUB_S_JETTON_DETAIL:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(tagJettonUser));
			if (wDataSize!=sizeof(tagJettonUser)) return false;
			//��Ϣ����
			tagJettonUser JettonUser=*((tagJettonUser *)pData);
			for(int i=1;i<AREA_COUNT;i++)
			{
				m_lUserJettonDetail[i][JettonUser.wChairID]=JettonUser.lUserJettonScore[i];
			}
			m_GameClientView.m_pDlgJettonInfo.InserUser(JettonUser);    

			return true;
		}
	case SUB_S_ADMIN_STORAGE:
		{
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight))
			{

				ASSERT(wDataSize==sizeof(CMD_S_SystemStorage));
				if(wDataSize!=sizeof(CMD_S_SystemStorage)) return false;

				if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd() )
				{
					CMD_S_SystemStorage* pResult = (CMD_S_SystemStorage*)pData;
					m_GameClientView.m_pClientControlDlg->UpDateSystemStorage(pResult);
				}
				return true;
			}
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//����״̬
			SetGameStatus(GAME_SCENE_FREE);

			//����ʱ��
			SetGameClock(GetMeChairID(), IDI_FREE, pStatusFree->cbTimeLeave);

            //������Ϣ
			m_lPlayFreeSocre = pStatusFree->lPlayFreeSocre;
		
			//ׯ����Ϣ
			m_wBankerUser = pStatusFree->wBankerUser;
			m_lBankerScore = pStatusFree->lBankerScore;
			m_lBankerWinScore = pStatusFree->lBankerWinScore;
			m_wBankerTime = pStatusFree->wBankerTime;

			// ϵͳ��ׯ
			m_bEnableSysBanker = pStatusFree->bEnableSysBanker;

			// ���Ʊ���
			m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
			m_lApplyBankerCondition = pStatusFree->lApplyBankerCondition;

			// ���ý���
			m_GameClientView.SetGameStatus(GAME_SCENE_FREE);
			m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));
			m_GameClientView.SetPlayBetScore(0);
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);
			m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore, m_lBankerWinScore, m_wBankerTime);
			m_GameClientView.SetEnableSysBanker(m_bEnableSysBanker);

			// ���ư�ť
			if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hControlInst)
			{
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
				m_GameClientView.m_btRealJettonInfo.ShowWindow(SW_SHOW);
				for ( int i = 0; i < CountArray(m_GameClientView.m_SkinListCtrl); ++i )
				{
					m_GameClientView.m_SkinListCtrl[i].ShowWindow(SW_SHOW);
				}
			}
			// ���ý���
			m_GameClientView.FlexAnimation(enFlexNULL,false);

			//���¿���
			UpdateButtonContron();

			return true;
		}
	case GAME_SCENE_PLAY:		//��Ϸ״̬
	case GAME_SCENE_END:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//��Ϣ����
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//����״̬
			SetGameStatus(pStatusPlay->cbGameStatus);

			//����ʱ��
			if( pStatusPlay->cbGameStatus == GAME_SCENE_END )
				SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD,pStatusPlay->cbTimeLeave);
			else
				SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

			//��ע��Ϣ
			memcpy(m_lAllBet, pStatusPlay->lAllBet, sizeof(m_lAllBet));
			memcpy(m_lPlayBet, pStatusPlay->lPlayBet, sizeof(m_lPlayBet));
			memcpy(m_lPlayScore, pStatusPlay->lPlayScore, sizeof(m_lPlayScore));
			
			//������Ϣ
			m_lPlayBetScore = pStatusPlay->lPlayBetScore;
			m_lPlayFreeSocre = pStatusPlay->lPlayFreeSocre;

			//ׯ����Ϣ
			m_wBankerUser = pStatusPlay->wBankerUser;
			m_lBankerScore = pStatusPlay->lBankerScore;
			m_lBankerWinScore = pStatusPlay->lBankerWinScore;
			m_wBankerTime = pStatusPlay->wBankerTime;

			// ϵͳ��ׯ
			m_bEnableSysBanker = pStatusPlay->bEnableSysBanker;

			// ���Ʊ���
			m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
			m_lApplyBankerCondition = pStatusPlay->lApplyBankerCondition;

			// ���ý���
			m_GameClientView.SetGameStatus(pStatusPlay->cbGameStatus);
			m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));
			m_GameClientView.SetPlayBetScore(m_lPlayBetScore);
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);
			m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore, m_lBankerWinScore, m_wBankerTime);
			m_GameClientView.SetEnableSysBanker(m_bEnableSysBanker);
			m_GameClientView.SetCardInfo(pStatusPlay->cbCardCount, pStatusPlay->cbTableCardArray);

			// ������ע����
			for( int i = 0 ; i < AREA_MAX; ++i )
			{
				m_GameClientView.SetPlayBet(i, m_lPlayBet[i]);
				m_GameClientView.SetAllBet(i, m_lAllBet[i]);
				m_GameClientView.AddChip(i, m_lAllBet[i]);
			}

			// ���ý�����Ϣ
			if ( pStatusPlay->cbGameStatus == GAME_SCENE_END && !IsLookonMode() )
			{
				//�ɼ���Ϣ
				m_GameClientView.SetCurGameScore(pStatusPlay->lPlayScore,pStatusPlay->lPlayAllScore);
			}

			// ���ý���
			if ( pStatusPlay->cbGameStatus == GAME_SCENE_END )
			{
				//�����˿�
				m_GameClientView.m_CardControl[INDEX_PLAYER].SetCardData(pStatusPlay->cbTableCardArray[INDEX_PLAYER], pStatusPlay->cbCardCount[INDEX_PLAYER]);
				m_GameClientView.m_CardControl[INDEX_BANKER].SetCardData(pStatusPlay->cbTableCardArray[INDEX_BANKER], pStatusPlay->cbCardCount[INDEX_BANKER]);

				// ���ý���
				m_GameClientView.FlexAnimation(enFlexDealCrad, true, false);

				//����
				m_GameClientView.FlashAnimation(true);
			}
			//else
			//{
			//	// ���ý���
			//	m_GameClientView.FlexAnimation(enFlexBetTip, true, false);
			//}
	
			//���ư�ť
			if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hControlInst)
			{
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
				m_GameClientView.m_btRealJettonInfo.ShowWindow(SW_SHOW);
				for ( int i = 0; i < CountArray(m_GameClientView.m_SkinListCtrl); ++i )
				{
					m_GameClientView.m_SkinListCtrl[i].ShowWindow(SW_SHOW);
				}
			}

			//���°�ť
			UpdateButtonContron();

			//���ð�ť
			m_GameClientView.SetCurrentBet(0L);

			//��ֹ��ť
			/*m_GameClientView.m_btBet500000.EnableWindow(FALSE);		
			m_GameClientView.m_btBet1000.EnableWindow(FALSE);		
			m_GameClientView.m_btBet10000.EnableWindow(FALSE);	
			m_GameClientView.m_btBet100000.EnableWindow(FALSE);
			m_GameClientView.m_btBet1000000.EnableWindow(FALSE);
			m_GameClientView.m_btBet5000000.EnableWindow(FALSE);
			m_GameClientView.m_btBet10000000.EnableWindow(FALSE);*/
			m_GameClientView.m_btPngBet1000.EnableButton(false);
			m_GameClientView.m_btPngBet10000.EnableButton(false);
			m_GameClientView.m_btPngBet50000.EnableButton(false);
			m_GameClientView.m_btPngBet100000.EnableButton(false);
			m_GameClientView.m_btPngBet500000.EnableButton(false);
			m_GameClientView.m_btPngBet1000000.EnableButton(false);
			m_GameClientView.m_btPngBet5000000.EnableButton(false);
			m_GameClientView.m_btPngBet10000000.EnableButton(false);

			return true;
		}
	}

	return false;
}

//��������
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
	if(bAllowSound && !m_bBackGroundSound )
	{
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
	}
	else if ( !bAllowSound )
	{
		StopSound();
		m_bBackGroundSound = false;
	}

	return true;
}

//��Ϸ��ʼ
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//�����Ϣ
	m_lPlayBetScore = pGameStart->lPlayBetScore;
	m_lPlayFreeSocre = pGameStart->lPlayFreeSocre;

	//ׯ����Ϣ
	m_wBankerUser = pGameStart->wBankerUser;
	m_lBankerScore = pGameStart->lBankerScore;

	//���ý���
	m_GameClientView.SetPlayBetScore(m_lPlayBetScore);
	m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore );

	//����ʱ��
	WORD wMeChair = GetMeChairID();
	SetGameClock(wMeChair,IDI_PLACE_JETTON,pGameStart->cbTimeLeave);

	//����״̬
	SetGameStatus(GAME_SCENE_BET);
	m_GameClientView.SetGameStatus(GAME_SCENE_BET);

	m_GameClientView.m_pDlgJettonInfo.ClearAll();

	for(int wChair =0;wChair<GAME_PLAYER;wChair++)
	{
		IClientUserItem *pUserData=GetTableUserItem(wChair);
		if(pUserData == NULL) continue;
		if(pGameStart->bRealUser[wChair]==FALSE) continue;
		tagJettonUser JettonUser;
		JettonUser.wChairID=wChair;
		ZeroMemory(&JettonUser,sizeof(JettonUser));
		_tcscpy(JettonUser.strUserName,pUserData->GetUserInfo()->szNickName);
		for(int i=0;i<8;i++)
		{
			JettonUser.lUserJettonScore[i] = 0;
		}
		m_GameClientView.m_pDlgJettonInfo.InserUser(JettonUser);     
	}


	//���¿���
	UpdateButtonContron();

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);

	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	m_PlaceBetArray.RemoveAll();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return false;

	//��Ϣ����
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

	//
	ZeroMemory(m_lUserJettonDetail,sizeof(m_lUserJettonDetail));

	//����ʱ��
	SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

	//����״̬
	SetGameStatus(GAME_SCENE_FREE);
	m_GameClientView.SetGameStatus(GAME_SCENE_FREE);

	//�����ע
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));

	//��ɷ���
	m_GameClientView.FinishDispatchCard();

	//��������
	m_GameClientView.CleanUserBet();
	m_GameClientView.m_pDlgJettonInfo.ClearAll();

	//����
	m_GameClientView.FlashAnimation(false);

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}

//�û���ע
bool CGameClientEngine::OnSubPlaceBet(const void * pBuffer, WORD wDataSize,bool bAndroid)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceBet));
	if (wDataSize!=sizeof(CMD_S_PlaceBet)) return false;

	//��Ϣ����
	CMD_S_PlaceBet * pPlaceBet=(CMD_S_PlaceBet *)pBuffer;

	//�����ж�
	if (pPlaceBet->cbAndroidUser==TRUE)
	{
		m_PlaceBetArray.Add(*pPlaceBet);
		if (m_PlaceBetArray.GetCount()==1) SetTimer(IDI_PLACE_JETTON_BUFFER,70,NULL);

		return true;

	}
	else if(pPlaceBet->cbAndroidUser==0)
	{
		m_lUserJettonDetail[pPlaceBet->cbBetArea][pPlaceBet->wChairID]+=pPlaceBet->lBetScore;
		//������Ϣ
		tagJettonUser JettonUser ;
		JettonUser.wChairID = pPlaceBet->wChairID;
		for(int i=0;i<AREA_COUNT;i++)
		{
			JettonUser.lUserJettonScore[i] = m_lUserJettonDetail[i][pPlaceBet->wChairID];
		}
		IClientUserItem* pUserData=GetTableUserItem(pPlaceBet->wChairID);
		_tcscpy(JettonUser.strUserName,pUserData->GetUserInfo()->szNickName);
		m_GameClientView.m_pDlgJettonInfo.UpdateUser(JettonUser);

		if(!bAndroid)
		{
			m_GameClientView.ControlBetDetection(pPlaceBet->cbBetArea,pPlaceBet->lBetScore,SwitchViewChairID(pPlaceBet->wChairID));
		}
	}

	if (GetMeChairID() != pPlaceBet->wChairID || IsLookonMode())
	{
		//��ע����
		m_lAllBet[pPlaceBet->cbBetArea] += pPlaceBet->lBetScore;

		m_GameClientView.SetAllBet( pPlaceBet->cbBetArea, m_lAllBet[pPlaceBet->cbBetArea]);
		m_GameClientView.AddChip( pPlaceBet->cbBetArea,pPlaceBet->lBetScore );

		//��������
		if (CGlobalUnits::GetInstance()->m_bAllowSound) 
		{
			if (pPlaceBet->wChairID!=GetMeChairID() || IsLookonMode())
			{
				if (pPlaceBet->lBetScore >= 5000000) 
					PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
				else 
					PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
			}
		}
	}
    
	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//����ʱ��
	SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

	//�˿���Ϣ
	m_GameClientView.SetCardInfo(pGameEnd->cbCardCount,pGameEnd->cbTableCardArray);

	//ׯ����Ϣ
	m_GameClientView.SetBankerOverInfo( pGameEnd->lBankerTotallScore, pGameEnd->nBankerTime,pGameEnd->lBankerScore);

	//�ɼ���Ϣ
	m_GameClientView.SetCurGameScore(pGameEnd->lPlayScore,pGameEnd->lPlayAllScore);

	//����״̬
	SetGameStatus(GAME_SCENE_END);
	m_GameClientView.SetGameStatus(GAME_SCENE_END);

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}

//���¿���
void CGameClientEngine::UpdateButtonContron()
{
	//�����ж�
	bool bEnablePlaceBet = true;
	if ( (m_bEnableSysBanker == false && m_wBankerUser == INVALID_CHAIR )
		|| (GetGameStatus() != GAME_SCENE_BET)
		|| (m_wBankerUser == GetMeChairID())
		|| (IsLookonMode()) )
	{
		bEnablePlaceBet = false;
	}

	//��ע��ť
	if ( bEnablePlaceBet )
	{
		//�������
		LONGLONG lCurrentBet = m_GameClientView.GetCurrentBet();

		LONGLONG lLeaveScore = 0;
		for ( int i = 0; i < AREA_MAX; ++i )
		{
			lLeaveScore = max( lLeaveScore, m_GameClientView.GetMaxPlayerScore(i) );
		}

		//���ù��
		if (lCurrentBet>lLeaveScore)
		{
			if (lLeaveScore>=10000000L) m_GameClientView.SetCurrentBet(10000000L);
			else if (lLeaveScore>=5000000L) m_GameClientView.SetCurrentBet(5000000L);
			else if (lLeaveScore>=1000000L) m_GameClientView.SetCurrentBet(1000000L);
			else if (lLeaveScore>=500000L) m_GameClientView.SetCurrentBet(500000L);
			else if (lLeaveScore>=100000L) m_GameClientView.SetCurrentBet(100000L);
            else if (lLeaveScore>=50000L) m_GameClientView.SetCurrentBet(50000L);
			else if (lLeaveScore>=10000L) m_GameClientView.SetCurrentBet(10000L);
			else if (lLeaveScore>=1000L) m_GameClientView.SetCurrentBet(1000L);
			else m_GameClientView.SetCurrentBet(0L);
		}

		//���ư�ť
		/*m_GameClientView.m_btBet500000.EnableWindow((lLeaveScore>=500000)?TRUE:FALSE);
		m_GameClientView.m_btBet1000.EnableWindow((lLeaveScore>=1000)?TRUE:FALSE);
		m_GameClientView.m_btBet10000.EnableWindow((lLeaveScore>=10000)?TRUE:FALSE);
		m_GameClientView.m_btBet100000.EnableWindow((lLeaveScore>=100000)?TRUE:FALSE);
		m_GameClientView.m_btBet1000000.EnableWindow((lLeaveScore>=1000000)?TRUE:FALSE);		
		m_GameClientView.m_btBet5000000.EnableWindow((lLeaveScore>=5000000)?TRUE:FALSE);
		m_GameClientView.m_btBet10000000.EnableWindow((lLeaveScore>=10000000)?TRUE:FALSE);*/
		m_GameClientView.m_btPngBet1000.EnableButton(lLeaveScore>=1000);
		m_GameClientView.m_btPngBet10000.EnableButton(lLeaveScore>=100000);
		m_GameClientView.m_btPngBet50000.EnableButton(lLeaveScore>=500000);
		m_GameClientView.m_btPngBet100000.EnableButton(lLeaveScore>=1000000);
		m_GameClientView.m_btPngBet500000.EnableButton(lLeaveScore>=5000000);
		m_GameClientView.m_btPngBet1000000.EnableButton(lLeaveScore>=1000000);
		m_GameClientView.m_btPngBet5000000.EnableButton(lLeaveScore>=5000000);
		m_GameClientView.m_btPngBet10000000.EnableButton(lLeaveScore>=10000000);
	}
	else
	{
		//���ù��
		m_GameClientView.SetCurrentBet(0L);

		//��ֹ��ť
		/*m_GameClientView.m_btBet500000.EnableWindow(FALSE);		
		m_GameClientView.m_btBet1000.EnableWindow(FALSE);		
		m_GameClientView.m_btBet10000.EnableWindow(FALSE);	
		m_GameClientView.m_btBet100000.EnableWindow(FALSE);
		m_GameClientView.m_btBet1000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet5000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet10000000.EnableWindow(FALSE);*/
		m_GameClientView.m_btPngBet1000.EnableButton(false);
			m_GameClientView.m_btPngBet10000.EnableButton(false);
			m_GameClientView.m_btPngBet50000.EnableButton(false);
			m_GameClientView.m_btPngBet100000.EnableButton(false);
			m_GameClientView.m_btPngBet500000.EnableButton(false);
			m_GameClientView.m_btPngBet1000000.EnableButton(false);
			m_GameClientView.m_btPngBet5000000.EnableButton(false);
			m_GameClientView.m_btPngBet10000000.EnableButton(false);
	}

	//ׯ�Ұ�ť
	if ( !IsLookonMode() )
	{
		//��ȡ��Ϣ
		IClientUserItem* pMeUserItem = GetMeUserItem();

		//���밴ť
		bool bEnableApply = true;
		if (m_wBankerUser == GetMeChairID() || m_bMeApplyBanker || m_lPlayFreeSocre < m_lApplyBankerCondition || pMeUserItem->GetUserScore() < m_lApplyBankerCondition) 
			bEnableApply = false;

		//m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);
		m_GameClientView.m_btPngApplyBanker.EnableButton(bEnableApply?true:false);

		//ȡ����ť
		bool bEnableCancel = true;
		if ( m_wBankerUser == GetMeChairID() && GetGameStatus() != GAME_SCENE_FREE ) bEnableCancel=false;
		if ( m_bMeApplyBanker == false ) bEnableCancel=false;
		/*m_GameClientView.m_btCancelBanker.EnableWindow(bEnableCancel?TRUE:FALSE);
		m_GameClientView.m_btCancelBanker.SetButtonImage(m_wBankerUser==GetMeChairID()?IDB_BT_CANCEL_BANKER:IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);*/

		m_GameClientView.m_btPngCancelBanker.EnableButton(bEnableCancel?true:false);
		m_GameClientView.m_btPngCancelBanker.SetButtonImage(AfxGetInstanceHandle(),m_wBankerUser==GetMeChairID()?TEXT("BT_CANCEL_BANKER"):TEXT("BT_CANCEL_APPLY"));

		//��ʾ�ж�
		if (m_bMeApplyBanker)
		{
			/*m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);*/
			m_GameClientView.m_btPngCancelBanker.ShowButton(true);
			m_GameClientView.m_btPngApplyBanker.ShowButton(false);
		}
		else
		{
			m_GameClientView.m_btPngCancelBanker.ShowButton(false);
			m_GameClientView.m_btPngApplyBanker.ShowButton(true);
		}
	}
	else
	{
		m_GameClientView.m_btPngCancelBanker.EnableButton(false);
		m_GameClientView.m_btPngApplyBanker.EnableButton(false);
		m_GameClientView.m_btPngApplyBanker.ShowButton(true);
		m_GameClientView.m_btPngCancelBanker.ShowButton(false);
	}
	//���а�ť
	/*m_GameClientView.m_btBankerDraw.EnableWindow(TRUE);
	m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
	m_GameClientView.m_btBankerStorage.ShowWindow(SW_SHOW);
	m_GameClientView.m_btBankerStorage.EnableWindow((GetGameStatus()==GAME_STATUS_FREE || IsLookonMode())?TRUE:FALSE);
	if(m_wBankerUser==GetMeChairID()&&IsLookonMode()==false)
	{
		m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
	}*/
	m_GameClientView.m_btPngDraw.EnableButton(true);


	if(!IsLookonMode())
	{
		switch (GetGameStatus())
		{
		case GAME_SCENE_FREE:
			{
				m_GameClientView.SetInsureOption(false, true);
				break;
			}
		case GAME_SCENE_PLAY:
		case GAME_SCENE_END:
			{				
				m_GameClientView.SetInsureOption(false, false);
				break;
			}
		default:break;
		}
	}
	else
	{
		m_GameClientView.SetInsureOption(false, false);
	}

	m_GameClientView.InvalidGameView(0,0,0,0);

	return;
}

//��ע��Ϣ
LRESULT CGameClientEngine::OnPlayBet(WPARAM wParam, LPARAM lParam)
{
	//��������
	BYTE cbBetArea=(BYTE)wParam;
	LONGLONG lBetScore= m_GameClientView.GetCurrentBet();

	//ׯ���ж�
	if ( GetMeChairID() == m_wBankerUser ) return true;

	//״̬�ж�
	if (GetGameStatus() != GAME_SCENE_BET)
	{
		UpdateButtonContron();
		return true;
	}

	//���ñ���
	m_lAllBet[cbBetArea] += lBetScore;
	m_lPlayBet[cbBetArea] += lBetScore;

	//���ý���
	m_GameClientView.SetPlayBet(cbBetArea, m_lPlayBet[cbBetArea]);
	m_GameClientView.SetAllBet(cbBetArea, m_lAllBet[cbBetArea]);
	m_GameClientView.AddChip(cbBetArea, lBetScore);

	//��������
	CMD_C_PlaceBet PlaceBet;
	ZeroMemory(&PlaceBet,sizeof(PlaceBet));

	//�������
	PlaceBet.cbBetArea = cbBetArea;
	PlaceBet.lBetScore = lBetScore;

	//������Ϣ
	SendSocketData(SUB_C_PLACE_JETTON,&PlaceBet,sizeof(PlaceBet));

	//���°�ť
	UpdateButtonContron();

	//��������
	if (CGlobalUnits::GetInstance()->m_bAllowSound) 
	{
		if (lBetScore >= 5000000) 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
		else 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
	}

	return 0;
}

//������Ϣ
LRESULT CGameClientEngine::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
	//�Ϸ��ж�
	IClientUserItem* pMeUserItem = GetMeUserItem();
	if (pMeUserItem->GetUserScore() < m_lApplyBankerCondition) return true;
	if (m_lPlayFreeSocre < m_lApplyBankerCondition) return true;

	//�Թ��ж�
	if (IsLookonMode()) return true;

	//ת������
	bool bApplyBanker = (wParam!=0) ? true:false;

	//��ǰ�ж�
	if (m_wBankerUser == GetMeChairID() && bApplyBanker) return true;

	if (bApplyBanker)
	{
		//������Ϣ
		SendSocketData(SUB_C_APPLY_BANKER, NULL, 0);

		m_bMeApplyBanker=true;
	}
	else
	{
		//������Ϣ
		SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

		m_bMeApplyBanker=false;
	}

	//���ð�ť
	UpdateButtonContron();

	return true;
}

//������Ϣ
LRESULT CGameClientEngine::OnPlaySound(WPARAM wParam, LPARAM lParam)
{
	if( !CGlobalUnits::GetInstance()->m_bAllowSound )
		return 0;

	PlayGameSound(AfxGetInstanceHandle(), (TCHAR*)wParam);
	return 0;
}

//������ׯ
bool CGameClientEngine::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker = (CMD_S_ApplyBanker *)pBuffer;

	//��ȡ���
	IClientUserItem* pClientUserItem = GetTableUserItem(pApplyBanker->wApplyUser);

	if ( pClientUserItem == NULL )
		return true;

	//�������
	if (m_wBankerUser != pApplyBanker->wApplyUser)
	{
		m_GameClientView.m_ValleysList.Add( SwitchViewChairID(pApplyBanker->wApplyUser) );
	}

	//�Լ��ж�
	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) m_bMeApplyBanker=true;

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}

//ȡ����ׯ
bool CGameClientEngine::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return false;

	//��Ϣ����
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

	//ɾ�����
	for( int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i )
	{
		if ( SwitchViewChairID(pCancelBanker->wCancelUser) == m_GameClientView.m_ValleysList[i] )
		{
			m_GameClientView.m_ValleysList.RemoveAt(i);
			break;
		}
	}

	//�Լ��ж�
	if ( IsLookonMode() == false && pCancelBanker->wCancelUser == GetMeChairID() ) 
		m_bMeApplyBanker = false;

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}

//�л�ׯ��
bool CGameClientEngine::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//��Ϣ����
	CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

	//��ʾͼƬ
	m_GameClientView.ShowChangeBanker(m_wBankerUser!=pChangeBanker->wBankerUser);

	//�Լ��ж�
	if (m_wBankerUser==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID()) 
	{
		m_bMeApplyBanker=false;
	}
	else if (IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
	{
		m_bMeApplyBanker=true;
	}

	//ׯ����
	m_wBankerUser = pChangeBanker->wBankerUser;
	m_lBankerScore = pChangeBanker->lBankerScore;
	m_lBankerWinScore = 0;
	m_wBankerTime = 0;
	m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore, m_lBankerWinScore, m_wBankerTime);

	//ɾ�����
	if (m_wBankerUser != INVALID_CHAIR)
	{
		for(int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i)
		{
			if( SwitchViewChairID(m_wBankerUser) == m_GameClientView.m_ValleysList[i] )
			{
				m_GameClientView.m_ValleysList.RemoveAt(i);
				break;
			}
		}
	}

	//���½���
	UpdateButtonContron();

	return true;
}

//��Ϸ��¼
bool CGameClientEngine::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
	if (wDataSize%sizeof(tagServerGameRecord)!=0) return false;

	//�������
	tagGameRecord GameRecord;
	ZeroMemory(&GameRecord,sizeof(GameRecord));

	//���ü�¼
	WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
	for (WORD wIndex=0;wIndex<wRecordCount;wIndex++) 
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

		m_GameClientView.SetGameHistory(enOperateResult_NULL, pServerGameRecord->cbPlayerCount, pServerGameRecord->cbBankerCount,
			pServerGameRecord->cbKingWinner,pServerGameRecord->bPlayerTwoPair,pServerGameRecord->bBankerTwoPair);
	}

	return true;
}

//��עʧ��
bool CGameClientEngine::OnSubPlaceBetFail(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceBetFail));
	if (wDataSize!=sizeof(CMD_S_PlaceBetFail)) return false;

	//��Ϣ����
	CMD_S_PlaceBetFail * pPlaceBetFail=(CMD_S_PlaceBetFail *)pBuffer;

	//Ч�����
	BYTE cbViewIndex = pPlaceBetFail->lBetArea;
	ASSERT(cbViewIndex < AREA_MAX);
	if (cbViewIndex >= AREA_MAX) return false;

	//�Լ��ж�
	if ( GetMeChairID() == pPlaceBetFail->wPlaceUser && !IsLookonMode() )
	{
		//���ñ���
		m_lPlayBet[cbViewIndex] -= pPlaceBetFail->lPlaceScore;
		m_lAllBet[cbViewIndex] -= pPlaceBetFail->lPlaceScore;
		
		//���ý���
		m_GameClientView.SetPlayBet(cbViewIndex, m_lPlayBet[cbViewIndex]);
		m_GameClientView.SetAllBet(cbViewIndex, m_lAllBet[cbViewIndex]);
		m_GameClientView.AddChip(pPlaceBetFail->lBetArea, -pPlaceBetFail->lPlaceScore);
	}

	m_GameClientView.InvalidGameView(0,0,0,0);

	return true;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CGameClientEngine::OnAdminCommand(WPARAM wParam,LPARAM lParam)
{
	SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
	return true;
}

bool CGameClientEngine::OnSubReqResult(const void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize == sizeof(CMD_S_CommandResult));
	if(wDataSize!=sizeof(CMD_S_CommandResult)) return false;

	m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
	return true;
}

void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	if (IDI_PLACE_JETTON_BUFFER == nIDEvent)
	{
		if (m_PlaceBetArray.GetCount()>0)
		{
			CMD_S_PlaceBet &PlaceBet=m_PlaceBetArray[0];
			PlaceBet.cbAndroidUser=2;
			OnSubPlaceBet(&PlaceBet,sizeof(PlaceBet),true);
			m_PlaceBetArray.RemoveAt(0);

			if(m_PlaceBetArray.GetCount()>30)
			{
				CMD_S_PlaceBet &PlaceBet=m_PlaceBetArray[0];
				PlaceBet.cbAndroidUser=2;
				OnSubPlaceBet(&PlaceBet,sizeof(PlaceBet),true);
				m_PlaceBetArray.RemoveAt(0);
			}
		}

		if (m_PlaceBetArray.GetCount() == 0) KillTimer(IDI_PLACE_JETTON_BUFFER);
		return;
	}

	CGameFrameEngine::OnTimer(nIDEvent);
}

