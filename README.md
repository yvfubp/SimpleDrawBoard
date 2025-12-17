# SimpleDrawBoard (简易绘图板)

基于 MFC 的面向对象绘图程序，实现了图形绘制、属性编辑、持久化存储及撤销重做功能。

## ✨ 功能特性
- **基本图形**: 支持直线、矩形、椭圆、自由曲线 (铅笔)。
- **样式编辑**: 支持自定义颜色和线宽 (1px - 8px)。
- **高级交互**: 支持无限级撤销/重做 (Undo/Redo)。
- **文件支持**: 自定义二进制格式 (.sdb) 保存与读取。
- **打印支持**: 所见即所得的打印输出。

## 🏗️ 架构设计
- **Design Patterns**: 
  - **Factory**: `ShapeFactory` 负责创建不同类型的图形对象。
  - **Command**: `AddShapeCommand` 封装绘图操作，支持撤销。
  - **Strategy**: `OnDraw` 中的绘图策略。
- **RAII**: 使用 `CPenWrapper` 等智能指针管理 GDI 资源，杜绝内存泄漏。
- **Unit Test**: 集成 GoogleTest，覆盖核心工厂与序列化逻辑。

## 🚀 编译环境
- Visual Studio 2026
- MFC (在共享 DLL 中使用)
- Unicode 字符集
