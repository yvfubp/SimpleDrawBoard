// Shapes.h

#pragma once
#include <afxwin.h>
#include <memory>
#include "CGdiObjectWrapper.h" // 【必须引入这个】
#include <vector>

enum class ShapeType { Line, Rectangle, Ellipse, Freehand };

// 1. 修改 IShape 基类
class IShape
{
public:
	virtual ~IShape() = default;
	virtual void Draw(CDC* pDC) = 0;
	virtual ShapeType GetType() const = 0;
	// 【新增】序列化接口
	virtual void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// 保存模式：写入数据
			ar << m_ptStart << m_ptEnd << m_color << m_nWidth;
		}
		else
		{
			// 读取模式：读出数据
			ar >> m_ptStart >> m_ptEnd >> m_color >> m_nWidth;
		}
	}

	/*void SetStart(CPoint pt) { m_ptStart = pt; }
	void SetEnd(CPoint pt) { m_ptEnd = pt; }*/

	// 【新增】设置颜色和线宽
	// 通用属性设置
	void SetColor(COLORREF c) { m_color = c; }
	void SetWidth(int w) { m_nWidth = w; }

	// 设置起点
	virtual void SetStart(CPoint pt)
	{
		m_ptStart = pt;
		m_ptEnd = pt;
	}

	// 【核心修改】处理鼠标拖拽
	// 默认行为（直线/矩形）：只更新终点
	virtual void OnDrag(CPoint pt)
	{
		m_ptEnd = pt;
	}

protected:
	CPoint m_ptStart;
	CPoint m_ptEnd;

	// 【新增】属性变量
	COLORREF m_color = RGB(0, 0, 0); // 默认黑色
	int m_nWidth = 1; // 默认 1px
};

// 2. 修改 CLine (直线)
class CLine : public IShape
{
public:
	void Draw(CDC* pDC) override
	{
		// 使用 RAII 包装器创建指定颜色和宽度的画笔
		CPenWrapper pen;
		pen.CreatePen(PS_SOLID, m_nWidth, m_color); // 使用自己的属性

		CPen* pOldPen = pDC->SelectObject(pen.Get());

		pDC->MoveTo(m_ptStart);
		pDC->LineTo(m_ptEnd);

		pDC->SelectObject(pOldPen); // 恢复旧画笔
	}

	ShapeType GetType() const override { return ShapeType::Line; }
};

// 3. 修改 CRectangle (矩形)
class CRectangle : public IShape
{
public:
	void Draw(CDC* pDC) override
	{
		CPenWrapper pen;
		pen.CreatePen(PS_SOLID, m_nWidth, m_color);
		CPen* pOldPen = pDC->SelectObject(pen.Get());

		// 使用透明画刷 (只画边框，不填充中间)
		auto pOldBrush = static_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH));

		pDC->Rectangle(CRect(m_ptStart, m_ptEnd));

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	ShapeType GetType() const override { return ShapeType::Rectangle; }
};

// 4. 修改 CEllipse (椭圆)
class CEllipse : public IShape
{
public:
	void Draw(CDC* pDC) override
	{
		CPenWrapper pen;
		pen.CreatePen(PS_SOLID, m_nWidth, m_color);
		CPen* pOldPen = pDC->SelectObject(pen.Get());

		auto pOldBrush = static_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH));

		pDC->Ellipse(CRect(m_ptStart, m_ptEnd));

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	ShapeType GetType() const override { return ShapeType::Ellipse; }
};

// 3. 【新增】自由曲线类
class CFreehand : public IShape
{
	std::vector<CPoint> m_points; // 存储轨迹上所有的点

public:
	// 重写设置起点：起点也是第一个轨迹点
	void SetStart(CPoint pt) override
	{
		IShape::SetStart(pt);
		m_points.clear();
		m_points.push_back(pt);
	}

	// 重写拖拽逻辑：不是更新终点，而是追加新点
	void OnDrag(CPoint pt) override
	{
		// 简单的去重（防止鼠标不动时添加大量重复点）
		if (m_points.empty() || m_points.back() != pt)
		{
			m_points.push_back(pt);
		}
	}

	void Draw(CDC* pDC) override
	{
		if (m_points.size() < 2) return; // 点太少画不出来

		CPenWrapper pen;
		pen.CreatePen(PS_SOLID, m_nWidth, m_color);
		CPen* pOldPen = pDC->SelectObject(pen.Get());

		// 使用 Polyline 高效绘制连续折线
		pDC->Polyline(m_points.data(), static_cast<int>(m_points.size()));

		pDC->SelectObject(pOldPen);
	}

	// 【新增】重写序列化
	void Serialize(CArchive& ar) override
	{
		// 1. 先调用基类保存颜色、线宽等
		IShape::Serialize(ar);

		if (ar.IsStoring())
		{
			// 写：先存点的数量，再循环存点
			ar << static_cast<int>(m_points.size());
			for (const auto& pt : m_points)
			{
				ar << pt;
			}
		}
		else
		{
			// 读：先读数量，再循环读点
			int count = 0;
			ar >> count;
			m_points.clear();
			for (int i = 0; i < count; i++)
			{
				CPoint pt;
				ar >> pt;
				m_points.push_back(pt);
			}
		}
	}

	ShapeType GetType() const override { return ShapeType::Freehand; }
};
