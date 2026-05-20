# C++ Study

> 个人 C++ 学习仓库，从基础语法到网络编程，记录学习过程中的实践与探索。

## 目录结构

```
c++study/
├── basic/                  # C++ 基础
│   ├── STL/                # STL 容器与手写数据结构
│   ├── Singleton_Pattern/  # 单例模式（含 CRTP 实现）
│   ├── lv_rv/              # 左右值引用与完美转发
│   ├── stream/             # I/O 流操作
│   ├── type_ptr/           # 指针与引用
│   └── model/              # 模板编程与特化
├── asio/                   # Boost.Asio 网络编程
│   ├── Async/              # 异步服务器（线程池 / 消息队列 / Session 管理）
│   ├── Coroutine/          # C++20 协程异步编程
│   └── http/               # HTTP 协议（Boost.Beast 客户端与服务端）
├── tsoding/                # 进阶专题
│   ├── graphics/           # 基础图形编程
│   └── mechine_learning/   # 神经网络从零实现（矩阵运算 / 反向传播）
├── third_party/            # 第三方库（jsoncpp）
├── daily.cpp               # 日常练习
└── bin/                    # 编译输出
```

## 学习内容

### 基础篇 `basic/`

| 主题 | 说明 |
| :--- | :--- |
| **STL 容器** | 标准库容器使用，手写实现 `AVLMap`、`HashMap`、`MyList`、`MyDeque` 等 |
| **设计模式** | 单例模式的多种实现，包括线程安全版本与 CRTP 模板实现 |
| **模板编程** | 模板特化、元编程基础 |
| **值类别** | 左值 / 右值引用、移动语义、完美转发 |

### 网络编程篇 `asio/`

| 主题 | 说明 |
| :--- | :--- |
| **异步服务器** | 基于 Boost.Asio 的异步 TCP 服务器，含线程池、消息队列、Session 管理 |
| **C++20 协程** | 使用 `co_spawn` / `awaitable` 实现协程版 Echo 服务器与 HTTP 客户端 |
| **HTTP 协议** | 基于 Boost.Beast 的 HTTP 客户端与服务端实现 |

### 进阶篇 `tsoding/`

| 主题 | 说明 |
| :--- | :--- |
| **神经网络** | 从零实现前馈神经网络，包含矩阵运算库与反向传播算法 |
| **XOR 问题** | 经典异或问题的神经网络求解 |
| **图形编程** | 基础图形渲染探索 |


## 学习进度

- [x] C++ 基础语法与 STL
- [x] 模板编程与设计模式
- [x] 左右值引用与移动语义
- [x] Boost.Asio 异步网络编程
- [x] C++20 协程
- [x] HTTP 协议实现
- [x] 神经网络基础
- [ ] WebSocket 实战
- [ ] 更多进阶专题...

