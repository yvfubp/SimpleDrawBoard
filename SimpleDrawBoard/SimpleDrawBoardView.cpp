
// SimpleDrawBoardView.cpp: CSimpleDrawBoardView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SimpleDrawBoard.h"
#endif

#include "SimpleDrawBoardDoc.h"
#include "SimpleDrawBoardView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpleDrawBoardView

IMPLEMENT_DYNCREATE(CSimpleDrawBoardView, CView)

BEGIN_MESSAGE_MAP(CSimpleDrawBoardView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSimpleDrawBoardView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CSimpleDrawBoardView 构造/析构

CSimpleDrawBoardView::CSimpleDrawBoardView() noexcept
{
	// TODO: 在此处添加构造代码

}

CSimpleDrawBoardView::~CSimpleDrawBoardView()
{
}

BOOL CSimpleDrawBoardView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CSimpleDrawBoardView 绘图

void CSimpleDrawBoardView::OnDraw(CDC* /*pDC*/)
{
	CSimpleDrawBoardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CSimpleDrawBoardView 打印


void CSimpleDrawBoardView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSimpleDrawBoardView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CSimpleDrawBoardView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CSimpleDrawBoardView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CSimpleDrawBoardView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSimpleDrawBoardView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSimpleDrawBoardView 诊断

#ifdef _DEBUG
void CSimpleDrawBoardView::AssertValid() const
{
	CView::AssertValid();
}

void CSimpleDrawBoardView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSimpleDrawBoardDoc* CSimpleDrawBoardView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSimpleDrawBoardDoc)));
	return (CSimpleDrawBoardDoc*)m_pDocument;
}
#endif //_DEBUG


// CSimpleDrawBoardView 消息处理程序
