// Commands.h
#pragma once
#include <vector>
#include <memory>
#include "Shapes.h"

// 1. 命令基类接口
class IDrawCommand {
public:
    virtual ~IDrawCommand() = default;
    virtual void Execute() = 0;   // 执行（或重做）
    virtual void UnExecute() = 0; // 撤销
};

// 2. 具体命令：添加图形命令
class AddShapeCommand : public IDrawCommand {
private:
    // 持有对文档数据列表的引用，以便修改它
    std::vector<std::shared_ptr<IShape>>& m_targetList;
    std::shared_ptr<IShape> m_shape; // 要操作的那个图形对象

public:
    AddShapeCommand(std::vector<std::shared_ptr<IShape>>& list, std::shared_ptr<IShape> shape)
        : m_targetList(list), m_shape(shape) {
    }

    // 【执行】：把图形加进列表
    void Execute() override {
        m_targetList.push_back(m_shape);
    }

    // 【撤销】：把图形从列表移除
    void UnExecute() override {
        if (!m_targetList.empty() && m_targetList.back() == m_shape) {
            m_targetList.pop_back();
        }
        // 注意：简单的 pop_back 假设撤销的是最后一个。
        // 如果支持非线性撤销，需要更复杂的逻辑，但这里栈式撤销足够了。
    }
};