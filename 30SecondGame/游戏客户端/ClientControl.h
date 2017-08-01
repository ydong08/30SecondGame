#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000

//游戏控制基类
class IClientControlDlg : public CDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

	virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
	//更新库存
	virtual void __cdecl UpDateSystemStorage(CMD_S_SystemStorage * pStorage)=NULL;
};
