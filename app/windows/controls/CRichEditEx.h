

#ifndef CRICH_EDIT_EX_H_
#define CRICH_EDIT_EX_H_

#pragma once


#define ID_RICH_UNDO		201
#define ID_RICH_CUT			202
#define ID_RICH_COPY		203
#define ID_RICH_PASTE		204
#define ID_RICH_CLEAR		205
#define ID_RICH_SELECTALL	206
#define ID_RICH_SETFONT		207

#define MSG_COLOR_RED		RGB(0xff,0x00,0x00)
#define MSG_COLOR_BLUE		RGB(0x0c,0x01,0x80)
#define MSG_COLOR_GREEN		RGB(0x00,0xcc,0x00)
#define MSG_COLOR_YELLOW	RGB(0xcc,0xdd,0x00)
#define MSG_COLOR_GRAY		RGB(0xee,0xee,0xee)
#define MSG_COLOR_BLACK		RGB(0x00,0x00,0x00)

#if !(defined(linux)|defined(__CYGWIN__))

/////////////////////////////////////////////////////////////////////////////
// CRichEditEx window

//#define CONTROL_STORE_CLASS 

class CRichEditEx : public CRichEditCtrl
{
	// Construction
public:
	CRichEditEx();
	virtual ~CRichEditEx();


	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditEx)
	//}}AFX_VIRTUAL

	//功能函数
public:
	//背景颜色
	void SetBackColor(COLORREF bk_color);
	//插入字符串
	void InsertString(CString strText);
	//插入字符串
	void InsertString(CString strText, COLORREF fn_color, DWORD fn_effects);
	//最大行数设置
	void SetMaxLines(long maxlines);
	//获取最大行数
	long GetMaxLines(){ return m_lMaxLines; }
	void SetEditDefFormat();

	// Generated message map functions
protected:
	//绑定控件
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CRichEditEx)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	//复制
	afx_msg void OnCopy() { Copy(); }		
	//剪切
	afx_msg void OnCut() { Cut(); }	
	//粘贴
	afx_msg void OnPaste() { Paste(); }	
	//全选
	afx_msg void OnSelectall() { SetSel(0, -1); }	
	//撤销
	afx_msg void OnUndo() { Undo(); }
	//清除
	afx_msg void OnClear() { Clear(); }		
	//改变字体
	afx_msg void OnSelectfont();					
	//创建
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//菜单响应
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);

	//数据成员
protected:
	long		m_lMaxLines;		//最大行数
	CHARFORMAT	m_cfCurrentFont;	//当前字体

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXRICHEDIT_H__01784CAD_9A89_481A_AC0E_63E1E5205AB3__INCLUDED_)
