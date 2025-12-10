
// SimpleDrawBoardDoc.h: CSimpleDrawBoardDoc 类的接口
//

#include <vector>
#include <memory>
#include "Shapes.h"
#include <stack>
#include "Commands.h"
#pragma once


class CSimpleDrawBoardDoc : public CDocument
{
protected: // 仅从序列化创建
	CSimpleDrawBoardDoc() noexcept;
	DECLARE_DYNCREATE(CSimpleDrawBoardDoc)

// SimpleDrawBoardDoc.h 类内部
public:
	// 使用 shared_ptr 自动管理内存，无需手动 delete
	std::vector<std::shared_ptr<IShape>> m_shapeList;

	// 当前选中的工具类型（默认直线）
	ShapeType m_currentShapeType = ShapeType::Line;
    // --- 撤销/重做系统 ---
    std::stack<std::shared_ptr<IDrawCommand>> m_undoStack;
    std::stack<std::shared_ptr<IDrawCommand>> m_redoStack;

    // 执行新操作（如画了一个新图）
    void CommitOperation(std::shared_ptr<IDrawCommand> cmd) {
        cmd->Execute();          // 执行
        m_undoStack.push(cmd);   // 进栈

        // 清空重做栈（一旦有新操作，之前的重做历史就失效了）
        while (!m_redoStack.empty()) m_redoStack.pop();

        UpdateAllViews(NULL);    // 通知视图刷新
    }

    // 撤销
    void OnUndo() {
        if (m_undoStack.empty()) return;

        auto cmd = m_undoStack.top();
        m_undoStack.pop();

        cmd->UnExecute();       // 反执行
        m_redoStack.push(cmd);  // 移入重做栈

        UpdateAllViews(NULL);
    }

    // 重做
    void OnRedo() {
        if (m_redoStack.empty()) return;

        auto cmd = m_redoStack.top();
        m_redoStack.pop();

        cmd->Execute();         // 再次执行
        m_undoStack.push(cmd);  // 移回撤销栈

        UpdateAllViews(NULL);
    }
// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CSimpleDrawBoardDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
    afx_msg void OnToolLine();
    afx_msg void OnUpdateToolLine(CCmdUI* pCmdUI);
    afx_msg void OnToolRect();
    afx_msg void OnUpdateToolRect(CCmdUI* pCmdUI);
};
