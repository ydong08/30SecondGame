#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"
#include "RandomHelper.h"
#include "StorageControl.h"
#include "UserJettonManager.h"

//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY			16									//��ʷ����
//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��ע��
protected:	
	CUserJettonManager				m_UserJettonManager;

	//����
protected:
	LONGLONG						m_lBankerScore;							//ׯ�һ���
	LONGLONG						m_lPlayScore[GAME_PLAYER][AREA_MAX];	//�����Ӯ
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//���˰��
	DWORD							m_dwLastBetTime[GAME_PLAYER];			//�ϴ���עʱ��
	//���Ʊ���
protected:
	LONGLONG						m_lAreaLimitScore;						//��������
	LONGLONG						m_lUserLimitScore;						//��������
	LONGLONG						m_lApplyBankerCondition;				//��������
	TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�
	int								m_nServiceCharge;						//�����
	int								m_nAreaRate[AREA_MAX];					//�������
	int								m_nBetMinInterval;						//��ע��������룩
	CStorageControl					m_StorageControl;
	CCriticalSection				m_cs;

	//�˿���Ϣ
protected:
	BYTE							m_cbCardCount[2];						//�˿���Ŀ
	LONGLONG                        m_lExitScore;
    BYTE							m_cbTableCardArray[2][3];				//�����˿�

	//״̬����
protected:
	DWORD							m_dwBetTime;							//��ʼʱ��

	//ׯ����Ϣ
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//�������
	WORD							m_wCurrentBanker;						//��ǰׯ��
	WORD							m_wBankerTime;							//��ׯ����
	LONGLONG						m_lBankerWinScore;						//�ۼƳɼ�
	LONGLONG						m_lBankerCurGameScore;					//��ǰ�ɼ�
	bool							m_bEnableSysBanker;						//ϵͳ��ׯ

	//��¼����
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	int								m_nRecordFirst;							//��ʼ��¼
	int								m_nRecordLast;							//����¼
	DWORD							m_dwRecordCount;						//��¼��Ŀ

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame	*					m_pITableFrame;							//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���
	CRandomHelper					m_rand;
	//�������
protected:
	HINSTANCE						m_hControlInst;
	IServerControl*					m_pServerContro;

	//���Ʊ���
protected:
	bool							m_bRefreshCfg;							//ÿ��ˢ��
	TCHAR							m_szGameRoomName[SERVER_LEN];			//��������
	LONGLONG						m_StorageStart;							//��������ÿ���ӵĿ����ֵ����ȡʧ�ܰ� 0 ����
	LONGLONG						m_StorageDeduct;						//ÿ����Ϸ������۳��Ŀ���������ȡʧ�ܰ� 1.00 ����


	//ׯ������
protected:
	//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_lBankerMAX)��
	//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
	//�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�
	LONGLONG						m_lBankerMAX;							//���ׯ����
	LONGLONG						m_lBankerAdd;							//ׯ��������

	//��ҳ���m_lBankerScoreMAX֮�󣬾�����������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
	LONGLONG						m_lBankerScoreMAX;						//ׯ��Ǯ
	LONGLONG						m_lBankerScoreAdd;						//ׯ��Ǯ��ʱ,��ׯ������

	//���ׯ����
	LONGLONG						m_lPlayerBankerMAX;						//������ׯ����

	//��ׯ
	bool							m_bExchangeBanker;						//����ׯ��

	//ʱ������
protected:
	BYTE							m_cbFreeTime;							//����ʱ��
	BYTE							m_cbBetTime;							//��עʱ��
	BYTE							m_cbEndTime;							//����ʱ��

	//�����˿���
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release();
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0; }
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

public:
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�����¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }
	
	//����¼�
public:
	//��ѯ���
	virtual bool OnEventQueryChargeable(IServerUserItem *pIServerUserItem, bool bLookonUser);
	//��ѯ�����
	virtual LONGLONG OnEventQueryCharge(IServerUserItem *pIServerUserItem);


	//��Ϸ�¼�
protected:
	//��ע�¼�
	bool OnUserPlayBet(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore);
	//����ׯ��
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//ȡ������
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//��������
private:
	//�����˿�
	bool DispatchTableCard();
	//����ׯ��
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//����ׯ��
	bool ChangeBanker(bool bCancelCurrentBanker);
	//�ֻ��ж�
	void TakeTurns();
	//���ͼ�¼
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//������Ϣ
	void SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore);
	//����Ա����
	bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer);
	//��ȡ����
	void ReadConfigInformation(bool bReadFresh);
	// ��Ӷ���
	CString AddComma( LONGLONG lScore );
	//��ȡ�����������
	void ReadAreaRate();

	//��ע����
private:
	//�����ע
	LONGLONG GetMaxPlayerScore( BYTE cbBetArea, WORD wChairID );

	//��Ϸͳ��
private:
	//��Ϸ��������
	LONGLONG GameOver();
	//����÷�
    bool CalculateScore(OUT LONGLONG& lBankerWinScore, OUT tagServerGameRecord& GameRecord);
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE* pWinArea);
	//���ƽ��
	void ControlResult();
	//��ע��Ϣ
	void SendJettonDetail(IServerUserItem *pIServerUser);
};

//////////////////////////////////////////////////////////////////////////

#endif
