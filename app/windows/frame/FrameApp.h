#ifndef FRAME_APP_H_
#define FRAME_APP_H_

#pragma once


class CDlgRoot;
class CFrameApp :public CWinApp
{
protected:
	CDlgRoot		m_DlgRoot;
public:
	CFrameApp() {}

	// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual bool FreeResource();

	// 实现

public:
	//进程目录
	bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);

protected:
	bool AppNotStartedWithSamePath(I32& nPidExists);			//统一路径只能有一个App启动
	static boost::atomic_bool m_bFreeResource;
	DECLARE_MESSAGE_MAP()
};

extern CFrameApp theApp;

#endif

