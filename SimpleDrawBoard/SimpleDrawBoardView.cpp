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
#include "CGdiObjectWrapper.h"
#include "ShapeFactory.h"
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
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_EDIT_UNDO, &CSimpleDrawBoardView::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CSimpleDrawBoardView::OnEditRedo)
	//	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CSimpleDrawBoardView::OnUpdateEditRedo)
END_MESSAGE_MAP()

// CSimpleDrawBoardView 构造/析构

CSimpleDrawBoardView::CSimpleDrawBoardView() noexcept
{
	// TODO: 在此处添加构造代码
	// 【新增】初始化变量
	m_bDrawing = false;
	m_ptStart = CPoint(0, 0);
	m_ptEnd = CPoint(0, 0);
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

void CSimpleDrawBoardView::OnDraw(CDC* pDC)
{
	CSimpleDrawBoardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
	// ==================================================
	// 分支 A: 打印模式 (直接绘制到打印机 DC)
	// ==================================================
	if (pDC->IsPrinting())
	{
		// 打印机不需要双缓冲 (而且打印机分辨率很高，创建兼容位图会消耗巨大内存)
		// 直接遍历列表绘制即可
		for (const auto& shape : pDoc->m_shapeList) {
			shape->Draw(pDC);
		}
		return; // 打印完成，直接返回
	}
	// ==================================================
	// 分支 B: 屏幕模式 (使用双缓冲防止闪烁)
	// ==================================================
	// --- 双缓冲技术 (Double Buffering) 开始 ---
	// 目的：防止绘图闪烁，对应性能要求 NF-02

	// 1. 获取窗口客户区大小
	CRect rect;
	GetClientRect(&rect);

	// 2. 创建内存 DC (画布)
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	// 3. 创建兼容位图 (纸张) 并选入内存 DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// 4. 填充背景为白色
	memDC.FillSolidRect(&rect, RGB(255, 255, 255));

	// ==========================================
	// 绘图区域：所有的绘制操作都在 memDC 上进行
	// ==========================================

	// A. (未来阶段) 这里遍历 pDoc->m_shapeList 绘制历史图形
	// A. 绘制历史图形 (遍历 Document 中的列表)
	// ---------------------------------------------
	// 这里使用 RAII 画笔/画刷，防止资源泄漏
	// 1. 先创建包装器
	//CPenWrapper pen;
	//// 2. 再创建画笔资源
	//pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	//CPen* pOldPen = memDC.SelectObject(pen.Get());

	for (const auto& shape : pDoc->m_shapeList)
	{
		shape->Draw(&memDC); // 多态调用：直线画直线，矩形画矩形
	}

	//memDC.SelectObject(pOldPen);
	// ---------------------------------------------
	// B. 绘制当前正在拖拽的临时线条 (橡皮筋效果)
	if (m_bDrawing && m_pTempShape)
	{
		// 同样需要设置画笔
		/*CPenWrapper pen;
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen* pOldPen = memDC.SelectObject(pen.Get());*/

		m_pTempShape->Draw(&memDC); // 画临时的

		//memDC.SelectObject(pOldPen);
	}
	//if (m_bDrawing)
	//{
	//	// 使用你的 RAII 包装器创建黑色实线画笔
	//	/*CPenWrapper pen(PS_SOLID, 1, RGB(0, 0, 0));

	//	CPen* pOldPen = memDC.SelectObject(pen.Get());*/

	//	// 1. 先创建包装器对象
	//	CPenWrapper pen;

	//	// 2. 再调用 CreatePen 创建画笔资源
	//	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	//	// 3. 选入设备上下文
	//	CPen* pOldPen = memDC.SelectObject(pen.Get());

	//	memDC.MoveTo(m_ptStart);
	//	memDC.LineTo(m_ptEnd);

	//	memDC.SelectObject(pOldPen); // 恢复旧画笔
	//}

	// ==========================================

	// 5. 将内存 DC 的内容一次性拷贝到屏幕 (BitBlt)
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	// 6. 清理资源 (CBitmap 等对象析构自动释放，但选入对象需还原)
	memDC.SelectObject(pOldBitmap);
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
	return static_cast<CSimpleDrawBoardDoc*>(m_pDocument);
}
#endif //_DEBUG


// CSimpleDrawBoardView 消息处理程序

BOOL CSimpleDrawBoardView::OnEraseBkgnd(CDC* pDC)
{
	// return CView::OnEraseBkgnd(pDC);  <-- 删除或注释掉这一行
	return TRUE; // 【修改】告诉系统：背景我已经自己画了，你别动，否则会闪
}

void CSimpleDrawBoardView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bDrawing = true; // 标记开始绘制
	m_ptStart = point; // 记录起点
	m_ptEnd = point; // 初始终点等于起点
	SetCapture(); // 【关键】捕获鼠标，防止拖出窗口外丢失状态
	// 1. 获取文档
	CSimpleDrawBoardDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	// 1. 创建临时对象，但暂时【不】放入 pDoc->m_shapeList
	m_pTempShape = ShapeFactory::CreateShape(pDoc->m_currentShapeType);
	// 2. 使用工厂创建当前选中的图形类型 [关键点：Factory 模式应用]
	//auto newShape = ShapeFactory::CreateShape(pDoc->m_currentShapeType);

	// 3. 设置初始坐标
	if (m_pTempShape)
	{
		m_pTempShape->SetStart(point);
		// m_pTempShape->SetEnd(point);
		// ▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
		// 【新增】注入属性：将文档的当前状态传给新图形
		m_pTempShape->SetColor(pDoc->m_currentColor);
		m_pTempShape->SetWidth(pDoc->m_currentWidth);
		// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
	}
	CView::OnLButtonDown(nFlags, point);
}

void CSimpleDrawBoardView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDrawing)
	{
		m_ptEnd = point; // 更新终点
		// 更新临时对象
		if (m_pTempShape)
		{
			// ❌ 删除旧代码: m_pTempShape->SetEnd(point);

			// ✅ 使用多态接口: 
			// 如果是直线，它会自动更新终点; 
			// 如果是曲线，它会自动 push_back 新点。
			m_pTempShape->OnDrag(point);
		}
		Invalidate(FALSE); // 【关键】触发 OnDraw 重绘。FALSE 表示不擦除背景(配合步骤2.5)
	}
	CView::OnMouseMove(nFlags, point);
}

void CSimpleDrawBoardView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDrawing)
	{
		m_bDrawing = false;
		m_ptEnd = point;
		ReleaseCapture(); // 释放鼠标捕获
		// 提交命令
		if (m_pTempShape)
		{
			// 再次更新一下终点，确保准确
			// m_pTempShape->SetEnd(point);
			CSimpleDrawBoardDoc* pDoc = GetDocument();

			// 创建一个命令对象
			auto cmd = std::make_shared<AddShapeCommand>(pDoc->m_shapeList, m_pTempShape);

			// 提交给 Doc 执行 (Execute 会自动把它加进 List)
			pDoc->CommitOperation(cmd);

			// 清空临时指针
			m_pTempShape = nullptr;
		}
		// 目前松开鼠标后线会消失，因为还没存入 Document
		// 这是正常的，我们将在下一阶段实现存储
		Invalidate(FALSE);
	}
	CView::OnLButtonUp(nFlags, point);
}

void CSimpleDrawBoardView::OnEditUndo()
{
	// TODO: 在此添加命令处理程序代码
	GetDocument()->OnUndo();
}

void CSimpleDrawBoardView::OnEditRedo()
{
	// TODO: 在此添加命令处理程序代码
	// 调用 Doc 里的 OnRedo 方法 (你之前写好的)
	GetDocument()->OnRedo();
}
