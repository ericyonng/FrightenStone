#include"StdAfx.h"
#include"DlgRoot.h"
#include "CRichEditEx.h"
#include<AppRoot.h>
#include <FrameApp.h>


/////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgRoot, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL, &CDlgRoot::OnBnClickedClose)

	ON_BN_CLICKED(IDC_BT_BROADCAST, &CDlgRoot::OnBnClickedBtBroadcast)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgRoot::OnBnFiniSvr)
END_MESSAGE_MAP()
CDlgRoot*CDlgRoot::m_pInstance = nullptr;

CDlgRoot::CDlgRoot(CWnd* pParent)
	: CDialog(CDlgRoot::IDD, pParent)
{
	//m_hIcon=AfxGetApp()->LoadIcon(IDI_ICON1);
	m_pInstance = this;
}


void CDlgRoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	//LoadConfig();
	//	DDX_Control(pDX, IDC_EDIT3, m_EditFile);
	DDX_Control(pDX, IDC_SYSTEM_STATUS_EDIT, m_Display);
	//}}AFX_DATA_MAP
}
BOOL CDlgRoot::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1));
	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon
	SetWindowText(TEXT("Lab-WinApp"));

	m_Display.SetReadOnly(false);
	m_Display.SetBackColor(MSG_COLOR_GRAY);
	m_Display.SetEditDefFormat();
	m_Display.SetMaxLines(1000000);	//100万条
	
	m_TextFont.CreateFont(12, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("宋体"));	
	
	IF_NOT(AppRootObj.Init())
	{
		ASSERT(AppRootObj.Fini());
		MessageBox("服务器启动失败！");
		EndDialog(-1);
		return true;
	}
	m_pThreadPool = CThreadPool::CreateNew(1, 1);
	CHECKF(m_pThreadPool);

	BUFFER1024 szTitle = { 0 };
	BUFFER128 szAddr = { 0 };
	gtool::NetWorkModulGetSvrBindAddr(szAddr, sizeof(szAddr));

	//工作目录
	char szDirectory[MAX_PATH] = TEXT("");
	theApp.GetWorkDirectory(szDirectory, CountArray(szDirectory));
	_sntprintf(szTitle, sizeof(szTitle) - 1, "Lab-WinApp@%s@%s", szAddr, szDirectory);
	SetWindowText(TEXT(szTitle));

	Display("服务器启动成功.");
	

	return true;  // return true  unless you set the focus to a control
}


void CDlgRoot::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CDlgRoot::OnBnClickedClose()
{
// 	auto bOk = MessageBox("确定关闭服务器？", "系统提示", MB_YESNO);
// 	if (IDYES == bOk || IDOK == bOk)
// 	{
// 		AppRootObj.Fini();
// 		CDialog::OnCancel();
// 	}
	CDialog::OnCancel();
}

//模态窗口
INT_PTR CDlgRoot::DoModal()
{
	return CDialog::DoModal();
}

//绘画进度
void CDlgRoot::DrawProgress(long lCurrentCount, long lTotalNum)
{
	CClientDC dc(this);
	CDC *pDC = CDC::FromHandle(dc.GetSafeHdc());
	HGDIOBJ pOld = pDC->SelectObject(m_TextFont);
	CRect rcClient, rcType, rcFileText, rcProgress, rcRange;

	GetClientRect(&rcClient);
	rcType.SetRect(rcClient.left + 40 - 4, rcClient.top + 4 + 206 + 12, rcClient.right - 40 - 4, rcClient.top + 24 + 206 + 12);
	rcFileText.SetRect(rcType.left, rcType.bottom + 4 - 5, rcType.right, rcType.bottom + 24 - 5);
	rcProgress.SetRect(rcFileText.left, rcFileText.bottom + 4 - 10, rcFileText.right, rcFileText.bottom + 24 - 10);
	rcRange.SetRect(rcType.left, rcType.top, rcType.right, rcProgress.bottom + 14);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 0, 0));
	pDC->FillSolidRect(&rcRange, RGB(0xf0, 0xf0, 0xf0));

	//类型
	TCHAR szString[1024] = { 0 };

	//进度
	memset(szString, 0, sizeof(szString));
	double lfProgress = (double)lCurrentCount / ((double)lTotalNum);
	_sntprintf(szString, CountArray(szString), TEXT("进度:%.3lf%% 总数:%ld"), lfProgress * 100, lTotalNum);
	rcProgress.SetRect(rcFileText.left, rcFileText.bottom, rcFileText.right, rcFileText.bottom + 20);
	pDC->DrawText(szString, lstrlen(szString), &rcProgress, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
	rcProgress.OffsetRect(0, 20);
	rcProgress.bottom = rcProgress.top + 14;
	pDC->FillSolidRect(&rcProgress, RGB(255, 255, 255));
	rcProgress.SetRect(rcFileText.left, rcFileText.bottom + 20, fabs(rcFileText.Width() *lfProgress) + rcFileText.left, rcFileText.bottom + 40);
	rcProgress.bottom = rcProgress.top + 14;
	pDC->FillSolidRect(&rcProgress, RGB(0, 255, 0));

	pDC->SelectObject(pOld);
}

void CDlgRoot::Display(std::string str, bool bNeedPreSym)
{
	str += "\n";
	if (bNeedPreSym) str = "* " + str;

	m_Display.InsertString(str.c_str());
	m_Display.UpdateWindow();
}

void CDlgRoot::DisplayTitle(std::string strFun, std::string str)
{
	Display("\n"+str+" "+strFun, false);
}

void CDlgRoot::SetTestButton(bool bEnable)
{
	m_btnEnvBuild.EnableWindow(bEnable);
}


void CDlgRoot::DestroyPool()
{
	CHECK(m_pThreadPool);
	m_pThreadPool->Destroy();
}

void CDlgRoot::OnBnClickedBtBroadcast()
{
	// TODO: Add your control notification handler code here
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_BROADCAST);
	CHECK(pEdit);

	std::string strData;
	BUFFER512 szBuffer = { 0 };
	auto nByte = pEdit->GetWindowText(szBuffer, sizeof(szBuffer));
	strData += szBuffer;
	szBuffer[0] = 0;
	sprintf(szBuffer, "字节：[%d] Byte", nByte);
	strData += szBuffer;
	TestDisPlay(strData);

	auto pBusinessThread = GetBusinessLogicMainThread();
	I32 n = 10000;
	while ((n--)>0)
	{
		IF_OK(pBusinessThread)
		{
			ASSERTNET(THREAD_TRANSFER::Post2Thread(pBusinessThread, POST_OBJ_TYPE_MAIN_THREAD, (U64)0, 0, 0));
		}
	}

	//向客户端发送数据
	//BusinessLogicBroadcast(szBuffer, nByte);
}

class CFiniTask : public CTaskBase
{
public:
	//任务执行体
	int Run()
	{
		//释放
		CDlgRoot* pDlg = (CDlgRoot*)m_pArg;
		CHECKF(pDlg);

		AppRootObj.Fini();
		pDlg->PostMessage(WM_COMMAND, IDCANCEL);

		return 0;
	}
};




void CDlgRoot::OnBnFiniSvr()
{
	auto bOk = MessageBox("确定关闭服务器？", "系统提示", MB_YESNO);
	if (IDYES == bOk || IDOK == bOk)
	{
		gtool::CSmartPtr<CTaskBase> pFiniTask = new CFiniTask;
		CHECK(pFiniTask);
		pFiniTask->SetArg(this);
		CTaskBase *pTask = pFiniTask;
		CHECK(m_pThreadPool->AddTask(*pTask));
		pFiniTask.pop();

// 		AppRootObj.Fini();
// 
// 		PostMessage(WM_COMMAND, IDCANCEL);
	}	
	// TODO: Add your control notification handler code here
}

void DlgDisplay(std::string str, bool bNeedPreSym)
{
	TestDisPlay(str);
}