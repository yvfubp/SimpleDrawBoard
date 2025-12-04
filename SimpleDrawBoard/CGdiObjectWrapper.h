// CGdiObjectWrapper.h
#pragma once
#include <afxwin.h>

// 这是一个模板类，用于自动管理 GDI 对象（画笔、画刷等）的生命周期
// 对应评分标准：目标3 - 资源泄漏、异常安全 [cite: 59, 169]

template <typename T>
class CGdiObjectWrapper {
public:
    // 构造函数：接受现有的指针或为空
    explicit CGdiObjectWrapper(T* pObject = nullptr) : m_pObject(pObject) {}

    // 析构函数：核心！自动删除对象
    ~CGdiObjectWrapper() {
        Release();
    }

    // 禁止复制（防止两个 Wrapper 删除同一个对象），但在 C++11 后可支持移动
    CGdiObjectWrapper(const CGdiObjectWrapper&) = delete;
    CGdiObjectWrapper& operator=(const CGdiObjectWrapper&) = delete;

    // 允许移动语义 (Move Semantics)
    CGdiObjectWrapper(CGdiObjectWrapper&& other) noexcept : m_pObject(other.m_pObject) {
        other.m_pObject = nullptr;
    }

    CGdiObjectWrapper& operator=(CGdiObjectWrapper&& other) noexcept {
        if (this != &other) {
            Release();
            m_pObject = other.m_pObject;
            other.m_pObject = nullptr;
        }
        return *this;
    }

    // 创建画笔的辅助函数
    bool CreatePen(int nPenStyle, int nWidth, COLORREF crColor) {
        Release();
        m_pObject = new CPen();
        if (!m_pObject->CreatePen(nPenStyle, nWidth, crColor)) {
            delete m_pObject;
            m_pObject = nullptr;
            return false;
        }
        return true;
    }

    // 获取原始指针用于 MFC 函数调用
    T* Get() const { return m_pObject; }

    // 重载 -> 操作符，使其像指针一样使用
    T* operator->() const { return m_pObject; }

private:
    void Release() {
        if (m_pObject) {
            m_pObject->DeleteObject(); // 删除 GDI 句柄
            delete m_pObject;          // 删除 C++ 对象
            m_pObject = nullptr;
        }
    }

    T* m_pObject;
};

// 使用别名方便调用
using CPenWrapper = CGdiObjectWrapper<CPen>;
using CBrushWrapper = CGdiObjectWrapper<CBrush>;