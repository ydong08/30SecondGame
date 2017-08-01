#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "GameLogic.h"
#include "DlgViewChart.h"
#include "ClientControl.h"
#include "RealJettonInfo.h"
//////////////////////////////////////////////////////////////////////////

//���붨��
#define BET_COUNT					8									//������Ŀ
#define JETTON_RADII				68									//����뾶

//��Ϣ����
#define IDM_PALY_BET				WM_USER+200							//��ס��Ϣ
#define IDM_APPLY_BANKER			WM_USER+201							//������Ϣ
#define IDM_PLAY_SOUND				WM_USER+202							//������Ϣ

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ
struct tagBetInfo
{
	int								nXPos;								//����λ��
	int								nYPos;								//����λ��
	BYTE							cbBetIndex;							//��������
};

//ѡȡ������Ϣ
struct tagFlashInfo
{
	BYTE							cbFlashArea;					//��ʾ����
	int								nFlashAreaAlpha;				//��ʾ͸����
	bool							bFlashAreaAlpha;				//�Ƿ���ʾ
};

enum enFlexMode
{
	enFlexNULL,
	enFlexBetTip,
	enFlexDealCrad,
	enFlexGameEnd,
};

//��������
typedef CWHArray<tagBetInfo,tagBetInfo&> CBetInfoArray;

//������
class CGameClientEngine;


//////////////////////////////////////////////////////////////////////////
//��ť״̬
enum enButtonState 
{ 					
	enNormal,
	enDown,
	enUp,
	enMouseOver,
	enDisabled
};
//Png��ť�ṹ
class CPngButton
{
	//��������
protected:
	UINT                        m_nID;                      //��ťID��
	CPoint						m_ptLeftTop;			    //��ťλ��
	CPngImage					m_pngImage;					//��ť��Դ
	CWnd*                       m_wndParent;
	LPCTSTR                     m_pszCaption;
	DWORD                       m_dwStyle;
	int							m_nWidth;					//��ť���
	int							m_nHeight;					//��ť�߶�
	enButtonState				m_nState;;				    //��ť״̬
	bool						m_bShow;					//�Ƿ�ɼ�
	CRect                       m_rcWindow;                 //��ť����
    bool                        m_bTrackMouse;              //��꾭��
	bool                        m_bAutoDraw;
	//����
public:
	CPngButton()
	{
		m_bShow =false;
		m_nState = enNormal;
		m_ptLeftTop.x = m_ptLeftTop.y = m_nWidth= m_nHeight= 0;
		m_wndParent=NULL;
		m_bTrackMouse=false;
		m_bAutoDraw=true;
	}
	~CPngButton(){}

	bool CreateButton(LPCTSTR lpszCaption, DWORD dwStyle,const RECT& rect, CWnd* pParentWnd, UINT nID)
	{
		m_pszCaption=lpszCaption;
		m_dwStyle=dwStyle;
		m_rcWindow=rect;
		m_wndParent=pParentWnd;
		m_nID=nID;
		if((m_dwStyle&WS_VISIBLE)!=0)
			m_bShow=true;
		if((m_dwStyle&WS_DISABLED)!=0)
		{
			m_nState = enDisabled;
		}
		return true;
	}
	void SetButtonImage(HINSTANCE hInstance, LPCTSTR pszResource) 
	{
		m_pngImage.LoadImage(hInstance,pszResource); 
		m_nWidth = m_pngImage.GetWidth()/5; 
		m_nHeight = m_pngImage.GetHeight();
		m_rcWindow.right = m_rcWindow.left+m_nWidth; 
		m_rcWindow.bottom = m_rcWindow.top + m_nHeight;
	}
	int GetWidth()
	{
		return m_nWidth;
	}
	int GetHeight()
	{
		return m_nHeight;
	}
	void SetAutoDraw(bool bDraw)
	{
		m_bAutoDraw=bDraw;
	}
	bool GetAutoDraw()
	{
		return m_bAutoDraw;
	}
	void SetButtonState(enButtonState enState)
	{
		if(enState==enUp)
		{
			if(m_nState==enDown)
			{
				if(m_wndParent!=NULL)
				{
					m_wndParent->SendMessage(WM_COMMAND,MAKELONG(m_nID,BN_CLICKED),NULL);
				}
			}
		}

		m_nState = enState;
		
	}

	void ShowButton(bool bShow)
	{
		m_bShow = bShow;
	}
	void EnableButton(bool bEnable)
	{
		if(bEnable)
		    m_nState = enNormal;
		else
            m_nState = enDisabled;
	}
    void SetButtonPos(int x,int y)
	{
		m_rcWindow.left = m_ptLeftTop.x = x; 
		m_rcWindow.top = m_ptLeftTop.y = y; 
		m_rcWindow.right = m_rcWindow.left+m_nWidth; 
		m_rcWindow.bottom = m_rcWindow.top + m_nHeight;
	}
	bool IsInButtonRect(CPoint p)
	{
		return m_rcWindow.PtInRect(p)==TRUE;
	}

	bool IsEnableButton()
	{ 
		return m_nState != enDisabled;
	}

	bool IsShowButton()
	{
		return m_bShow;
	}

	bool IsTrackMouse()
	{
		return m_bTrackMouse;
	}
	void SetTrackMouse(bool bTrackMouse)
	{
		m_bTrackMouse=bTrackMouse;
	}

	void DrawButton(CDC *pDC)
	{ 
		if(!m_pngImage.IsNull())
		{
			if (m_bShow)
				m_pngImage.DrawImage(pDC, m_ptLeftTop.x, m_ptLeftTop.y, m_nWidth,m_nHeight, m_nState*m_nWidth,0, m_nWidth,m_nHeight);
		}
	}
	void GetButtonRect(CRect &rect)
	{
		rect=m_rcWindow;
	}
};

typedef CWHArray<CPngButton *>		PngButtonArray;			//������Ŀ
//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewGDI
{
	//������Ϣ
protected:
	LONGLONG						m_lMeMaxScore;						//�����ע
    LONGLONG						m_lAreaLimitScore;					//��������

    LONGLONG						m_lAllPlayBet[GAME_PLAYER][AREA_COUNT+1];//���������ע

	CGameClientEngine					*m_pGameClientDlg;					//����ָ��

	//��ע��
protected:
	LONGLONG						m_lAllBet[AREA_MAX];				//����ע
	LONGLONG						m_lPlayBet[AREA_MAX];				//�����ע


	//λ����Ϣ
protected:
	CWHArray<CRect>					m_ArrayBetArea[AREA_MAX];			//��ע����
	CPoint							m_ptBetFrame[AREA_MAX];				//��ע��ʾλ��
	CSize							m_sizeWin;							//���ڴ�С

	//�˿���Ϣ
protected:	
	BYTE							m_cbCardCount[2];					//�˿���Ŀ
    BYTE							m_cbTableCardArray[2][3];			//�����˿�

	//��������
protected:
	int								m_nDealMove;						//���Ʋ���
	int								m_nDealIndex;						//��������
	CPoint							m_ptDispatchCard;					//����λ��
	CCardControl					m_DispatchCard;						//�����˿�

	//��������
protected:
	enFlexMode						m_enFlexMode;						//��������
	int								m_nFlexMove;						//��������
	bool							m_bFlexShow;						//��ʾ
	CPoint							m_ptFlexBeing;						//�ƶ�λ��
	CPoint							m_ptFlexMove;						//�ƶ�λ��
	CPngImage*						m_pImageFlex;						//�ƶ�ͼƬ

	//��ʷ��Ϣ
protected:
	LONGLONG						m_lMeStatisticScore;				//��Ϸ�ɼ�
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];//��Ϸ��¼
	int								m_nRecordFirst;						//��ʼ��¼
	int								m_nRecordLast;						//����¼
	int								m_nScoreHead;						//�ɼ�λ��

	tagClientGameRecord				m_TraceGameRecordArray[100];		//·����¼
	int								m_TraceGameRecordCount;				//·����Ŀ
	bool							m_bFillTrace[6][25];				//����ʶ

	int                             m_nShowRecordCount;
	//״̬����
protected:
	LONG							m_nWinCount;						//Ӯ����
	LONG							m_nLoseCount;						//������
	BYTE							m_cbGameStatus;						//��Ϸ״̬
	WORD							m_wMeChairID;						//�ҵ�λ��
	LONGLONG						m_lCurrentBet;						//��ǰ����
	CString							m_strDispatchCardTips;				//������ʾ
	bool							m_bShowChangeBanker;				//�ֻ�ׯ��
	bool							m_bNeedSetGameRecord;				//�������

	//������
protected:
	CWHArray<BYTE>					m_ArrayFlashArea;					//ʤ�����
	int								m_nFlashAreaAlpha;					//ʤ����ʾ
	bool							m_bFlashAreaAlpha;					//ʤ����ʾ
	CWHArray<tagFlashInfo>			m_ArrayCurrentFlash;				//ѡ������

	//ׯ����Ϣ
protected:	
	WORD							m_wBankerUser;						//��ǰׯ��
	WORD							m_wBankerTime;						//��ׯ����
	LONGLONG						m_lBankerScore;						//ׯ�һ���
	LONGLONG						m_lBankerWinScore;					//ׯ�ҳɼ�	
	LONGLONG						m_lBankerWinTempScore;				//ׯ�ҳɼ�[��ʱ����]
	bool							m_bEnableSysBanker;					//ϵͳ��ׯ

	//���ֳɼ�
public:
	LONGLONG						m_lPlayScore[AREA_MAX];				//�����Ӯ
	LONGLONG						m_lPlayAllScore;					//�������Ӯ
	LONGLONG                        m_LBankerWinScoreCurrent;

	//��������
public:
	int								m_nWinShowArea;						//Ӯ��������
	int								m_nWinShowIndex[3];					//Ӯ��������
    bool                            m_bShowWinType;
	//���ݱ���
protected:
	CPoint							m_ptBetNumber[AREA_MAX];			//����λ��
	CPoint							m_ptAllBetNumber[AREA_MAX];			//����λ��
	CBetInfoArray					m_BetInfoArray[AREA_MAX];			//��������
    CPoint                          m_ptBack;
	CPoint                          m_ptBottom;
	//�ؼ�����
public:
	//CSkinButton						m_btBet1000;						//���밴ť
	//CSkinButton						m_btBet10000;						//���밴ť
	//CSkinButton						m_btBet100000;						//���밴ť
	//CSkinButton						m_btBet500000;						//���밴ť
	//CSkinButton						m_btBet1000000;						//���밴ť
	//CSkinButton						m_btBet5000000;						//���밴ť
	//CSkinButton						m_btBet10000000;					//���밴ť
	CSkinButton						m_btViewChart;						//�鿴·��
	
	//CSkinButton						m_btApplyBanker;					//����ׯ��
	//CSkinButton						m_btCancelBanker;					//ȡ��ׯ��

	//CSkinButton						m_btScoreMoveL;						//�ƶ��ɼ�
	//CSkinButton						m_btScoreMoveR;						//�ƶ��ɼ�

	//CSkinButton						m_btValleysUp;						//��ť
	//CSkinButton						m_btValleysDown;					//��ť

	CButton							m_btOpenAdmin;                      //����
	CButton                         m_btRealJettonInfo;                  //�鿴��ʵ��ע��Ϣ


	//������Դ
public:
	CDFontEx						m_FontValleysListOne;				//��ׯ�б�����һ
	CDFontEx						m_FontValleysListTwo;				//��ׯ�б������

	//�ؼ�����
public:
	INT								m_nShowValleyIndex;					//��ʾ��ׯ�б�
	CWHArray<WORD>					m_ValleysList;						//��ׯ�б�			
	CCardControl					m_CardControl[2];					//�˿˿ؼ�	
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CDlgViewChart					m_DlgViewChart;						//�鿴·��

	//�������
protected:
	CBitImage						m_ImageViewFill;					//����λͼ
	CPngImage						m_ImageViewBack;					//����λͼ
	CPngImage						m_ImageViewBackJB;					//����λͼ
	CPngImage						m_ImageWinFlags;					//��־λͼ
	CPngImage						m_ImageWinFlagsCard;					//��־λͼ
	CPngImage						m_ImageBetView;						//������ͼ
	CPngImage						m_ImageScoreBack;					//������ͼ
	CPngImage						m_ImageScoreNumber;					//������ͼ
	CPngImage						m_ImageMeScoreBack;					//������ͼ
	CPngImage						m_ImageMeScoreNumber;				//������ͼ


	//�߿���Դ
protected:
	CPngImage						m_ImageFrame[AREA_MAX];				//�߿�ͼƬ

	CPngImage						m_ImageMeBanker;					//�л�ׯ��
	CPngImage						m_ImageChangeBanker;				//�л�ׯ��
	CPngImage						m_ImageNoBanker;					//�л�ׯ��

	//��������
protected:
	CPngImage						m_ImageBetTip;						//��ע��ʾ
	CPngImage						m_ImageGameEnd;						//�ɼ�����
	CPngImage						m_ImageDealBack;					//���Ʊ���

	//������Դ
protected:
	CPngImage						m_ImageGamePoint;					//����ͼƬ
	CPngImage						m_ImageGameEndMyScore;				//��ҽ�������
	CPngImage						m_ImageGameEndAllScore;				//��ҽ�������
	CPngImage						m_ImageGameEndPoint;				//�����������

	CPngImage						m_ImageWinType;						//��Ӯ����
	CPngImage						m_ImageWinXian;						//����
	CPngImage						m_ImageWinZhuang;					//ׯ��


	//�������
protected:
	CPngImage						m_ImageBrandBoxRight;				//�ƺ���

	CPngImage						m_ImageTimeNumber;					//ʱ������
	CPngImage						m_ImageTimeBack;					//ʱ�䱳��
	CPngImage						m_ImageTimeType;					//ʱ���ʶ
	CPngImage						m_ImageTimeFlag;					//ʱ���ʶ

	CPngImage						m_ImagePlayLeft;					//�����Ϣ��
	CPngImage						m_ImagePlayMiddle;					//�����Ϣ��
	CPngImage						m_ImagePlayRight;					//�����Ϣ��

	//CPngImage						m_ImageScoreInfo;					//������Ϣ
	CPngImage						m_ImageBankerInfo;					//ׯ����Ϣ
	//CPngImage						m_ImageWaitValleys;					//�ȴ���ׯ
	CPngImage						m_ImageWaitFirst;					//��һ�ȴ�
	
	CPngImage                       m_pngMeBack;
public:
	bool							m_blCanStore;                       //�Ƿ��ܱ���
	CSkinButton						m_btBankerStorage;					//��ť
	CSkinButton						m_btBankerDraw;						//ȡ�ť

	//����
public:
	HINSTANCE						m_hControlInst;
	IClientControlDlg*				m_pClientControlDlg;
	CRealJettonInfo                 m_pDlgJettonInfo;
	CListCtrl						m_SkinListCtrl[AREA_MAX];

	//png��ť
public:
	CPngButton                      m_btPngApplyBanker;
	CPngButton						m_btPngCancelBanker;					//ȡ��ׯ��

	CPngButton                      m_btPngDraw;

	CPngButton						m_btPngViewChart;						//�鿴·��
	CPngButton						m_btPngViewCard;						//�鿴·��
    bool                            m_bShowChart;

	CPngButton						m_btPngScoreMoveL;						//�ƶ��ɼ�
	CPngButton						m_btPngScoreMoveR;						//�ƶ��ɼ�

	CPngButton						m_btPngScrollUp;						//�ƶ��ɼ�
	CPngButton						m_btPngScrollDown;

	CPngButton						m_btPngState;
	bool                            m_bShowState;
	CPngImage                       m_pngState;


	CPngImage						m_PngPlayerEXFlag;					//�м�ͼƬ
	CPngImage						m_PngBankerEXFlag;					//ׯ��ͼƬ
	CPngImage						m_PngTieEXFlag;						//ƽ��ͼƬ

	


    CPngButton						m_btPngBet1000;						    //���밴ť
	CPngButton						m_btPngBet10000;					    //���밴ť
	CPngButton						m_btPngBet50000;					    //���밴ť
	CPngButton						m_btPngBet100000;					    //���밴ť
	CPngButton						m_btPngBet500000;					    //���밴ť
	CPngButton						m_btPngBet1000000;					    //���밴ť
	CPngButton						m_btPngBet5000000;						//���밴ť
	CPngButton						m_btPngBet10000000;					    //���밴ť


	PngButtonArray                  m_pngButtonArray;

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

	//���ƽӿ�
public:
	//���ý���
	virtual VOID ResetGameView();

	//�̳к���
private:
	//�����ؼ�
	virtual VOID RectifyControl(int nWidth, int nHeight);
	//�滭����
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	//WIN7֧��
	virtual bool RealizeWIN7() { return true; }


	//���ú���
public:
	//����״̬
	VOID SetGameStatus(BYTE cbGameStatus);
	//�ҵ�λ��
	void SetMeChairID(WORD wMeChairID);
	//���������ע
	void SetPlayBetScore(LONGLONG lPlayBetScore);
	//��������
	void SetAreaLimitScore(LONGLONG lAreaLimitScore);
	//ׯ����Ϣ
	void SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore);
	//ׯ����Ϣ
	void SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore, LONGLONG lBankerWinScore, WORD wBankerTime);
	//ׯ����Ϣ
	void SetBankerOverInfo(LONGLONG lBankerWinScore, WORD wBankerTime,LONGLONG LBankerWinScoreCurrent);
	//����ϵͳ�Ƿ���ׯ
	void SetEnableSysBanker(bool bEnableSysBanker);
	//������ע
	void SetPlayBet(BYTE cbViewIndex, LONGLONG lBetCount);
	//ȫ����ע
	void SetAllBet(BYTE cbViewIndex, LONGLONG lBetCount);
	//���ó���
	void AddChip(BYTE cbViewIndex, LONGLONG lScoreCount);
	//���ֳɼ�
	void SetCurGameScore(LONGLONG lPlayScore[AREA_MAX], LONGLONG lPlayAllScore);
	//�����˿�
	void SetCardInfo(BYTE cbCardCount[2], BYTE cbTableCardArray[2][3]);
	//���ó���
	void SetCurrentBet(LONGLONG lCurrentBet);
	//��ʷ��¼
	void SetGameHistory(enOperateResult OperateResult, BYTE cbPlayerCount, BYTE cbBankerCount, BYTE cbKingWinner, bool bPlayerTwoPair, bool bBankerTwoPair);
	//�ֻ�ׯ��
	void ShowChangeBanker( bool bChangeBanker );

	//��������
public:	
	//��ʼ����
	void DispatchCard();
	//��������
	void FinishDispatchCard(bool bScene = false);
	//��������
	void FlexAnimation(enFlexMode nFlexMode, bool bShow, bool bMove = true);
	//��˸����
	void FlashAnimation( bool bBegin );


	//�滭����
protected:
	// �滭������Ϣ
	void DrawTopInfo(CDC *pDC, int nWidth, int nHeight);
	// �滭�ײ���Ϣ
	void DrawBottomInfo(CDC *pDC, int nWidth, int nHeight);
	//��ʾ��Ӯ
	void DrawWinType(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY );
	//��ʾ���
	void DrawGameOver(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY );
	// ��˸�߿�
	void DrawFlashFrame(CDC *pDC, int nWidth, int nHeight);

	//��������
public:
	//�����ע
	LONGLONG GetMaxPlayerScore(BYTE cbBetArea);

	//���溯��
public:
	//�������
	void CleanUserBet();
	//������ʾ
	void SetDispatchCardTips();
	//������ע���
	void ControlBetDetection(BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair );

	//��������
public:
	//��ǰ����
	inline LONGLONG GetCurrentBet() { return m_lCurrentBet; }	

	//�ڲ�����
private:
	//��ȡ����
	BYTE GetBetArea(CPoint MousePoint);
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE* pWinArea);
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner, bool &bPlayerTwoPair, bool &bBankerTwoPair);
	// ��Ӷ���
	CString AddComma( LONGLONG lScore , bool bPlus = false);
	// ɾ������
	LONGLONG DeleteComma( CString strScore );

	//�滭���亯��
private:
	//��������
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//�滭�ַ�
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat);
	//��������
	void DrawTextString(CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//�滭�ַ�
	void DrawTextString(CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat);
	// �滭����
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	// �滭ʱ��
	void DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos);

	//��ť��Ϣ
protected:
	//���밴ť
	afx_msg void OnBetButton50000();
	//���밴ť
	afx_msg void OnBetButton1000();
	//���밴ť
	afx_msg void OnBetButton10000();
	//���밴ť
	afx_msg void OnBetButton100000();
	//���밴ť
	afx_msg void OnBetButton500000();
	//���밴ť
	afx_msg void OnBetButton1000000();
	//���밴ť
	afx_msg void OnBetButton5000000();
	//���밴ť
	afx_msg void OnBetButton10000000();
	//�鿴·��
	afx_msg void OnViewChart();
	afx_msg void OnViewCard();
	//��ׯ��ť
	afx_msg void OnApplyBanker();
	//��ׯ��ť
	afx_msg void OnCancelBanker();
	//�ƶ���ť
	afx_msg void OnScoreMoveL();
	//�ƶ���ť
	afx_msg void OnScoreMoveR();
	//�Ϸ�ҳ
	afx_msg void OnValleysUp();
	//�·�ҳ
	afx_msg void OnValleysDown();
	//����Ա����
	afx_msg void OpenAdminWnd();
	//���д��
	afx_msg void OnBankStorage();
	//����ȡ��
	afx_msg void OnBankDraw();
	//��ע��Ϣ
	afx_msg void OnJettonInfo();

	afx_msg void OnState();

	//��Ϣӳ��
protected:
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	//��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�����Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//�����Ϣ
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);	
	//�����Ϣ
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	

	//���
	afx_msg void OnMouseMove(UINT nFlags, CPoint wndPoint);
	//�����Ϣ
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
