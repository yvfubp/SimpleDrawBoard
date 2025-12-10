// ShapeFactory.h
#pragma once
#include "Shapes.h"
#include <memory>

class ShapeFactory {
public:
    // 静态工厂方法：根据枚举创建对应的图形对象
    static std::shared_ptr<IShape> CreateShape(ShapeType type) {
        switch (type) {
        case ShapeType::Line:
            return std::make_shared<CLine>();
        case ShapeType::Rectangle:
            return std::make_shared<CRectangle>();
        default:
            return nullptr;
        }
    }
};