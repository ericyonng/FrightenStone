#include "StdAfx.h"
#include "CRichEditEx.h"



//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CRichEditEx
BEGIN_MESSAGE_MAP(CRichEditEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditEx)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP

	ON_WM_CONTEXTMENU()	//菜单消息
	ON_WM_CREATE()

	ON_COMMAND(ID_RICH_COPY, OnCopy)
	ON_COMMAND(ID_RICH_CUT, OnCut)
	ON_COMMAND(ID_RICH_PASTE, OnPaste)
	ON_COMMAND(ID_RICH_SELECTALL, OnSelectall)
	ON_COMMAND(ID_RICH_UNDO, OnUndo)
	ON_COMMAND(ID_RICH_CLEAR, OnClear)
	ON_COMMAND(ID_RICH_SETFONT, OnSelectfont)
END_MESSAGE_MAP()

CRichEditEx::CRichEditEx()
{
	m_lMaxLines = 0;
	memset(&m_cfCurrentFont, 0, sizeof(m_cfCurrentFont));
}

CRichEditEx::~CRichEditEx()
{
}




/////////////////////////////////////////////////////////////////////////////
// CRichEditEx message handlers

void CRichEditEx::SetMaxLines(long maxlines)
{
	m_lMaxLines = maxlines;
}

void CRichEditEx::SetEditDefFormat()
{
	//int nTextLength;// = m_cMessage.GetWindowTextLength();

	SetSel(-1, -1);

	CHARFORMAT cf;
	memset((char*)&cf, 0, sizeof(cf));
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_CHARSET | CFM_FACE | CFM_SIZE;
	cf.dwEffects = 0;
	cf.crTextColor = MSG_COLOR_YELLOW;
	cf.bCharSet = GB2312_CHARSET;
	lstrcpy(cf.szFaceName, TEXT("宋体"));
	cf.yHeight = 180;
	memset(&m_cfCurrentFont, 0, sizeof(m_cfCurrentFont));
	memmove(&m_cfCurrentFont, &cf, sizeof(m_cfCurrentFont));

	SetSelectionCharFormat(m_cfCurrentFont);
}

void CRichEditEx::InsertString(CString Msg, COLORREF fn_color, DWORD fn_effects)
{
	int nTextLength;// = m_cMessage.GetWindowTextLength();

	SetSel(-1, -1);

	CHARFORMAT cf;
	memset((char*)&cf, 0, sizeof(cf));
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_CHARSET | CFM_FACE | CFM_SIZE;
	cf.dwEffects = fn_effects;
	cf.crTextColor = fn_color;
	cf.bCharSet = GB2312_CHARSET;
	lstrcpy(cf.szFaceName, TEXT("宋体"));
	cf.yHeight = 180;
	memset(&m_cfCurrentFont, 0, sizeof(m_cfCurrentFont));
	memmove(&m_cfCurrentFont, &cf, sizeof(m_cfCurrentFont));

	SetSelectionCharFormat(m_cfCurrentFont);

	ReplaceSel(Msg);

	int TotalLines = GetLineCount();
	if (m_lMaxLines > 0) {
		if (TotalLines > m_lMaxLines)
		{
			//SetReadOnly(false);
			nTextLength = LineIndex(TotalLines - m_lMaxLines);		//清除前几行
			SetSel(0, nTextLength);
			Clear();
			//SetReadOnly(true);
		}
	}

	PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CRichEditEx::SetBackColor(COLORREF bk_color)
{
	//SetReadOnly(false);
	SetBackgroundColor(false, bk_color);
	//SetReadOnly(true);
}

void CRichEditEx::InsertString(CString strText)
{
	int nTextLength;// = m_cMessage.GetWindowTextLength();

	SetSel(-1, -1);

	CHARFORMAT cf;
	memset((char*)&cf, 0, sizeof(cf));
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_CHARSET | CFM_FACE | CFM_SIZE;
	cf.dwEffects = 0;
	cf.crTextColor = MSG_COLOR_BLACK;
	cf.bCharSet = GB2312_CHARSET;
	lstrcpy(cf.szFaceName, TEXT("宋体"));
	cf.yHeight = 180;
	memset(&m_cfCurrentFont, 0, sizeof(m_cfCurrentFont));
	memmove(&m_cfCurrentFont, &cf, sizeof(m_cfCurrentFont));

	SetSelectionCharFormat(m_cfCurrentFont);

	ReplaceSel(strText);

	int TotalLines = GetLineCount();
	if (m_lMaxLines > 0) {
		if (TotalLines > m_lMaxLines)
		{
			//SetReadOnly(false);
			nTextLength = LineIndex(TotalLines - m_lMaxLines);
			SetSel(0, nTextLength);
			Clear();
			//SetReadOnly(true);
		}
	}

	PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CRichEditEx::OnSelectfont()
{
	// TODO: Add your command handler code here
	CHARFORMAT cf;
	LOGFONT lf;
	memset(&cf, 0, sizeof(CHARFORMAT));
	memset(&lf, 0, sizeof(LOGFONT));
	//判断是否选择了内容
	bool bSelect = (GetSelectionType() != SEL_EMPTY) ? true : false;
	if (bSelect)
	{
		GetSelectionCharFormat(cf);
	}
	else
	{
		if (m_cfCurrentFont.dwMask == 0)
		{
			GetDefaultCharFormat(cf);
		}
		else
		{
			memmove(&cf, &m_cfCurrentFont, sizeof(m_cfCurrentFont));
		}
		
		//cf = m_cfCurrentFont;
		//GetDefaultCharFormat(cf);
	}
	//得到相关字体属性
	bool bIsBold = cf.dwEffects & CFE_BOLD;
	bool bIsItalic = cf.dwEffects & CFE_ITALIC;
	bool bIsUnderline = cf.dwEffects & CFE_UNDERLINE;
	bool bIsStrickout = cf.dwEffects & CFE_STRIKEOUT;
	//设置属性
	lf.lfCharSet = cf.bCharSet;
	lf.lfHeight = cf.yHeight / 15;
	lf.lfPitchAndFamily = cf.bPitchAndFamily;
	lf.lfItalic = BYTE(bIsItalic);
	lf.lfWeight = (bIsBold ? FW_BOLD : FW_NORMAL);
	lf.lfUnderline = BYTE( bIsUnderline);
	lf.lfStrikeOut = BYTE( bIsStrickout);
	memset(lf.lfFaceName, 0, sizeof(lf.lfFaceName));
	lstrcpyn(lf.lfFaceName, cf.szFaceName, sizeof(lf.lfFaceName)/sizeof(TCHAR));

	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = cf.crTextColor;
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat(cf);               //获得所选字体的属性
		if (bSelect)
			SetSelectionCharFormat(cf);  //为选定的内容设定所选字体
		else
		{
			SetWordCharFormat(cf);       //为将要输入的内容设定字体
			memset(&m_cfCurrentFont, 0, sizeof(m_cfCurrentFont));
			memmove(&m_cfCurrentFont, &cf, sizeof(cf));
		}
			
	}

}

int CRichEditEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1) return -1;
	SetEventMask(ENM_MOUSEEVENTS);

	SetSel(-1, -1);

	//字体设置
	memset(&m_cfCurrentFont, 0, sizeof(m_cfCurrentFont));
	m_cfCurrentFont.cbSize = sizeof(m_cfCurrentFont);
	m_cfCurrentFont.dwMask = CFM_COLOR | CFM_BOLD | CFM_CHARSET | CFM_FACE | CFM_SIZE;
	m_cfCurrentFont.dwEffects = 0;
	m_cfCurrentFont.crTextColor = MSG_COLOR_YELLOW;
	m_cfCurrentFont.bCharSet = GB2312_CHARSET;
	lstrcpy(m_cfCurrentFont.szFaceName, TEXT("宋体"));
	m_cfCurrentFont.yHeight = 180;
	SetWordCharFormat(m_cfCurrentFont);       //为将要输入的内容设定字体

	return 0;
}

void CRichEditEx::DoDataExchange(CDataExchange* pDX)
{
	CRichEditCtrl::DoDataExchange(pDX);


	//LoadConfig();
	//	DDX_Control(pDX, IDC_EDIT3, m_EditFile);

	//}}AFX_DATA_MAP
}

//响应内容消息
void CRichEditEx::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CRichEditCtrl::OnContextMenu(pWnd, pos);

	//创建一个弹出式菜单
	CMenu popmenu;
	popmenu.CreatePopupMenu();

	//添加菜单项目
	popmenu.AppendMenu(0, ID_RICH_UNDO, TEXT("&撤销"));
	popmenu.AppendMenu(0, MF_SEPARATOR);
	popmenu.AppendMenu(0, ID_RICH_CUT, TEXT("&剪切"));
	popmenu.AppendMenu(0, ID_RICH_COPY, TEXT("&复制"));
	popmenu.AppendMenu(0, ID_RICH_PASTE, TEXT("&粘贴"));
	popmenu.AppendMenu(0, ID_RICH_CLEAR, TEXT("&清除"));
	popmenu.AppendMenu(0, MF_SEPARATOR);
	popmenu.AppendMenu(0, ID_RICH_SELECTALL, TEXT("&选择全部"));
	popmenu.AppendMenu(0, MF_SEPARATOR);
	popmenu.AppendMenu(0, ID_RICH_SETFONT, TEXT("&选择字体"));

	//初始化菜单项
	UINT nUndo = (CanUndo() ? 0 : MF_GRAYED);
	popmenu.EnableMenuItem(ID_RICH_UNDO, MF_BYCOMMAND | nUndo);
	UINT nSel = ((GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
	popmenu.EnableMenuItem(ID_RICH_CUT, MF_BYCOMMAND | nSel);
	popmenu.EnableMenuItem(ID_RICH_COPY, MF_BYCOMMAND | nSel);
	popmenu.EnableMenuItem(ID_RICH_CLEAR, MF_BYCOMMAND | nSel);
	UINT nPaste = (CanPaste() ? 0 : MF_GRAYED);
	popmenu.EnableMenuItem(ID_RICH_PASTE, MF_BYCOMMAND | nPaste);

	//显示菜单
	CPoint pt;
	GetCursorPos(&pt);
	popmenu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	popmenu.DestroyMenu();
}