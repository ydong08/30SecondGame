#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						122									//��Ϸ I D
#define GAME_PLAYER					150							//��Ϸ����
#define GAME_NAME					TEXT("�۰�30��")						//��Ϸ����
#define AREA_COUNT					                8					//������Ŀ

//�������
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//����汾

//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//��Ϸ����
#define GAME_SCENE_BET				GAME_STATUS_PLAY					//��ע״̬
#define	GAME_SCENE_END				GAME_STATUS_PLAY+1					//����״̬

//�������
#define AREA_XIAN					0									//�м�����
#define AREA_BEGIN					AREA_XIAN
#define AREA_PING					1									//ƽ������
#define AREA_ZHUANG					2									//ׯ������
#define AREA_XIAN_TIAN				3									//������
#define AREA_ZHUANG_TIAN			4									//ׯ����
#define AREA_TONG_DUI				5									//ͬ��ƽ
#define AREA_XIAN_DUI				6									//�ж���
#define AREA_ZHUANG_DUI				7									//ׯ����
#define AREA_MAX					8									//�������
#define AREA_RESERVE				255

//������multiple
#define MULTIPLE_XIAN				2									//�мұ���
#define MULTIPLE_PING				9									//ƽ�ұ���
#define MULTIPLE_ZHUANG				2									//ׯ�ұ���
#define MULTIPLE_XIAN_TIAN			3									//����������
#define MULTIPLE_ZHUANG_TIAN		3									//ׯ��������
#define MULTIPLE_TONG_DIAN			33									//ͬ��ƽ����
#define MULTIPLE_XIAN_PING			12									//�ж��ӱ���
#define MULTIPLE_ZHUANG_PING		12									//ׯ���ӱ���


//���ʶ���
#define RATE_TWO_PAIR				12									//��������
#define SERVER_LEN					32									//���䳤��

//��¼��Ϣ
struct tagServerGameRecord
{
	BYTE							cbKingWinner;						//����Ӯ��
	bool							bPlayerTwoPair;						//���ӱ�ʶ
	bool							bBankerTwoPair;						//���ӱ�ʶ
	BYTE							cbPlayerCount;						//�мҵ���
	BYTE							cbBankerCount;						//ׯ�ҵ���
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE				99									//��Ϸ����
#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			101									//�û���ע
#define SUB_S_GAME_END				102									//��Ϸ����
#define SUB_S_APPLY_BANKER			103									//����ׯ��
#define SUB_S_CHANGE_BANKER			104									//�л�ׯ��
#define SUB_S_CHANGE_USER_SCORE		105									//���»���
#define SUB_S_SEND_RECORD			106									//��Ϸ��¼
#define SUB_S_PLACE_JETTON_FAIL		107									//��עʧ��
#define SUB_S_CANCEL_BANKER			108									//ȡ������
#define SUB_S_AMDIN_COMMAND			109									//����Ա����
#define SUB_S_JETTON_DETAIL         110                                 //��ע�б�
#define SUB_S_ADMIN_STORAGE         211                                 //���֪ͨ

//����ظ�
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//�ظ�����
		 #define ACK_SET_WIN_AREA  1
		 #define ACK_PRINT_SYN     2
		 #define ACK_RESET_CONTROL 3
	BYTE cbResult;
	#define CR_ACCEPT  2			//����
	#define CR_REFUSAL 3			//�ܾ�
	BYTE cbExtendData[20];			//��������
};

//���¿��
struct CMD_S_SystemStorage
{
	LONGLONG						lStorage;						//�¿��ֵ
	LONGLONG						lStorageDeduct;					//���˥��
};

//ʧ�ܽṹ
struct CMD_S_PlaceBetFail
{
	WORD							wPlaceUser;							//��ע���
	BYTE							lBetArea;							//��ע����
	LONGLONG						lPlaceScore;						//��ǰ��ע
};

//����ׯ��
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//�������
};

//ȡ������
struct CMD_S_CancelBanker
{
	WORD							wCancelUser;						//ȡ�����
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//��ׯ���
	LONGLONG						lBankerScore;						//ׯ�ҷ���
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�����Ϣ
	LONGLONG						lPlayFreeSocre;						//������ɽ��

	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	LONGLONG						lBankerScore;						//ׯ�ҷ���
	LONGLONG						lBankerWinScore;					//ׯ��Ӯ��
	WORD							wBankerTime;						//ׯ�Ҿ���

	//�Ƿ�ϵͳ��ׯ
	bool							bEnableSysBanker;					//ϵͳ��ׯ

	//������Ϣ
	LONGLONG						lApplyBankerCondition;				//��������
	LONGLONG						lAreaLimitScore;					//��������

	//������Ϣ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BYTE							cbGameStatus;						//��Ϸ״̬

	//��ע��
	LONGLONG						lAllBet[AREA_MAX];					//����ע
	LONGLONG						lPlayBet[AREA_MAX];					//�����ע

	//��һ���
	LONGLONG						lPlayBetScore;						//��������ע	
	LONGLONG						lPlayFreeSocre;						//������ɽ��

	//�����Ӯ
	LONGLONG						lPlayScore[AREA_MAX];				//�����Ӯ
	LONGLONG						lPlayAllScore;						//��ҳɼ�

	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	LONGLONG						lBankerScore;						//ׯ�ҷ���
	LONGLONG						lBankerWinScore;					//ׯ��Ӯ��
	WORD							wBankerTime;						//ׯ�Ҿ���

	//�Ƿ�ϵͳ��ׯ
	bool							bEnableSysBanker;					//ϵͳ��ׯ

	//������Ϣ
	LONGLONG						lApplyBankerCondition;				//��������
	LONGLONG						lAreaLimitScore;					//��������

	//�˿���Ϣ
 	BYTE							cbCardCount[2];						//�˿���Ŀ
	BYTE							cbTableCardArray[2][3];				//�����˿�

	//������Ϣ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������
};

//��Ϸ����
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	WORD							wBankerUser;						//ׯ��λ��
	LONGLONG						lBankerScore;						//ׯ�ҽ��

	LONGLONG						lPlayBetScore;						//��������ע	
	LONGLONG						lPlayFreeSocre;						//������ɽ��

	int								nChipRobotCount;					//�������� (��ע������)
	__int64                         nListUserCount;						//�б�����
	int								nAndriodCount;						//�������б�����
	BYTE                            bRealUser[GAME_PLAYER];              //������ 
};

//�û���ע
struct CMD_S_PlaceBet
{
	WORD							wChairID;							//�û�λ��
	BYTE							cbBetArea;							//��������
	LONGLONG						lBetScore;							//��ע��Ŀ
	BYTE							cbAndroidUser;						//������ʶ
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//�¾���Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�˿���Ϣ
	BYTE							cbCardCount[2];						//�˿���Ŀ
	BYTE							cbTableCardArray[2][3];				//�����˿�
 
	//ׯ����Ϣ
	LONGLONG						lBankerScore;						//ׯ�ҳɼ�
	LONGLONG						lBankerTotallScore;					//ׯ�ҳɼ�
	INT								nBankerTime;						//��ׯ����

	//��ҳɼ�
	LONGLONG						lPlayScore[AREA_MAX];				//��ҳɼ�
	LONGLONG						lPlayAllScore;						//��ҳɼ�

	//ȫ����Ϣ
	LONGLONG						lRevenue;							//��Ϸ˰��
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON			1									//�û���ע
#define SUB_C_APPLY_BANKER			2									//����ׯ��
#define SUB_C_CANCEL_BANKER			3									//ȡ������
#define SUB_C_AMDIN_COMMAND			4									//����Ա����
#define SUB_C_ADMIN_RESET_STORAGE   8                                  //������


struct CMD_C_AdminReq
{
	BYTE cbReqType;
		 #define RQ_SET_WIN_AREA	1
		 #define RQ_RESET_CONTROL	2
		 #define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//��������
	LONGLONG nServalValue;
};


//struct CMD_C_AdminReq
//{
//	BYTE cbReqType;
//		 #define RQ_SETWINAREA   1 
//	BYTE cbExtendData[120];
//};


//�û���ע
struct CMD_C_PlaceBet
{
	BYTE							cbBetArea;						//��������
	LONGLONG						lBetScore;						//��ע��Ŀ
};

struct tagJettonUser
{
	//�����Ϣ
	WORD                            wChairID;
	TCHAR                           strUserName[32];                                              //����ʺ�
	LONGLONG                        lUserJettonScore[AREA_MAX+1];                             //��ҽ��
};

//���¿��
struct CMD_C_UpdateStorage
{
	LONGLONG						lStorage;						//�¿��ֵ
	LONGLONG						lStorageDeduct;					//���˥��
};


//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
