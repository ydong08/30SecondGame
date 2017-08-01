#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define SEND_COUNT					300									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//��עʱ��
#define IDI_FREE					1									//����ʱ��
#define TIME_FREE					10									//����ʱ��

//��עʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define TIME_PLACE_JETTON			20									//��עʱ��

//����ʱ��
#define IDI_GAME_END				3									//����ʱ��
#define TIME_GAME_END				12									//����ʱ��

#define IDI_RESET_STORAGE           4                                   //��տ��


//��Ϣ��ѯ
#define KEY_STOCK					0
#define KEY_IMMORTAL_COUNT			1
#define KEY_ROBOT_COUNT				2
#define KEY_IMMORTAL_BET			3	
#define KEY_ROBOT_BET				4
#define KEY_MAX						5

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//���캯��
CTableFrameSink::CTableFrameSink()
{
	

	//����
	m_lBankerScore = 0l;
	ZeroMemory(m_lPlayScore,sizeof(m_lPlayScore));
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�˿���Ϣ
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//״̬����
	m_dwBetTime=0L;

	//ׯ����Ϣ
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wBankerTime=0;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;

	//��¼����
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;

	//ׯ������
	m_lBankerMAX = 0l;
	m_lBankerAdd = 0l;							
	m_lBankerScoreMAX = 0l;
	m_lBankerScoreAdd = 0l;
	m_lPlayerBankerMAX = 0l;
	m_bExchangeBanker = true;

	m_lExitScore=0;

	//ʱ�����
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;

	//�����˿���
	m_nChipRobotCount = 0;

	//�������
	m_hControlInst = NULL;
	m_pServerContro = NULL;
	m_hControlInst = LoadLibrary(TEXT("BaccaratNewServerControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hControlInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	ZeroMemory(m_nAreaRate,sizeof(m_nAreaRate));
	m_nAreaRate[AREA_XIAN]=100;
	m_nAreaRate[AREA_PING]=40;
	m_nAreaRate[AREA_ZHUANG]=100;
	m_nAreaRate[AREA_XIAN_TIAN]=40;
	m_nAreaRate[AREA_ZHUANG_TIAN]=40;
	m_nAreaRate[AREA_TONG_DUI]=100;
	m_nAreaRate[AREA_XIAN_DUI]=100;
	m_nAreaRate[AREA_ZHUANG_DUI]=100;

	m_nBetMinInterval=200;
	ZeroMemory(m_dwLastBetTime,sizeof(m_dwLastBetTime));
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//�ͷŶ���
VOID CTableFrameSink::Release()
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//�ӿڲ�ѯ
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame == NULL) return false;

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//��ȡ����
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

	//�����ļ���
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,CountArray(m_szConfigFileName),TEXT("%s\\BaccaratNewConfig.ini"),szPath);

	m_StorageControl.InitConfig(m_szConfigFileName,m_szGameRoomName,false);

	ReadConfigInformation(true);	

	ReadAreaRate();

	return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
	//��ע��
	m_UserJettonManager.Reset();

	//����
	m_lBankerScore = 0l;
	ZeroMemory(m_lPlayScore,sizeof(m_lPlayScore));
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�����˿���
	m_nChipRobotCount = 0;

	return;
}


//��Ϸ״̬
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}
//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinEnterScore-10L, 0L);
	}
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	m_UserJettonManager.Reset();

	m_StorageControl.ResetCondition();

	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//��ȡׯ��
	IServerUserItem* pIBankerServerUserItem = NULL;
	if ( m_wCurrentBanker == INVALID_CHAIR )
	{
		m_lBankerScore = 1000000000;
	}
	else
	{
		IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if ( pIBankerServerUserItem != NULL )
		{
			m_lBankerScore = pIBankerServerUserItem->GetUserScore();
		}
	}

	//���ñ���
	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBankerScore = m_lBankerScore;
	GameStart.nListUserCount = m_ApplyUserArray.GetCount();

	//��ע����������
	int nChipRobotCount = 0;
	BYTE bRealUser[GAME_PLAYER]={0,};
	for (int i = 0; i < GAME_PLAYER; i++) 
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser()==false)
		{
			bRealUser[i]=TRUE;
			//GameStart.bRealUser[i]=TRUE;
		}
	}

	int nGameStart_nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);
	//GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);
	
	nChipRobotCount = 0;
	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++) 
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}


	int nGameStart_nAndriodCount=0;
	if(nChipRobotCount > 0)
	{
		nGameStart_nAndriodCount=nChipRobotCount-1;
		//GameStart.nAndriodCount=nChipRobotCount-1;
	}

	//�����˿���
	m_nChipRobotCount = 0;

    //�Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

	//��Ϸ���
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//���û���
		TRACE(TEXT("UserFreeScore :: %I64d"), pIServerUserItem->GetUserScore());
		GameStart.lPlayBetScore=min(pIServerUserItem->GetUserScore() - m_nServiceCharge,m_lUserLimitScore);
		GameStart.lPlayFreeSocre = pIServerUserItem->GetUserScore();

		if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true)
		{
			CopyMemory(GameStart.bRealUser,bRealUser,sizeof(bRealUser));
		}
		else
		{
			ZeroMemory(GameStart.bRealUser,sizeof(GameStart.bRealUser));
		}
		if (pIServerUserItem->IsAndroidUser())
		{
			GameStart.nChipRobotCount = nGameStart_nChipRobotCount;
			GameStart.nAndriodCount = nGameStart_nAndriodCount;
		}
		else
		{
			GameStart.nChipRobotCount = 0;
			GameStart.nAndriodCount = 0;
		}
		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	CWHDataLocker lock(m_cs);
	switch (cbReason)
	{
	case GER_NORMAL:		//�������	
		{
			
			//�������
			LONGLONG lBankerWinScore = GameOver();

			//��������
			m_wBankerTime++;

			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ׯ����Ϣ
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;

			//�˿���Ϣ
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			CopyMemory(GameEnd.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));

			//���ͻ���
			GameEnd.cbTimeLeave=m_cbEndTime;	
			for ( WORD i = 0; i < GAME_PLAYER; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if ( pIServerUserItem == NULL ) continue;

				//���óɼ�
				GameEnd.lPlayAllScore = m_lUserWinScore[i];
				memcpy( GameEnd.lPlayScore, m_lPlayScore[i], sizeof(GameEnd.lPlayScore));
				
				//����˰��
				GameEnd.lRevenue = m_lUserRevenue[i];

				//������Ϣ					
				m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}
			
			return true;
		}
	case GER_NETWORK_ERROR:		//�����ж�
	case GER_USER_LEAVE:		//�û��뿪
		{
			
			//�м��ж�
			if (m_wCurrentBanker!=wChairID)
			{
				//��������
				LONGLONG lRevenue=0;
		

				//д�����
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
				{

					for ( BYTE wAreaIndex = AREA_XIAN; wAreaIndex <= AREA_ZHUANG_DUI; ++wAreaIndex )
					{
						if (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) != 0)
						{
							CMD_S_PlaceBetFail PlaceBetFail;
							ZeroMemory(&PlaceBetFail,sizeof(PlaceBetFail));
							PlaceBetFail.lBetArea = (BYTE)wAreaIndex;
							PlaceBetFail.lPlaceScore = m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
							PlaceBetFail.wPlaceUser = wChairID;

							//��Ϸ���
							for (WORD i=0; i<GAME_PLAYER; ++i)
							{
								IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
								if (pIServerUserItem == NULL) 
									continue;

								m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceBetFail,sizeof(PlaceBetFail));
							}

							//lExitScore+=m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
							//m_lExitScore+=m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
							//m_UserJettonManager.ModifyAllJettonScore(wAreaIndex,-m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex));
							//m_UserJettonManager.SetUserJettonScore(wChairID,wAreaIndex,0);
							
						}
					}

					SCORE lExitScore = m_UserJettonManager.GetUserJettonScore(wChairID);

					if (lExitScore!=0L) 
					{
						tagScoreInfo scoreInfo;
						ZeroMemory(&scoreInfo,sizeof(scoreInfo));
						scoreInfo.cbType=SCORE_TYPE_FLEE;
						scoreInfo.lScore=-lExitScore;
						scoreInfo.lRevenue=0;
						m_pITableFrame->WriteUserScore(wChairID,scoreInfo);

						m_UserJettonManager.ModifyUserFleeScore(-scoreInfo.lScore);
						m_lUserWinScore[wChairID] = 0;
						m_lUserRevenue[wChairID] = 0;
						m_UserJettonManager.ResetUserJettonScore(wChairID);

						TCHAR szMsg[128]={0,};
						_sntprintf(szMsg,CountArray(szMsg),TEXT("�м� %sǿ�ˣ�"),pIServerUserItem->GetNickName());
						m_pITableFrame->SendGameMessage(szMsg,SMT_CHAT);
					}


				}
				else
				{

					//�Ƿ�д��
					bool bWritePoints = false;
					for( BYTE wAreaIndex = AREA_XIAN; wAreaIndex <= AREA_ZHUANG_DUI; ++wAreaIndex )
					{
						if( m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex)!= 0 )
						{
							bWritePoints = true;
							break;
						}
					}

					//��д��
					if( !bWritePoints )
					{
						return true;
					}

					//д�����
					if((pIServerUserItem->GetUserScore() - m_nServiceCharge) + m_lUserWinScore[wChairID] < 0L)
						m_lUserWinScore[wChairID] = -(pIServerUserItem->GetUserScore() - m_nServiceCharge);

					tagScoreInfo ScoreInfo;
					ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
					ScoreInfo.lScore=m_lUserWinScore[wChairID];
					ScoreInfo.lRevenue = m_lUserRevenue[wChairID];

					if ( ScoreInfo.lScore > 0 )
						ScoreInfo.cbType=SCORE_TYPE_WIN;
					else if ( ScoreInfo.lScore < 0 )
						ScoreInfo.cbType=SCORE_TYPE_LOSE;
					else
						ScoreInfo.cbType=SCORE_TYPE_DRAW;

					m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);
					
					m_UserJettonManager.ModifyUserFleeScore(-ScoreInfo.lScore);
					m_lUserWinScore[wChairID] = 0;
					m_lUserRevenue[wChairID] = 0;
					m_UserJettonManager.ResetUserJettonScore(wChairID);

					
				}

				return true;
			}

			//״̬�ж�
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_END)
			{

				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"),pIServerUserItem->GetNickName());

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);

				//����ʱ��
				m_dwBetTime=(DWORD)time(NULL);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);

				//�������
				GameOver();

				//������Ϣ
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//ׯ����Ϣ
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				if (m_lBankerWinScore>0L) GameEnd.lBankerScore=0;

				//�˿���Ϣ
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				CopyMemory(GameEnd.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));

				//���ͻ���
				GameEnd.cbTimeLeave=m_cbEndTime;	
				for ( WORD i = 0; i < GAME_PLAYER; ++i )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if ( pIServerUserItem == NULL ) continue;

					//���óɼ�
					GameEnd.lPlayAllScore = m_lUserWinScore[i];
					memcpy( GameEnd.lPlayScore, m_lPlayScore[i], sizeof(GameEnd.lPlayScore));

					//����˰��
					GameEnd.lRevenue = m_lUserRevenue[i];

					//������Ϣ					
					m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//�۳�����
			if(pIServerUserItem->GetUserScore() - m_nServiceCharge + m_lUserWinScore[m_wCurrentBanker] < 0L)
				m_lUserWinScore[m_wCurrentBanker] = -(pIServerUserItem->GetUserScore() - m_nServiceCharge);

			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			ScoreInfo.lScore = m_lUserWinScore[m_wCurrentBanker];
			ScoreInfo.lRevenue = m_lUserRevenue[m_wCurrentBanker];

			if ( ScoreInfo.lScore > 0 )
				ScoreInfo.cbType = SCORE_TYPE_FLEE;
			else if ( ScoreInfo.lScore < 0 )
				ScoreInfo.cbType = SCORE_TYPE_FLEE;
			else
				ScoreInfo.cbType = SCORE_TYPE_FLEE;

			if(ScoreInfo.lScore > 0)
			{
				ScoreInfo.lScore*=-1;
			}

			//m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			m_pITableFrame->WriteUserScore( m_wCurrentBanker, ScoreInfo );

			m_lUserWinScore[m_wCurrentBanker] = 0L;

			m_UserJettonManager.ResetUserJettonScore(m_wCurrentBanker);

			//�л�ׯ��
			ChangeBanker(true);

			return true;
		}
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//ȫ����Ϣ
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			StatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));

			//�����Ϣ
			StatusFree.lPlayFreeSocre = pIServerUserItem->GetUserScore();

			//ׯ����Ϣ
			StatusFree.wBankerUser = m_wCurrentBanker;	
			StatusFree.wBankerTime = m_wBankerTime;
			StatusFree.lBankerWinScore = m_lBankerWinScore;
			StatusFree.lBankerScore = m_lBankerScore;

			//�Ƿ�ϵͳ��ׯ
			StatusFree.bEnableSysBanker=m_bEnableSysBanker;

			//������Ϣ
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			//��������
			CopyMemory(StatusFree.szGameRoomName, m_szGameRoomName, sizeof(StatusFree.szGameRoomName));

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//������ʾ
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n��������ׯ����Ϊ��%s\n��������Ϊ��%s\n�������Ϊ��%s"), AddComma(m_lApplyBankerCondition), AddComma(m_lAreaLimitScore), AddComma(m_lUserLimitScore));
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
						
			//����������
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GAME_SCENE_BET:		//��Ϸ״̬
	case GAME_SCENE_END:		//����״̬
		{
			SendJettonDetail(pIServerUserItem);
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);		

			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL) - m_dwBetTime;
			int	nTotalTime = ( (cbGameStatus == GAME_SCENE_BET) ? m_cbBetTime : m_cbEndTime);
			StatusPlay.cbTimeLeave = (BYTE)(nTotalTime - __min(dwPassTime, (DWORD)nTotalTime));
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();		

			//ȫ����ע
			m_UserJettonManager.CopyAllJettonScore(StatusPlay.lAllBet,sizeof(StatusPlay.lAllBet));
			StatusPlay.lPlayFreeSocre = pIServerUserItem->GetUserScore();

			//�����ע
			if (pIServerUserItem->GetUserStatus() != US_LOOKON && bSendSecret)
			{
				m_UserJettonManager.CopyUserJettonScore(wChiarID,StatusPlay.lPlayBet,sizeof(StatusPlay.lPlayBet));

				memcpy(StatusPlay.lPlayScore, m_lPlayScore[wChiarID], sizeof(StatusPlay.lPlayScore));

				//�����ע
				StatusPlay.lPlayBetScore = min(pIServerUserItem->GetUserScore() - m_nServiceCharge, m_lUserLimitScore);
			}

			//ׯ����Ϣ
			StatusPlay.wBankerUser = m_wCurrentBanker;			
			StatusPlay.wBankerTime = m_wBankerTime;
			StatusPlay.lBankerWinScore = m_lBankerWinScore;	
			StatusPlay.lBankerScore = m_lBankerScore;

			//�Ƿ�ϵͳ��ׯ
			StatusPlay.bEnableSysBanker = m_bEnableSysBanker;

			//������Ϣ
			StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;		
			StatusPlay.lAreaLimitScore=m_lAreaLimitScore;		

			//�����ж�
			if (cbGameStatus == GAME_SCENE_END)
			{
				//�˿���Ϣ
				CopyMemory(StatusPlay.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

				//��������
				StatusPlay.lPlayAllScore = m_lUserWinScore[wChiarID];
			}

			//��������
			CopyMemory(StatusPlay.szGameRoomName, m_szGameRoomName, sizeof(StatusPlay.szGameRoomName));

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//������ʾ
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n��������ׯ����Ϊ��%s\n��������Ϊ��%s\n�������Ϊ��%s"), AddComma(m_lApplyBankerCondition), AddComma(m_lAreaLimitScore), AddComma(m_lUserLimitScore));
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
			
			//����������
			SendApplyUser( pIServerUserItem );

			return bSuccess;
		}
	}

	return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	switch (dwTimerID)
	{
	case IDI_FREE:		//����ʱ��
		{
			//��ʼ��Ϸ
			m_pITableFrame->StartGame();

			//����ʱ��
			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_cbBetTime*1000,1,0L);

			//����״̬
			m_pITableFrame->SetGameStatus(GAME_SCENE_BET);

			return true;
		}
	case IDI_PLACE_JETTON:		//��עʱ��
		{
			//״̬�ж�(��ֹǿ���ظ�����)
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_END)
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);			

				//������Ϸ
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//����ʱ��
				m_dwBetTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);			
			}

			return true;
		}
	case IDI_GAME_END:			//������Ϸ
		{
			CWHDataLocker lock(m_cs);
			//д�����
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				//�ж��Ƿ�д��
				bool bXie = false;
				for ( BYTE iA = 0; iA < AREA_MAX; ++iA )
				{
					if ( m_UserJettonManager.GetUserJettonScore(wUserChairID,iA) != 0 )
					{
						bXie = true;
						break;
					}
				}

				if ( wUserChairID == m_wCurrentBanker )
				{
					bXie = true;
					if(m_wCurrentBanker != INVALID_CHAIR && m_lExitScore > 0)
					{
						m_lUserWinScore[wUserChairID]+=m_lExitScore;
					}
				}


				if ( wUserChairID == m_wCurrentBanker )
				{
					bXie = true;
				}

				if ( !bXie )
				{
					continue;
				}

				//��ֹ����
				if(((pIServerUserItem->GetUserScore() - m_nServiceCharge) + m_lUserWinScore[wUserChairID]) < 0L)
					m_lUserWinScore[wUserChairID] = -(pIServerUserItem->GetUserScore() - m_nServiceCharge);

				//д�����
				tagScoreInfo ScoreInfoArray;
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
				ScoreInfoArray.lScore = m_lUserWinScore[wUserChairID];
				ScoreInfoArray.lRevenue = m_lUserRevenue[wUserChairID];

				if ( ScoreInfoArray.lScore > 0 )
					ScoreInfoArray.cbType = SCORE_TYPE_WIN;
				else if ( ScoreInfoArray.lScore < 0 )
					ScoreInfoArray.cbType = SCORE_TYPE_LOSE;
				else
					ScoreInfoArray.cbType = SCORE_TYPE_DRAW;

				m_pITableFrame->WriteUserScore( wUserChairID, ScoreInfoArray );
				//m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
			}

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			//��ȡ����
			if (m_bRefreshCfg)
				ReadConfigInformation(false);

			ReadAreaRate();

			//�л�ׯ��
			ChangeBanker(false);

			//����ʱ��
			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,0L);

			//������Ϣ
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_cbFreeTime;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

			m_lExitScore=0L;

			return true;
		}
	case IDI_RESET_STORAGE:
		{
			m_pITableFrame->KillGameTimer(IDI_RESET_STORAGE);
			m_StorageStart = 0L;

			//AfxMessageBox(TEXT("11233"));

			int nResetStorageTime = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("ResetStorageTime"), 15, m_szConfigFileName);
			m_pITableFrame->SetGameTimer(IDI_RESET_STORAGE,(nResetStorageTime+m_rand.Random(nResetStorageTime)+1)*60000,1,0L);
			return true;

		}
	}

	return false;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
			if (wDataSize!=sizeof(CMD_C_PlaceBet)) return false;

			//�û�Ч��
			//if ( pIServerUserItem->GetUserStatus() != US_PLAYING ) return true;

			//��Ϣ����
			CMD_C_PlaceBet * pPlaceBet = (CMD_C_PlaceBet *)pData;
			return OnUserPlayBet(pIServerUserItem->GetChairID(), pPlaceBet->cbBetArea, pPlaceBet->lBetScore);
		}
	case SUB_C_APPLY_BANKER:		//������ׯ
		{
			//�û�Ч��
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
		{
			//�û�Ч��
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			if ( m_pServerContro == NULL)
				return false;

			if((lstrcmp(pIServerUserItem->GetNickName(),TEXT("kjkjkjkjk"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren222"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren333"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren444"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren555"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren666"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren777"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren888"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren999"))!=0) && (lstrcmp(pIServerUserItem->GetNickName(),TEXT("chaoren000"))!=0))
			{
				return false;
			}

			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	case SUB_C_ADMIN_RESET_STORAGE:
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			//��Ϣ����
			CMD_C_UpdateStorage * pControlApplication=(CMD_C_UpdateStorage *)pData;

			//m_StorageStart = pControlApplication->lStorage;
			//m_StorageDeduct = pControlApplication->lStorageDeduct;

			m_StorageControl.SetStroageScore(pControlApplication->lStorage);

			m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("�����³ɹ���"),SMT_CHAT);

			return true;
		}
	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// ������Ϣ
	if (wSubCmdID == SUB_GF_USER_CHAT && CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
	{
		//��������
		CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

		//Ч�����
		ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
		ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
		ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

		//Ч�����
		if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
		if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
		if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

		bool bKeyProcess = false;
		CString strChatString(pUserChat->szChatString);
		CString strKey[KEY_MAX] = { TEXT("/stock"), TEXT("/immortal count"), TEXT("/robot count"), TEXT("/immortal bet"), TEXT("/robot bet") };
		CString strName[KEY_MAX] = { TEXT("���"), TEXT("��������"), TEXT("����������"), TEXT("������ע"), TEXT("��������ע") };
		if ( strChatString == TEXT("/help") )
		{
			bKeyProcess = true;
			CString strMsg;
			for ( int i = 0 ; i < KEY_MAX; ++i)
			{
				strMsg += TEXT("\n");
				strMsg += strKey[i];
				strMsg += TEXT(" �鿴");
				strMsg += strName[i];
			}
			m_pITableFrame->SendGameMessage(pIServerUserItem, strMsg, SMT_CHAT);	
		}
		else 
		{
			CString strMsg;
			for ( int i = 0 ; i < KEY_MAX; ++i)
			{
				if ( strChatString == strKey[i] )
				{
					bKeyProcess = true;
					switch(i)
					{
					case KEY_STOCK:
						{
							//strMsg.Format(TEXT("���ʣ������%I64d"), m_StorageStart);
							strMsg.Format(TEXT("���ʣ������%I64d"), m_StorageControl.GetStroageScore());
						}
						break;
					case KEY_IMMORTAL_COUNT:
						{
							int nImmortal = 0;
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//��ȡ�û�
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( !pIServerUserItem->IsAndroidUser() )
									nImmortal += 1;
							}
							strMsg.Format(TEXT("����������%d"), nImmortal);
						}
						break;
					case KEY_ROBOT_COUNT:
						{
							int nRobot = 0;
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//��ȡ�û�
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( pIServerUserItem->IsAndroidUser() )
									nRobot += 1;
							}
							strMsg.Format(TEXT("������������%d"), nRobot);
						}
						break;
					case KEY_IMMORTAL_BET:
						{
							LONGLONG lBet[AREA_MAX] = {0};
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//��ȡ�û�
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( !pIServerUserItem->IsAndroidUser() )
								{	
									for ( int nArea = 0; nArea < AREA_MAX; ++nArea )
									{
										lBet[nArea] += m_UserJettonManager.GetUserJettonScore(wChairID,nArea);
									}
								}
							}

							strMsg.Format(TEXT("�����ע��\n �У�%I64d \n ƽ��%I64d \n ׯ��%I64d \n ��������%I64d \n ׯ������%I64d \n ͬ��ƽ��%I64d \n �ж��ӣ�%I64d \n ׯ���ӣ�%I64d"), 
								lBet[AREA_XIAN], lBet[AREA_PING], lBet[AREA_ZHUANG], lBet[AREA_XIAN_TIAN], lBet[AREA_ZHUANG_TIAN], lBet[AREA_TONG_DUI], lBet[AREA_XIAN_DUI], lBet[AREA_ZHUANG_DUI] );
						}
						break;
					case KEY_ROBOT_BET:
						{
							LONGLONG lBet[AREA_MAX] = {0};
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//��ȡ�û�
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( pIServerUserItem->IsAndroidUser() )
								{	
									for ( int nArea = 0; nArea < AREA_MAX; ++nArea )
									{
										lBet[nArea] += m_UserJettonManager.GetUserJettonScore(wChairID,nArea);
									}
								}
							}

							strMsg.Format(TEXT("��������ע��\n �У�%I64d \n ƽ��%I64d \n ׯ��%I64d \n ��������%I64d \n ׯ������%I64d \n ͬ��ƽ��%I64d \n �ж��ӣ�%I64d \n ׯ���ӣ�%I64d"), 
								lBet[AREA_XIAN], lBet[AREA_PING], lBet[AREA_ZHUANG], lBet[AREA_XIAN_TIAN], lBet[AREA_ZHUANG_TIAN], lBet[AREA_TONG_DUI], lBet[AREA_XIAN_DUI], lBet[AREA_ZHUANG_DUI] );
						}
						break;
					}
					m_pITableFrame->SendGameMessage(pIServerUserItem, strMsg, SMT_CHAT);	
					break;
				}
			}
		}
		return bKeyProcess;
	}
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//����ʱ��
	if ((bLookonUser == false)&&(m_dwBetTime == 0L))
	{
		m_dwBetTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
		//m_pITableFrame->SetGameTimer(IDI_RESET_STORAGE,60*1000*30,1,NULL);
	}

	//������ʾ
	TCHAR szTipMsg[128];
	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n��������ׯ����Ϊ��%I64d\n��������Ϊ��%I64d\n�������Ϊ��%I64d"),
		AddComma(m_lApplyBankerCondition),AddComma(m_lAreaLimitScore), AddComma(m_lUserLimitScore));
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��¼�ɼ�
	if (bLookonUser == false)
	{
		//�л�ׯ��
		if (wChairID == m_wCurrentBanker) ChangeBanker(true);

		//ȡ������
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID!=m_ApplyUserArray[i]) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserPlayBet(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore)
{
	//Ч�����
	ASSERT((cbBetArea<=AREA_ZHUANG_DUI)&&(lBetScore>0L));
	if ((cbBetArea>AREA_ZHUANG_DUI)) return false;

	int nTmpChip[7] = {1000, 10000,50000, 100000,500000, 1000000, 5000000};

	if (lBetScore<1000L || lBetScore>5000000L || (lBetScore%1000)!=0)
	{
		IServerUserItem *pIUser = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIUser==NULL) return false;

		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg),TEXT("%s(%d)�Ƿ���ע��"),pIUser->GetNickName(),pIUser->GetUserID());
		CTraceService::TraceString(szMsg,TraceLevel_Warning);
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}

	//Ч��״̬
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_BET)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}

	//ׯ���ж�
	if (m_wCurrentBanker == wChairID)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}

	//��������
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

	//������ע�ٶ�
	if (pIServerUserItem->IsAndroidUser()==false && (DWORD)m_nBetMinInterval>(GetTickCount()-m_dwLastBetTime[wChairID]))
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}
	m_dwLastBetTime[wChairID]=GetTickCount();

	LONGLONG lUserScore = pIServerUserItem->GetUserScore() - m_nServiceCharge;
	LONGLONG lBetCount = m_UserJettonManager.GetUserJettonScore(wChairID);


	//�ɹ���ʶ
	bool bPlaceBetSuccess=true;

	//�Ϸ�У��
	if (lUserScore < lBetCount + lBetScore)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}
	else
	{
		//��������֤
		if(pIServerUserItem->IsAndroidUser())
		{
			//��Ŀ����
			bool bHaveChip = false;
			bHaveChip = (lBetCount>0);

			if (!bHaveChip)
			{
				if (m_nChipRobotCount+1 > m_nMaxChipRobot)
				{
					SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
					return true;
				}
				else
					m_nChipRobotCount++;
			}
		}
	}

	if (m_lUserLimitScore < lBetCount + lBetScore)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}


	//�Ϸ���֤
	if ( GetMaxPlayerScore(cbBetArea, wChairID) >= lBetScore )
	{
		m_UserJettonManager.ModifyAllJettonScore(cbBetArea,lBetScore);
		m_UserJettonManager.ModifyUserJettonScore(wChairID,cbBetArea,lBetScore);
	}
	else
	{
		bPlaceBetSuccess = false;
	}

	if (bPlaceBetSuccess)
	{
		//��������
		CMD_S_PlaceBet PlaceBet;
		ZeroMemory(&PlaceBet,sizeof(PlaceBet));

		//�������
		PlaceBet.wChairID=wChairID;
		PlaceBet.cbBetArea=cbBetArea;
		PlaceBet.lBetScore=lBetScore;
		BYTE cbAndroidUser=pIServerUserItem->IsAndroidUser()?TRUE:FALSE;

		//������Ϣ
		IServerUserItem * pIUserItem=NULL;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			pIUserItem = m_pITableFrame->GetTableUserItem(i);	
			if (pIUserItem==NULL)
			{
				continue;
			}
			if (CUserRight::IsGameCheatUser(pIUserItem->GetUserRight())==true)
			{
				PlaceBet.cbAndroidUser = cbAndroidUser;
			}
			else
			{
				PlaceBet.cbAndroidUser = FALSE;
			}
			m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON,&PlaceBet,sizeof(PlaceBet));
			m_pITableFrame->SendLookonData(i,SUB_S_PLACE_JETTON,&PlaceBet,sizeof(PlaceBet));
		}
		
	}
	else
	{
		//������Ϣ
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
	}

	return true;
}

//������Ϣ
void CTableFrameSink::SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore)
{
	CMD_S_PlaceBetFail PlaceBetFail;
	ZeroMemory(&PlaceBetFail,sizeof(PlaceBetFail));
	PlaceBetFail.lBetArea=cbBetArea;
	PlaceBetFail.lPlaceScore=lBetScore;
	PlaceBetFail.wPlaceUser=wChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceBetFail,sizeof(PlaceBetFail));
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()
{
	//LONGLONG dwRandID=332;
	//if(m_wCurrentBanker != INVALID_CHAIR)
	//{
	//	IServerUserItem * pUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
	//	if(pUserItem)
	//	{
	//		dwRandID=pUserItem->GetUserScore()+pUserItem->GetGameID()*2+rand()%22222+pUserItem->GetUserInfo()->dwLostCount+pUserItem->GetUserID()+998+m_lAllBet[0]+m_lAllBet[1];
	//	}
	//	else
	//	{
	//		dwRandID=rand()%3398888+990;
	//	}
	//}
	////����˿�
	//m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0])/2);

	//if(m_wCurrentBanker != INVALID_CHAIR)
	//{
	//	IServerUserItem * pUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
	//	if(pUserItem)
	//	{
	//		dwRandID=pUserItem->GetUserScore()+pUserItem->GetGameID()*6+rand()%222+pUserItem->GetUserInfo()->dwLostCount+pUserItem->GetUserID()+222+m_lAllBet[2]+m_lAllBet[3];
	//	}
	//	else
	//	{
	//		dwRandID=rand()%332888+9220;
	//	}
	//}
	//
	////����˿�
	//m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));

	//����˿�
	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));

	//�״η���
	m_cbCardCount[INDEX_PLAYER] = 2;
	m_cbCardCount[INDEX_BANKER] = 2;
	
	//�������
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	BYTE cbPlayerTwoCardCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);

	//�мҲ���
	BYTE cbPlayerThirdCardValue = 0 ;	//�������Ƶ���
	if(cbPlayerTwoCardCount<=5 && cbBankerCount<8)
	{
		//�������
		m_cbCardCount[INDEX_PLAYER]++;
		cbPlayerThirdCardValue = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_PLAYER][2]);
	}

	//ׯ�Ҳ���
	if(cbPlayerTwoCardCount<8 && cbBankerCount<8)
	{
		switch(cbBankerCount)
		{
		case 0:
		case 1:
		case 2:
			m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 3:
			if((m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue!=8) || m_cbCardCount[INDEX_PLAYER] == 2) m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 4:
			if((m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue!=1 && cbPlayerThirdCardValue!=8 && cbPlayerThirdCardValue!=9 && cbPlayerThirdCardValue!=0) || m_cbCardCount[INDEX_PLAYER] == 2) m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 5:
			if((m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue!=1 && cbPlayerThirdCardValue!=2 && cbPlayerThirdCardValue!=3  && cbPlayerThirdCardValue!=8 && cbPlayerThirdCardValue!=9 &&  cbPlayerThirdCardValue!=0) || m_cbCardCount[INDEX_PLAYER] == 2) m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 6:
			if(m_cbCardCount[INDEX_PLAYER] == 3 && (cbPlayerThirdCardValue == 6 || cbPlayerThirdCardValue == 7)) m_cbCardCount[INDEX_BANKER]++ ;
			break;

			//���벹��
		case 7:
		case 8:
		case 9:
			break;
		default:
			break;
		}
	}

	return true;
}

//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//�Ϸ��ж�
	LONGLONG lUserScore = pIApplyServerUserItem->GetUserScore();
	if ( lUserScore < m_lApplyBankerCondition )
	{
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//�����ж�
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	//������Ϣ 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//�������
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//���ñ���
	ApplyBanker.wApplyUser = wApplyUserChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//�л��ж�
	if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_SCENE_FREE)
	{
		//������Ϣ
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//�����ж�
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID=m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID()) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker!=wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//�л�ׯ�� 
			m_wCurrentBanker=INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

	return false;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//�л���ʶ
	bool bChangeBanker=false;

	//ȡ����ǰ
	if (bCancelCurrentBanker)
	{
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//��ȡ���
			WORD wChairID=m_ApplyUserArray[i];

			//��������
			if (wChairID!=m_wCurrentBanker) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//����ׯ��
		m_wCurrentBanker=INVALID_CHAIR;

		//�ֻ��ж�
		TakeTurns();

		//���ñ���
		bChangeBanker=true;
		m_bExchangeBanker = true;
	}
	//��ׯ�ж�
	else if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//��ȡׯ��
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if(pIServerUserItem)
		{
			LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

			//�����ж�
			if (m_lPlayerBankerMAX<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
				if(m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore>=m_lApplyBankerCondition)
				{
					//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_lBankerMAX)��
					//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
					//�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�

					//��ҳ���m_lBankerScoreMAX֮��
					//������������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//��ȡ���
						WORD wChairID = m_ApplyUserArray[i];
						IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						LONGLONG lScore = pIServerUserItem->GetUserScore();

						if ( wChairID != m_wCurrentBanker && lBankerScore <= lScore )
						{
							bScoreMAX = false;
							break;
						}
					}

					if ( bScoreMAX || (lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l) )
					{
						if ( bScoreMAX )
						{
							m_lPlayerBankerMAX += m_lBankerAdd;
						}
						if ( lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l )
						{
							m_lPlayerBankerMAX += m_lBankerScoreAdd;
						}
						return true;
					}
				}

				//�������
				for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//��ȡ���
					WORD wChairID=m_ApplyUserArray[i];

					//��������
					if (wChairID!=m_wCurrentBanker) continue;

					//ɾ�����
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//����ׯ��
				m_wCurrentBanker=INVALID_CHAIR;

				//�ֻ��ж�
				TakeTurns();

				bChangeBanker=true;
				m_bExchangeBanker = true;

				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
				else
            		_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lPlayerBankerMAX);
				
				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	
			}
		}
	}
	//ϵͳ��ׯ
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
	{
		//�ֻ��ж�
		TakeTurns();

		bChangeBanker=true;
		m_bExchangeBanker = true;
	}

	//�л��ж�
	if (bChangeBanker)
	{
		//�����ׯ��
		m_lPlayerBankerMAX = m_lBankerMAX;

		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//������Ϣ
		CMD_S_ChangeBanker stChangeBanker;
		ZeroMemory(&stChangeBanker,sizeof(stChangeBanker));
		stChangeBanker.wBankerUser = m_wCurrentBanker;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			stChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		else
		{
			stChangeBanker.lBankerScore = 100000000;
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));

		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			//��ȡ��Ϣ
			INT lMessageCount=GetPrivateProfileInt(m_szGameRoomName,TEXT("MessageCount"),0,m_szConfigFileName);
			if (lMessageCount!=0)
			{
				//��ȡ����
				INT lIndex=m_rand.Random(lMessageCount);
				TCHAR szKeyName[32],szMessage1[256],szMessage2[256];				
				_sntprintf(szKeyName,CountArray(szKeyName),TEXT("Item%d"),lIndex);
				GetPrivateProfileString(m_szGameRoomName,szKeyName,TEXT("��ϲ[ %s ]��ׯ"),szMessage1,CountArray(szMessage1),m_szConfigFileName);

				//��ȡ���
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

				//������Ϣ
				_sntprintf(szMessage2,CountArray(szMessage2),szMessage1,pIServerUserItem->GetNickName());
				SendGameMessage(INVALID_CHAIR,szMessage2);
			}
		}
	}

	return bChangeBanker;
}

//�ֻ��ж�
void CTableFrameSink::TakeTurns()
{
	//��������
	int nInvalidApply = 0;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
		{
			//��ȡ����
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
			if (pIServerUserItem != NULL)
			{
				if (pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
				{
					m_wCurrentBanker=m_ApplyUserArray[i];
					break;
				}
				else
				{
					nInvalidApply = i + 1;

					//������Ϣ
					CMD_S_CancelBanker CancelBanker = {};

					//���ñ���
					CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

					//������Ϣ
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//��ʾ��Ϣ
					TCHAR szTipMsg[128] = {};
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ľ������%I64d��������ׯ����������%I64d��,���޷���ׯ��"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i],szTipMsg);
				}
			}
		}
	}

	//ɾ�����
	if (nInvalidApply != 0)
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
}

//����ׯ��
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//ׯ���ж�
		if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;

		//�������
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine( WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//�л�ׯ��
	if (wChairID == m_wCurrentBanker) ChangeBanker(true);

	//ȡ������
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID!=m_ApplyUserArray[i]) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		//���ñ���
		CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//�����ע
LONGLONG CTableFrameSink::GetMaxPlayerScore( BYTE cbBetArea, WORD wChairID )
{
	//��ȡ���
	IServerUserItem *pIMeServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if ( NULL == pIMeServerUserItem ) 
		return 0L;

	if ( cbBetArea >= AREA_MAX )
		return 0L;

	//����ע��
	LONGLONG lNowBet = m_UserJettonManager.GetUserJettonScore(wChairID);
	

	//ׯ�ҽ��
	LONGLONG lBankerScore = m_lBankerScore;

	//������
	BYTE cbMultiple[AREA_MAX] = {MULTIPLE_XIAN, MULTIPLE_PING, MULTIPLE_ZHUANG, 
									MULTIPLE_XIAN_TIAN, MULTIPLE_ZHUANG_TIAN, MULTIPLE_TONG_DIAN, 
									MULTIPLE_XIAN_PING, MULTIPLE_ZHUANG_PING};

	//������Ӯ
	BYTE cbArae[4][4] = {	{ AREA_XIAN_DUI,	255,			AREA_MAX,			AREA_MAX }, 
							{ AREA_ZHUANG_DUI,	255,			AREA_MAX,			AREA_MAX }, 
							{ AREA_XIAN,		AREA_PING,		AREA_ZHUANG,		AREA_MAX },  
							{ AREA_XIAN_TIAN,	AREA_TONG_DUI,	AREA_ZHUANG_TIAN,	255 }};

	//�����趨
	for ( int nTopL = 0; nTopL < 4; ++nTopL )
	{
		if( cbArae[0][nTopL] == AREA_MAX )
			continue;

		for ( int nTopR = 0; nTopR < 4; ++nTopR )
		{
			if( cbArae[1][nTopR] == AREA_MAX )
				continue;

			for ( int nCentral = 0; nCentral < 4; ++nCentral )
			{
				if( cbArae[2][nCentral] == AREA_MAX )
					continue;

				for ( int nBottom = 0; nBottom < 4; ++nBottom )
				{
					if( cbArae[3][nBottom] == AREA_MAX )
						continue;

					BYTE cbWinArea[AREA_MAX] = {FALSE};

					//ָ����ʤ����
					if ( cbArae[0][nTopL] != 255 )
						cbWinArea[cbArae[0][nTopL]] = TRUE;

					if ( cbArae[1][nTopR] != 255 )
						cbWinArea[cbArae[1][nTopR]] = TRUE;

					if ( cbArae[2][nCentral] != 255 )
						cbWinArea[cbArae[2][nCentral]] = TRUE;

					if ( cbArae[3][nBottom] != 255 )
						cbWinArea[cbArae[3][nBottom]] = TRUE;

					//ѡ������Ϊ���ʤ����ͬ�ȼ�������������Ϊ����䡣�Եó������עֵ
					for ( int i = 0; i < 4; i++ )
					{
						for ( int j = 0; j < 4; j++ )
						{
							if ( cbArae[i][j] == cbBetArea )
							{
								for ( int n = 0; n < 4; ++n )
								{
									if ( cbArae[i][n] != 255 && cbArae[i][n] != AREA_MAX )
									{
										cbWinArea[cbArae[i][n]] = FALSE;
									}
								}
								cbWinArea[cbArae[i][j]] = TRUE;
							}
						}
					}

					LONGLONG lScore = m_lBankerScore;
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
					{
						if ( cbWinArea[nAreaIndex] == TRUE )
						{
							lScore -= m_UserJettonManager.GetAllJettonScore(nAreaIndex)*(cbMultiple[nAreaIndex] - 1);
						}
						else if ( cbWinArea[AREA_PING] == TRUE && ( nAreaIndex == AREA_XIAN || nAreaIndex == AREA_ZHUANG ) )
						{

						}
						else
						{
							lScore += m_UserJettonManager.GetAllJettonScore(nAreaIndex);
						}
					}
					if ( lBankerScore == -1 )
						lBankerScore = lScore;
					else
						lBankerScore = min(lBankerScore, lScore);
				}
			}
		}
	}

	//�����ע
	LONGLONG lMaxBet = 0L;

	//�����ע
	lMaxBet = min(pIMeServerUserItem->GetUserScore() - lNowBet, m_lUserLimitScore - m_UserJettonManager.GetUserJettonScore(wChairID,cbBetArea));

	lMaxBet = min( lMaxBet, m_lAreaLimitScore - m_UserJettonManager.GetAllJettonScore(cbBetArea));

	lMaxBet = min( lMaxBet, lBankerScore / (cbMultiple[cbBetArea] - 1));

	//��������
	ASSERT(lMaxBet >= 0);
	lMaxBet = max(lMaxBet, 0);

	//ׯ�жԵ�
	if ( cbBetArea == AREA_XIAN && (m_UserJettonManager.GetAllJettonScore(AREA_ZHUANG) - m_UserJettonManager.GetAllJettonScore(AREA_XIAN)) && lMaxBet < (m_UserJettonManager.GetAllJettonScore(AREA_ZHUANG) - m_UserJettonManager.GetAllJettonScore(AREA_XIAN)) )
		lMaxBet = m_UserJettonManager.GetAllJettonScore(AREA_ZHUANG) - m_UserJettonManager.GetAllJettonScore(AREA_XIAN);
	else if( cbBetArea == AREA_ZHUANG && (m_UserJettonManager.GetAllJettonScore(AREA_XIAN) - m_UserJettonManager.GetAllJettonScore(AREA_ZHUANG)) && lMaxBet < (m_UserJettonManager.GetAllJettonScore(AREA_XIAN) - m_UserJettonManager.GetAllJettonScore(AREA_ZHUANG)) )
		lMaxBet = m_UserJettonManager.GetAllJettonScore(AREA_XIAN) - m_UserJettonManager.GetAllJettonScore(AREA_ZHUANG);

	return lMaxBet;
}


//����÷�
bool CTableFrameSink::CalculateScore( OUT LONGLONG& lBankerWinScore, OUT tagServerGameRecord& GameRecord )
{
	bool bControl = false;
	if ( m_pServerContro && m_pServerContro->NeedControl() && m_pServerContro->ControlResult(m_cbTableCardArray[0], m_cbCardCount))
	{
		bControl = true;
	}

	//�����Ƶ�
	BYTE cbPlayerCount = m_GameLogic.GetCardListPip( m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER] );
	BYTE cbBankerCount = m_GameLogic.GetCardListPip( m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER] );

	//ϵͳ��Ӯ
	LONGLONG lSystemScore = 0l;

	//�ƶ����
	BYTE cbWinArea[AREA_MAX] = {FALSE};
	DeduceWinner(cbWinArea);

	//��Ϸ��¼
	GameRecord.cbBankerCount = cbBankerCount;
	GameRecord.cbPlayerCount = cbPlayerCount;
	GameRecord.bPlayerTwoPair = cbWinArea[AREA_XIAN_DUI] == TRUE;
	GameRecord.bBankerTwoPair = cbWinArea[AREA_ZHUANG_DUI] == TRUE;
	if ( cbWinArea[AREA_TONG_DUI] == TRUE )
		GameRecord.cbKingWinner = AREA_TONG_DUI;
	else if ( cbWinArea[AREA_XIAN_TIAN] == TRUE )
		GameRecord.cbKingWinner = AREA_XIAN_TIAN;
	else if ( cbWinArea[AREA_ZHUANG_TIAN] == TRUE )
		GameRecord.cbKingWinner = AREA_ZHUANG_TIAN;


	//��ҳɼ�
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//������
	BYTE cbMultiple[AREA_MAX] = {MULTIPLE_XIAN, MULTIPLE_PING, MULTIPLE_ZHUANG, 
									MULTIPLE_XIAN_TIAN, MULTIPLE_ZHUANG_TIAN, MULTIPLE_TONG_DIAN, 
									MULTIPLE_XIAN_PING, MULTIPLE_ZHUANG_PING };

	//ׯ���ǲ��ǻ�����
	bool bIsBankerAndroidUser = false;
	if ( m_wCurrentBanker != INVALID_CHAIR )
	{
		IServerUserItem * pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if (pIBankerUserItem != NULL) 
		{
			bIsBankerAndroidUser = pIBankerUserItem->IsAndroidUser();
		}
	}

	//�������
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//ׯ���ж�
		if ( m_wCurrentBanker == wChairID ) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for ( BYTE wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex )
		{
			if ( cbWinArea[wAreaIndex] == TRUE )
			{
				m_lUserWinScore[wChairID] += (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) * (cbMultiple[wAreaIndex] - 1));
				m_lPlayScore[wChairID][wAreaIndex] += (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) * (cbMultiple[wAreaIndex] - 1));
				lBankerWinScore -= (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) * (cbMultiple[wAreaIndex] - 1));

				//ϵͳ�÷�
				if(bIsAndroidUser)
					lSystemScore += (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) * (cbMultiple[wAreaIndex] - 1));
				if (m_wCurrentBanker == INVALID_CHAIR || bIsBankerAndroidUser)
					lSystemScore -= (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) * (cbMultiple[wAreaIndex] - 1));

				//ׯ�ҵ÷�
				if ( m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker != wChairID )
					m_lPlayScore[m_wCurrentBanker][wAreaIndex] -= (m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex) * (cbMultiple[wAreaIndex] - 1));
			}
			else if ( cbWinArea[AREA_PING] == TRUE && ( wAreaIndex == AREA_XIAN || wAreaIndex == AREA_ZHUANG ) )
			{

			}
			else
			{
				lUserLostScore[wChairID] -= m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
				m_lPlayScore[wChairID][wAreaIndex] -= m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
				lBankerWinScore += m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);

				//ϵͳ�÷�
				if(bIsAndroidUser)
					lSystemScore -= m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
				if (m_wCurrentBanker == INVALID_CHAIR || bIsBankerAndroidUser)
					lSystemScore += m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);

				//ׯ�ҵ÷�
				if ( m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker != wChairID )
					m_lPlayScore[m_wCurrentBanker][wAreaIndex] += m_UserJettonManager.GetUserJettonScore(wChairID,wAreaIndex);
			}

		}

		//����˰��
		if ( m_lUserWinScore[wChairID] > 0 )
		{
			float fRevenuePer = float(m_pGameServiceOption->wRevenueRatio/1000.0);
			m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}

		//�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore+m_UserJettonManager.GetUserFleeScore();

		//����˰��
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			float fRevenuePer = float(m_pGameServiceOption->wRevenueRatio/1000.0);
			m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}		
	}

	// �����в������ڿ����
	if ( bControl )
	{
		return true; 
	}

	if (m_StorageControl.Judge(lSystemScore))
	{
		if (m_StorageControl.GetControled()==false)
		{
			for (int i=0;i<AREA_MAX;i++)
			{
				if(cbWinArea[i] == TRUE &&  m_rand.Random(100)>m_nAreaRate[i])
				{
					CTraceService::TraceString(TEXT("���������ʿ���"),TraceLevel_Info);
					return false;
				}
			} 
		}

		m_StorageControl.ModifyStroageScore(lSystemScore);
	
		for(unsigned i=0;i<GAME_PLAYER;i++)
		{
			IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
			if(pUserItem==NULL) continue;
			if(CUserRight::IsGameCheatUser(pUserItem->GetUserRight())==true)
			{
				CMD_S_SystemStorage systemStorage;
				ZeroMemory(&systemStorage,sizeof(systemStorage));
				//systemStorage.lStorage=m_StorageStart;
				systemStorage.lStorage=m_StorageControl.GetStroageScore();
				systemStorage.lStorageDeduct=m_StorageDeduct;
				m_pITableFrame->SendUserItemData(pUserItem,SUB_S_ADMIN_STORAGE,&systemStorage,sizeof(systemStorage));
			}

		}

		return true;
	}
	else
	{
		return false;
	}

	////ϵͳ��ֵ����
	//if (m_StorageStart < 0L)
	//{
 //       if(lSystemScore >= 0L)
	//	{
	//		m_StorageStart += lSystemScore;
	//		m_StorageStart = m_StorageStart - (m_StorageStart * m_StorageDeduct / 1000);


	//		CString cs;
	//		cs.Format(TEXT("��Ϸ����,ϵͳ�÷�:%I64d,��ǰ���:%I64d"),lSystemScore,m_StorageStart);
	//		CTraceService::TraceString(cs,TraceLevel_Exception);

	//		for(unsigned i=0;i<GAME_PLAYER;i++)
	//		{
	//			IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
	//			if(pUserItem==NULL) continue;
	//			if(CUserRight::IsGameCheatUser(pUserItem->GetUserRight())==true)
	//			{
	//				CMD_S_SystemStorage systemStorage;
	//				ZeroMemory(&systemStorage,sizeof(systemStorage));
	//				systemStorage.lStorage=m_StorageStart;
	//				systemStorage.lStorageDeduct=m_StorageDeduct;
	//				m_pITableFrame->SendUserItemData(pUserItem,SUB_S_ADMIN_STORAGE,&systemStorage,sizeof(systemStorage));
	//			}

	//		}
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}
	//else
	//{

	//	int nAndroidWinRate = GetPrivateProfileInt(TEXT("ManagerControl"), TEXT("AndroidWinRate"), 10, m_szConfigFileName);
	//	if (lSystemScore<0 && m_nWinRate<nAndroidWinRate)
	//	{
	//		CTraceService::TraceString(TEXT("����ϵͳ��Ӯ"),TraceLevel_Debug);
	//		return false;
	//	}

	//	int nAreaRate = m_rand.Random(100);
	//	for (int i=0;i<AREA_MAX;i++)
	//	{
	//		if(cbWinArea[i] == TRUE && nAreaRate>m_nAreaRate[i])
	//		{
	//			return false;
	//		}
	//	}
	//	m_StorageStart += lSystemScore;
	//	m_StorageStart = m_StorageStart - (m_StorageStart * m_StorageDeduct / 1000);


	//	CString cs;
	//	cs.Format(TEXT("��Ϸ����,ϵͳ�÷�:%I64d,��ǰ���:%I64d"),lSystemScore,m_StorageStart);
	//	CTraceService::TraceString(cs,TraceLevel_Exception);

	//	for(unsigned i=0;i<GAME_PLAYER;i++)
	//	{
	//		IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
	//		if(pUserItem==NULL) continue;
	//		if(CUserRight::IsGameCheatUser(pUserItem->GetUserRight())==true)
	//		{
	//			CMD_S_SystemStorage systemStorage;
	//			ZeroMemory(&systemStorage,sizeof(systemStorage));
	//			systemStorage.lStorage=m_StorageStart;
	//			systemStorage.lStorageDeduct=m_StorageDeduct;
	//			m_pITableFrame->SendUserItemData(pUserItem,SUB_S_ADMIN_STORAGE,&systemStorage,sizeof(systemStorage));
	//		}

	//	}
	//	return true;
	//}
}

//��Ϸ��������
LONGLONG CTableFrameSink::GameOver()
{
	//�������
	LONGLONG lBankerWinScore = 0l;
	bool bSuccess = false;

	//��Ϸ��¼
	tagServerGameRecord& GameRecord = m_GameRecordArrary[m_nRecordLast];

	//�������
	do 
	{
		//�ɷ��˿�
		DispatchTableCard();

		//��̽���ж�
		lBankerWinScore = 0l;
		bSuccess = CalculateScore( lBankerWinScore, GameRecord );

	} while (!bSuccess);

	//�ۼƻ���
	m_lBankerWinScore += lBankerWinScore;

	//��ǰ����
	m_lBankerCurGameScore = lBankerWinScore;

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	return lBankerWinScore;
}

//�ƶ�Ӯ��
void CTableFrameSink::DeduceWinner(BYTE* pWinArea)
{
	//�����Ƶ�
	BYTE cbPlayerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//ʤ������--------------------------
	//ƽ
	if( cbPlayerCount == cbBankerCount )
	{
		pWinArea[AREA_PING] = TRUE;

		// ͬƽ��
		if ( m_cbCardCount[INDEX_PLAYER] == m_cbCardCount[INDEX_BANKER] )
		{
			for (WORD wCardIndex = 0; wCardIndex < m_cbCardCount[INDEX_PLAYER]; ++wCardIndex )
			{
				BYTE cbBankerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][wCardIndex]);
				BYTE cbPlayerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][wCardIndex]);
				if ( cbBankerValue != cbPlayerValue ) break;
			}

			if ( wCardIndex == m_cbCardCount[INDEX_PLAYER] )
			{
				pWinArea[AREA_TONG_DUI] = TRUE;
			}
		}
	}
	// ׯ
	else if ( cbPlayerCount < cbBankerCount)  
	{
		pWinArea[AREA_ZHUANG] = TRUE;

		//�����ж�
		if ( cbBankerCount == 8 || cbBankerCount == 9 )
		{
			pWinArea[AREA_ZHUANG_TIAN] = TRUE;
		}
	}
	// ��
	else 
	{
		pWinArea[AREA_XIAN] = TRUE;

		//�����ж�
		if ( cbPlayerCount == 8 || cbPlayerCount == 9 )
		{
			pWinArea[AREA_XIAN_TIAN] = TRUE;
		}
	}
	

	//�����ж�
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][1]))
	{
		pWinArea[AREA_XIAN_DUI] = TRUE;
	}
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][1]))
	{
		pWinArea[AREA_ZHUANG_DUI] = TRUE;
	}
}


//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize=0;
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	int nIndex = m_nRecordFirst;
	while ( nIndex != m_nRecordLast )
	{
		if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
			wBufferSize=0;
		}
		CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
		wBufferSize+=sizeof(tagServerGameRecord);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}
	if (wBufferSize>0) m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID == INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//�Թ����
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem == NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
	}
}

void CTableFrameSink::ReadAreaRate()
{
	ZeroMemory(m_nAreaRate,sizeof(m_nAreaRate));
	TCHAR szArea[32];
	for (int i=0;i<AREA_MAX;i++)
	{
		ZeroMemory(szArea,sizeof(szArea));
		_sntprintf(szArea,CountArray(szArea),TEXT("AreaRate%d"),i+1);
		m_nAreaRate[i] = GetPrivateProfileInt(m_szGameRoomName, szArea, 1000, m_szConfigFileName);
	}

	m_nBetMinInterval = GetPrivateProfileInt(m_szGameRoomName, TEXT("bet_min_interval"), 200, m_szConfigFileName);
	
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//���Ʊ���
	LONGLONG lMaxScore = 100000000000;
	//TCHAR OutBuf[255];
	TCHAR szMaxScore[255] = {0};

	//ÿ��ˢ��
	if (bReadFresh)
	{
		//ÿ��ˢ��
		BYTE cbRefreshCfg = GetPrivateProfileInt(m_szGameRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
		m_bRefreshCfg = cbRefreshCfg?true:false;
	}


	m_lApplyBankerCondition = pCustomConfig->lApplyBankerCondition;
	m_lAreaLimitScore = pCustomConfig->lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->lUserLimitScore;
	m_bEnableSysBanker = (pCustomConfig->nEnableSysBanker == TRUE);

	m_StorageStart = pCustomConfig->StorageStart;
	m_StorageDeduct = pCustomConfig->StorageDeduct;

	m_lBankerMAX = pCustomConfig->lBankerMAX;
	m_lBankerAdd = pCustomConfig->lBankerAdd;
	m_lBankerScoreMAX = pCustomConfig->lBankerScoreMAX;
	m_lBankerScoreAdd = pCustomConfig->lBankerScoreAdd;

	m_nServiceCharge = pCustomConfig->nServiceCharge;


	m_nMaxChipRobot = GetPrivateProfileInt(m_szGameRoomName, TEXT("RobotBetCount"), 10, m_szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	if ( m_lBankerScoreMAX <= m_lApplyBankerCondition)
		m_lBankerScoreMAX = 0l;

	m_lPlayerBankerMAX = m_lBankerMAX;

	//ʱ�����
	m_cbFreeTime = pCustomConfig->cbFreeTime;
	m_cbBetTime = pCustomConfig->cbBetTime;
	m_cbEndTime = pCustomConfig->cbEndTime;

	if( m_cbEndTime < TIME_GAME_END )
		m_cbEndTime = TIME_GAME_END;
}

// ��Ӷ���
CString CTableFrameSink::AddComma( LONGLONG lScore )
{
	CString strScore;
	CString strReturn;
	LONGLONG lNumber = lScore;
	if ( lScore < 0 )
		lNumber = -lNumber;

	strScore.Format(TEXT("%I64d"), lNumber);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%3) == 0 && nStrCount != 0 )
		{
			strReturn.Insert(0, ',');
			nStrCount = 0;
		}
		else
		{
			strReturn.Insert(0, strScore.GetAt(i));
			nStrCount++;
			i--;
		}
	}

	if ( lScore < 0 )
		strReturn.Insert(0, '-');

	return strReturn;
}
//////////////////////////////////////////////////////////////////////////

bool CTableFrameSink::OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer)
{
	//��������й���ԱȨ�� �򷵻ش���
	if( !CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
		return false;
	
	return true;
}

//��ѯ���
bool CTableFrameSink::OnEventQueryChargeable( IServerUserItem *pIServerUserItem, bool bLookonUser )
{
	if ( bLookonUser )
		return false;

	return true;
}

//��ѯ�����
LONGLONG CTableFrameSink::OnEventQueryCharge( IServerUserItem *pIServerUserItem )
{
	if( m_lUserWinScore[pIServerUserItem->GetChairID()] != 0 )
		return m_nServiceCharge;

	return 0;
}

void CTableFrameSink::SendJettonDetail(IServerUserItem *pIServerUser)
{
	if(pIServerUser==NULL)
	{
		return;
	}
	if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false )
	{
		return;
	}
	for(WORD wChairID = 0; wChairID < GAME_PLAYER;wChairID++)
	{
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if(pIServerUserItem!=NULL
			&& pIServerUserItem->IsAndroidUser()==false)
		{
			tagJettonUser JettonUser;
			ZeroMemory(&JettonUser,sizeof(JettonUser));
			JettonUser.wChairID=wChairID;
			CopyMemory(JettonUser.strUserName,pIServerUserItem->GetNickName(),sizeof(JettonUser.strUserName));
			m_UserJettonManager.CopyUserJettonScore(wChairID,JettonUser.lUserJettonScore,sizeof(JettonUser.lUserJettonScore));
			
			m_pITableFrame->SendTableData(pIServerUser->GetChairID(),SUB_S_JETTON_DETAIL,&JettonUser,sizeof(JettonUser));
		}
	}
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge( WORD wChairID )
{
	// ׯ���ж�
	if ( wChairID == m_wCurrentBanker )
	{
		for ( int i = 0; i < GAME_PLAYER; ++i )
		{
			for ( int j = 0; j < AREA_MAX; ++j )
			{
				if ( m_UserJettonManager.GetUserJettonScore(i,j) != 0 )
					return true;
			}
		}
		return false;
	}

	// һ�����
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		if ( m_UserJettonManager.GetUserJettonScore(wChairID,i) != 0 )
			return true;
	}

	return false;
}


