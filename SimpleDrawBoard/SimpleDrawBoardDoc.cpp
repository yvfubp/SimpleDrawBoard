
// SimpleDrawBoardDoc.cpp: CSimpleDrawBoardDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SimpleDrawBoard.h"
#endif

#include "SimpleDrawBoardDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSimpleDrawBoardDoc

IMPLEMENT_DYNCREATE(CSimpleDrawBoardDoc, CDocument)

BEGIN_MESSAGE_MAP(CSimpleDrawBoardDoc, CDocument)
	ON_COMMAND(ID_TOOL_LINE, &CSimpleDrawBoardDoc::OnToolLine)
	ON_UPDATE_COMMAND_UI(ID_TOOL_LINE, &CSimpleDrawBoardDoc::OnUpdateToolLine)
	ON_COMMAND(ID_TOOL_RECT, &CSimpleDrawBoardDoc::OnToolRect)
	ON_UPDATE_COMMAND_UI(ID_TOOL_RECT, &CSimpleDrawBoardDoc::OnUpdateToolRect)
END_MESSAGE_MAP()


// CSimpleDrawBoardDoc 构造/析构

CSimpleDrawBoardDoc::CSimpleDrawBoardDoc() noexcept
{
	// TODO: 在此添加一次性构造代码

}

CSimpleDrawBoardDoc::~CSimpleDrawBoardDoc()
{
}

BOOL CSimpleDrawBoardDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CSimpleDrawBoardDoc 序列化

void CSimpleDrawBoardDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CSimpleDrawBoardDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CSimpleDrawBoardDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CSimpleDrawBoardDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSimpleDrawBoardDoc 诊断

#ifdef _DEBUG
void CSimpleDrawBoardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSimpleDrawBoardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSimpleDrawBoardDoc 命令

void CSimpleDrawBoardDoc::OnToolLine()
{
	// TODO: 在此添加命令处理程序代码
	m_currentShapeType = ShapeType::Line; // 切换状态
}

void CSimpleDrawBoardDoc::OnUpdateToolLine(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	// 如果当前是 Line，就设为 SetCheck(TRUE) 打钩
	pCmdUI->SetCheck(m_currentShapeType == ShapeType::Line);
}

void CSimpleDrawBoardDoc::OnToolRect()
{
	// TODO: 在此添加命令处理程序代码
	m_currentShapeType = ShapeType::Rectangle;
}

void CSimpleDrawBoardDoc::OnUpdateToolRect(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentShapeType == ShapeType::Rectangle);
}
