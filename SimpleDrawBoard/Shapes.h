// Shapes.h
#pragma once
#include <afxwin.h>
#include <memory> // 必须包含，用于智能指针

// 图形类型枚举
enum class ShapeType {
    Line,
    Rectangle,
    Ellipse // 预留
};

// ==========================================
// 1. IShape 接口 (抽象基类)
// ==========================================
class IShape {
public:
    virtual ~IShape() = default;

    // 纯虚函数：每个图形都要知道怎么画自己
    virtual void Draw(CDC* pDC) = 0;

    // 设置坐标
    void SetStart(CPoint pt) { m_ptStart = pt; }
    void SetEnd(CPoint pt) { m_ptEnd = pt; }

    // 获取类型
    virtual ShapeType GetType() const = 0;

protected:
    CPoint m_ptStart;
    CPoint m_ptEnd;
    COLORREF m_color = RGB(0, 0, 0); // 默认黑色
};

// ==========================================
// 2. 具体图形类：直线 (CLine)
// ==========================================
class CLine : public IShape {
public:
    void Draw(CDC* pDC) override {
        // 简单绘制，暂不处理颜色（使用当前 DC 的画笔）
        pDC->MoveTo(m_ptStart);
        pDC->LineTo(m_ptEnd);
    }

    ShapeType GetType() const override { return ShapeType::Line; }
};

// ==========================================
// 3. 具体图形类：矩形 (CRectangle)
// ==========================================
class CRectangle : public IShape {
public:
    void Draw(CDC* pDC) override {
        // 绘制矩形 (使用空画刷防止覆盖背景)
        CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(CRect(m_ptStart, m_ptEnd));
        pDC->SelectObject(pOldBrush);
    }

    ShapeType GetType() const override { return ShapeType::Rectangle; }
};