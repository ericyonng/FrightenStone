#ifndef DLG_ROOT_H_
#define	DLG_ROOT_H_
#pragma once

#include"resource.h"
#include "CRichEditEx.h"
class CThreadPool;
class CDlgRoot :public CDialog
{
public:
	static CDlgRoot &GetInstance() { ASSERT(m_pInstance); return *m_pInstance; }

	CDlgRoot(CWnd* pParent = NULL);
	virtual ~CDlgRoot() {}
	enum { IDD = IDD_DIALOG1 };
	//模态窗口
	virtual INT_PTR DoModal();

	//配置文件
public:
	//绘画进度
	void DrawProgress(long lCurrentCount, long lTotalNum);
	//记录
	void Display(std::string str, bool bNeedPreSym=true);
	//记录
	void DisplayTitle(std::string strFun, std::string str);
	//测试按钮
	void SetTestButton(bool bEnable);

	//对象
public:
	//线程池
	void DestroyPool();

	//消息相应
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

protected:
	HICON		m_hIcon = NULL;
	CRichEditEx  m_Display;
	CFont		m_TextFont;
	//tq::CTimer m_tmPrintStatisticMsg;

public:
	CButton		m_btnEnvBuild;
	CButton		m_btnCancel;
	gtool::CSmartPtr<CThreadPool> m_pThreadPool;

protected:
	static CDlgRoot *m_pInstance;

public:

	afx_msg void OnBnClickedClose();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtBroadcast();
	afx_msg void OnBnFiniSvr();
};

#undef MainDlg2
#define MainDlg2	(CDlgRoot::GetInstance())

//窗口打印字符串
#undef TestDisPlay
#define TestDisPlay(x)	(MainDlg2.Display(x))

//窗口打印字符串带函数
#undef TestDisPlayTitle
#define TestDisPlayTitle(x)	(MainDlg2.DisplayTitle(__FUNCTION__, x))

#undef TestBtnEnable
#define TestBtnEnable(x)	(MainDlg2.SetTestButton(x))

extern void DlgDisplay(std::string str, bool bNeedPreSym);


#endif // !CODE_TEST_WND_H_

