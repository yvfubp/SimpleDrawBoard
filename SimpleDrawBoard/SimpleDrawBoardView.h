// SimpleDrawBoardView.h: CSimpleDrawBoardView 类的接口
//

#pragma once


class CSimpleDrawBoardView : public CView
{
protected: // 仅从序列化创建
	CSimpleDrawBoardView() noexcept;
	DECLARE_DYNCREATE(CSimpleDrawBoardView)
	// protected 区域
	std::shared_ptr<IShape> m_pTempShape; // 当前正在画但还没松手的图形

protected: // 添加以下成员
	// 绘图交互状态
	bool m_bDrawing; // 鼠标是否按下
	CPoint m_ptStart; // 起点
	CPoint m_ptEnd; // 当前终点（鼠标移动时实时更新）
	// 特性
public:
	CSimpleDrawBoardDoc* GetDocument() const;

	// 操作

	// 重写
	void OnDraw(CDC* pDC) override; // 重写以绘制该视图
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

protected:
	BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;

	// 实现
public:
	~CSimpleDrawBoardView() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// 生成的消息映射函数
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	//	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // SimpleDrawBoardView.cpp 中的调试版本
inline CSimpleDrawBoardDoc* CSimpleDrawBoardView::GetDocument() const
{
	return reinterpret_cast<CSimpleDrawBoardDoc*>(m_pDocument);
}
#endif
