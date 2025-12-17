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
#include "ShapeFactory.h" // 【记得引入工厂头文件】
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
	ON_COMMAND(ID_TOOL_ELLIPSE, &CSimpleDrawBoardDoc::OnToolEllipse)
	ON_UPDATE_COMMAND_UI(ID_TOOL_ELLIPSE, &CSimpleDrawBoardDoc::OnUpdateToolEllipse)
	ON_COMMAND(ID_TOOL_COLOR, &CSimpleDrawBoardDoc::OnToolColor)
	ON_COMMAND(ID_WIDTH_1, &CSimpleDrawBoardDoc::OnWidth1)
	ON_UPDATE_COMMAND_UI(ID_WIDTH_1, &CSimpleDrawBoardDoc::OnUpdateWidth1)
	ON_COMMAND(ID_WIDTH_2, &CSimpleDrawBoardDoc::OnWidth2)
	ON_UPDATE_COMMAND_UI(ID_WIDTH_2, &CSimpleDrawBoardDoc::OnUpdateWidth2)
	ON_COMMAND(ID_WIDTH_4, &CSimpleDrawBoardDoc::OnWidth4)
	ON_UPDATE_COMMAND_UI(ID_WIDTH_4, &CSimpleDrawBoardDoc::OnUpdateWidth4)
	ON_COMMAND(ID_WIDTH_8, &CSimpleDrawBoardDoc::OnWidth8)
	ON_UPDATE_COMMAND_UI(ID_WIDTH_8, &CSimpleDrawBoardDoc::OnUpdateWidth8)
	ON_COMMAND(ID_TOOL_FREEHAND, &CSimpleDrawBoardDoc::OnToolFreehand)
	ON_UPDATE_COMMAND_UI(ID_TOOL_FREEHAND, &CSimpleDrawBoardDoc::OnUpdateToolFreehand)
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
	// ▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
	// 【核心修复】手动清空所有数据
	// ▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼

	// 1. 清空图形列表
	m_shapeList.clear();

	// 2. 清空撤销/重做栈 (防止新建后还能撤销回上个文件的内容)
	while (!m_undoStack.empty()) m_undoStack.pop();
	while (!m_redoStack.empty()) m_redoStack.pop();

	// 3. (可选) 重置画笔属性为默认值
	m_currentColor = RGB(0, 0, 0); // 恢复黑色
	m_currentWidth = 1; // 恢复 1px
	m_currentShapeType = ShapeType::Line; // 恢复直线工具

	// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
	return TRUE;
}


// CSimpleDrawBoardDoc 序列化

void CSimpleDrawBoardDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
		// === 保存逻辑 ===

		// 1. 写入图形总数
		int count = static_cast<int>(m_shapeList.size());
		ar << count;

		// 2. 遍历保存每个图形
		for (const auto& shape : m_shapeList)
		{
			// 2.1 先写入类型 ID (关键！否则读取时不知道创建哪个类)
			ar << static_cast<int>(shape->GetType());

			// 2.2 调用图形自己的保存逻辑
			shape->Serialize(ar);
		}
	}
	else
	{
		// TODO: 在此添加加载代码
		// === 读取逻辑 ===

		// 0. 清空现有数据
		m_shapeList.clear();
		// 清空撤销栈 (防止读取新文件后还能撤销回旧文件的状态)
		while (!m_undoStack.empty()) m_undoStack.pop();
		while (!m_redoStack.empty()) m_redoStack.pop();

		// 1. 读取总数
		int count = 0;
		ar >> count;

		// 2. 循环读取
		for (int i = 0; i < count; i++)
		{
			int typeInt;
			ar >> typeInt;

			// 2.1 使用工厂根据 ID 创建空对象
			auto shape = ShapeFactory::CreateShape(static_cast<ShapeType>(typeInt));

			if (shape)
			{
				// 2.2 让对象自己读取数据
				shape->Serialize(ar);

				// 2.3 加入列表
				m_shapeList.push_back(shape);
			}
		}

		// 刷新视图
		UpdateAllViews(nullptr);
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

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
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
		CMFCFilterChunkValueImpl* pChunk = nullptr;
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

void CSimpleDrawBoardDoc::OnToolEllipse()
{
	// TODO: 在此添加命令处理程序代码
	m_currentShapeType = ShapeType::Ellipse;
}

void CSimpleDrawBoardDoc::OnUpdateToolEllipse(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentShapeType == ShapeType::Ellipse);
}

void CSimpleDrawBoardDoc::OnToolColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog dlg(m_currentColor); // 默认选中当前色
	if (dlg.DoModal() == IDOK)
	{
		m_currentColor = dlg.GetColor(); // 保存用户选的颜色
	}
}

void CSimpleDrawBoardDoc::OnWidth1()
{
	// TODO: 在此添加命令处理程序代码
	m_currentWidth = 1;
}

void CSimpleDrawBoardDoc::OnUpdateWidth1(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentWidth == 1);
}

void CSimpleDrawBoardDoc::OnWidth2()
{
	// TODO: 在此添加命令处理程序代码
	m_currentWidth = 2;
}

void CSimpleDrawBoardDoc::OnUpdateWidth2(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentWidth == 2);
}

void CSimpleDrawBoardDoc::OnWidth4()
{
	// TODO: 在此添加命令处理程序代码
	m_currentWidth = 4;
}

void CSimpleDrawBoardDoc::OnUpdateWidth4(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentWidth == 4);
}

void CSimpleDrawBoardDoc::OnWidth8()
{
	// TODO: 在此添加命令处理程序代码
	m_currentWidth = 8;
}

void CSimpleDrawBoardDoc::OnUpdateWidth8(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentWidth == 8);
}

void CSimpleDrawBoardDoc::OnToolFreehand()
{
	// TODO: 在此添加命令处理程序代码
	m_currentShapeType = ShapeType::Freehand;
}

void CSimpleDrawBoardDoc::OnUpdateToolFreehand(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_currentShapeType == ShapeType::Freehand);
}
