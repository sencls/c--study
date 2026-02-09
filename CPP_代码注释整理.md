# C++ 代码注释整理

> 本文档整理了项目中所有 .cpp 文件的注释内容（排除 jsontest 和 bin 目录）

---

## 目录

1. [内存管理](#内存管理)
2. [可调用对象](#可调用对象)
3. [STL 容器](#stl-容器)
4. [模板特化](#模板特化)
5. [模板元编程](#模板元编程)
6. [左值与右值](#左值与右值)
7. [单例模式](#单例模式)

---

## 内存管理

### C 风格内存管理

```cpp
void *malloc(size_t size);  // 返回万能指针，size是开辟的字节数，malloc在于堆上分配
void free(void *ptr);       // 释放原来由malloc，calloc，realloc分配的内存
```

**示例：**
```cpp
int *p = (int *)malloc(sizeof(int));  // malloc返回值是void* 故强转
if (p == nullptr) {
    std::cerr << "Memory allocation failed" << std::endl;
    return -1;
}
*p = 42;
std::cout << *p << std::endl;
free(p);
```

**注意：**
- `malloc` 只开辟内存，内容不定，如果是类的话，不调用构造函数
- `malloc` 只能与 `free` 搭配

**开辟数组：**
```cpp
int *arr_p = (int *)(malloc(5 * sizeof(int)));
// 相当于 int[5]
```

### C++ 风格内存管理

```cpp
new/delete;  // 直接调用构造函数
```

**开辟数组：**
```cpp
new type[size];           // 基本类型
new student[2] {{},{}};   // 类类型
delete[] arr;             // 释放数组需要加[]
```

**不抛异常的 new：**
```cpp
new(std::nothrow) type  // 不抛出异常，直接令其返回nullptr (需要 #include<new>)
```

### malloc/free 与 new/delete 的区别

| 特性 | malloc/free | new/delete |
|------|-------------|------------|
| 返回类型 | void* | 类型指针 |
| 构造/析构 | 不调用 | 调用构造/析构函数 |
| 分配失败 | 返回 nullptr | 默认抛出 std::bad_alloc |

### C 的 realloc

```cpp
void* realloc(void* ptr, size_t new_size)  // 对内存重新分配
```

### 简单实现动态数组

```cpp
class dynamicArray {
    size_t size;
    size_t capacity;
    int *data;

    void resize(size_t num) {
        int *temp = (int *)realloc(data, num * sizeof(int));
        if (temp == nullptr) {
            throw std::bad_alloc{};
        }
        data = temp;
        capacity = num;
    }

public:
    dynamicArray() : size(0), capacity(2), data((int *)malloc(capacity * sizeof(int))) {
        if (data == nullptr) {
            std::cerr << "memory failed" << std::endl;
            throw std::bad_alloc{};
        }
    }

    ~dynamicArray() {
        free(data);
    }

    void add(int val) {
        if (size == capacity) {
            resize(capacity * 2);
        }
        *(data + size) = val;
        size++;
    }

    int get(size_t index) {
        if (index >= size)
            throw std::out_of_range{"index out of range"};
        return data[index];
    }

    size_t getsize() {
        return size;
    }
};
```

### 简单认识内存池

```cpp
class memoryPool {
    size_t objsize;
    size_t totalsize;
    char *pool;
    std::stack<void *> freelist;

public:
    memoryPool(size_t objsize, size_t total) : objsize(objsize), totalsize(total) {
        pool = (char *)malloc(objsize * totalsize);
        if (pool == nullptr)
            throw std::bad_alloc();
        for (size_t i = 0; i < totalsize; i++) {
            freelist.push(pool + i * objsize);
        }
    }

    ~memoryPool() {
        free(pool);
        std::cout << "release";
    }

    void *allocate() {
        if (freelist.empty())
            throw std::bad_alloc();
        void *p = freelist.top();
        freelist.pop();
        return p;
    }

    void deallocate(void *p) {
        freelist.push(p);
    }
};
```

### 避免内存泄漏

确保 `new` 与 `delete` 配对存在，使用 RAII 与智能指针，使用 Valgrind 检测内存泄漏。

---

## 可调用对象

C++ 中的可调用对象包括：**函数指针、仿函数、lambda 表达式、std::function、std::bind**

### 1. 函数指针

```cpp
int (*p)(int, int);

int add(int a, int b) {
    return a + b;
}

p = add;               // 等价于 p = &add;
std::cout << (*p)(1, 2);  // 等价于 p(1, 2)
```

**优点：** 简单直观，适合简单的回调函数
**缺点：** 不能捕获上下文，指针的声明和使用较复杂

### 2. 仿函数（函数对象）

仿函数是重载了 `operator()` 的类或结构体。

```cpp
struct adder {
    int to_add;
    adder(int val) : to_add(val) {}
    int operator()(int x) {
        return x + to_add;
    }
};

adder a(2);
std::cout << a.to_add;  // 输出: 2
int b = a(1);           // 调用 operator()
std::cout << b;         // 输出: 3
```

**优点：**
- 携带状态：可以拥有内部状态，通过成员变量存储数据，调用时具备上下文
- 灵活性高：可以根据需要添加更多的成员函数和变量，拓展功能
- 性能优化：编译器对仿函数优化，内联展开
- 类型安全：使用时编译期间检查

**使用场景：** 需要携带状态时，复杂操作时（lambda 难以表达的复杂逻辑），性能关键场景

**与 STL 结合使用：**

```cpp
struct IsGreatThan {
    int threshold;
    IsGreatThan(int val) : threshold(val) {}
    bool operator()(int x) const {
        return x > threshold;
    }
};

IsGreatThan great(10);
std::vector<int> num = {1, 2, 10, 7};
auto it = std::find_if(num.begin(), num.end(), great);
```

**泛型比较器：**

```cpp
template <typename T>
struct Compare {
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};

std::vector<int> a = {1, 1, 1, 44565, 3, 7, 8, 312};
std::sort(a.begin(), a.end(), Compare<int>());
```

### 3. Lambda 表达式

```cpp
[capture](parameters) -> return_type {
    // 函数体
}
```

- **capture**: 向外捕获需要的变量，值捕获、引用捕获、混合捕获
  - `[=]` 值捕获
  - `[&]` 引用捕获
  - `[=,&val]`, `[&,val]` 混合捕获
  - `[=name]` 加变量名只收集对应的内容，不加会收集前面所有变量的
- **parameters**: 参数列表
- **return_type**: 可以不写，C++14 自动推导

**示例：通过 erase 与 remove_if 实现目标删除**

```cpp
int threshold = 10;
std::vector<int> a = {1, 12, 111, 4, 3, 7, 8, 312};
a.erase(std::remove_if(a.begin(), a.end(),
                       [threshold](int n) -> bool {  // true 满足条件删除
                           return n > threshold;
                       }),
        a.end());
```

**优点：** 简洁，灵活，性能优化，与 STL 无缝结合使用

**`mutable` 关键字：** `[]() mutable` 可变 lambda 可以让值捕获的变量在函数内修改，但作用域外的值不会修改

**在类中使用：**

```cpp
class Precessor {
    int _threshold;

public:
    Precessor(int threshold) : _threshold(threshold) {}

    void process(std::vector<int> &data) {
        data.erase(std::remove_if(data.begin(), data.end(),
                                  [this](int x) -> bool {
                                      return x <= _threshold;
                                  }),
                   data.end());
    }
};

// 类外使用 a 是类的变量，用引用捕获
auto dfun = [&a](int x) -> void { a.m(23, x); };
```

### 4. std::function

`std::function`（functional 库）是 C++11 通用的可调用对象，可以封装任何可调用对象，包括普通函数、lambda 函数、函数对象以及绑定表达式。

```cpp
std::function<int(int, int)> callback = add;  // 绑函数
std::cout << callback(1, 2) << std::endl;

A a;
std::function<int(int, int)> fun1 = a;  // 仿函数
std::cout << fun1(1, 2) << std::endl;

std::function<int(int)> fun2 = [](int x) -> int { return x; };  // lambda 函数
std::cout << fun2(3) << std::endl;

std::function<void()> fun3 = []() -> void {
    std::cout << "aaaaa" << std::endl;
};
```

**特点：**
- 可以存储各种可调用对象
- 支持动态改变存储的可调用对象
- 频繁调用时有开销

**使用场景：**

1. **回调函数**

```cpp
using CallBack = std::function<void(int)>;

void triggerEvent(CallBack cb, int value) {
    cb(value);
}

triggerEvent([](int x) -> void {
    std::cout << "aaa" << ' ' << x << std::endl;
}, 3);

struct Printer {
    void operator()(int x) const {
        std::cout << "调用" << x << std::endl;
    }
} printt;

triggerEvent(printt, 4);
```

2. **存储与调用不同类型的可调用对象**

```cpp
// 使用容器类来存储
// 比如 vector<std::function<int(int,int)>>, map 之类
// emplace_back 比 push_back 少一次构造，更节省
```

### 5. std::bind

`std::bind()` 在 functional 库中，可以绑定对象函数，同时控制一定的参数，待定参数使用 `std::placeholders` 占位。

```cpp
// bind(绑定对象，参数1，参数2)
auto neew = std::bind(add, std::placeholders::_1, 2);  // 相当于 add(x, 2)

// 类成员函数，成员函数需要取地址，类对象做第二个参数，也取地址
auto neew = std::bind(&A::m, &a, std::placeholders::_1, 2);
// 静态成员函数不需要第二参数
```

---

## STL 容器

### list（双端链表）

```cpp
List<int> l;
l.push_back(22);         // 尾部加
l.push_front(11);        // 前向加
l.insert(--l.end(), 12); // 迭代器前面加, 返回值是添加元素所在的迭代器
l.remove(2);             // 遍历全部，删除所有等值元素
l.erase(--l.end());      // 删除迭代器所指位置，并返回删除后迭代器所指
l.pop_back();            // 删除尾部
l.pop_front();           // 删除头部
int a = l.front();       // 取头
int b = l.back();        // 取尾
```

### deque（双端队列）

支持前端后端以 Θ(1) 进行插入与删除，访问常数时间，但中间插入是 Θ(n)

**内部原理：**
由大小固定的数组块组成，这些块通过一个中央映射数组管理

1. **中央映射数组 (map)**：指针数组，指向各数组块，本身也是动态分配，可以在两端添加新的数据块
2. **数据块**：本身是固定大小连续的内存区域，大小通常与平台和编译器有关
3. **起始与结束指针**：维护指向 map 的第一个有效数据块与第一个无效数据块，以此来达到双端的增减

**函数：**

```cpp
std::deque<std::string> dq;
dq.push_back("hello");  // 后插
dq.push_back("back");
dq.push_front("c++");   // 前插
dq.push_front("Java");
dq.pop_front();         // 前删
dq.pop_back();          // 后删
std::string a = dq.front();
```

### map（关联容器）

存储键值对，键是唯一的，并按特定顺序（通常是升序）自动排序，常基于红黑树实现

**特点：**
- 内容有序
- 键唯一（重复插入会报错）
- 通过键查询在对数时间复杂度
- 动态插入和删除

```cpp
std::map<int, std::string> mymap = {{1, "www"}, {3, "qqqq"}, {2, "eeee"}};

// 插入
mymap.insert(std::pair<int, std::string>(12, "hello"));
mymap.insert(std::make_pair(5, "for"));
mymap.insert({23, "alal"});
mymap.insert({23, "al"});  // 重复插入，不会复写，不会报错，但不操作

// 修改/添加
mymap[12] = "aaaa";   // 可以修改
mymap[24] = "qwe";    // 没有对应的键就添加

// 访问
auto f = mymap[12];   // 不存在的话，会默认生成一个 null 值的键对
// auto r = mymap.at(11);  // 安全访问，没有会抛出异常

// 查找
auto t = mymap.find(3);  // 利用键搜索，返回迭代器
if (t != mymap.end())
    std::cout << t->second << std::endl;

// 删除
mymap.erase(mymap.begin(), mymap.find(12));  // 可以传迭代器或者键值，两个参数(迭代器)时删除符合参数范围左闭右开区间

// 其他操作
std::cout << mymap.size() << std::endl;
mymap.count(1);                   // 返回当前键值的出现次数
auto p = mymap.lower_bound(12);   // 返回第一个不小于该键值的迭代器
auto s = mymap.upper_bound(24);   // 返回第一个大于的迭代器

// 泛型函数的使用
for_each(p, s, [](auto pair) -> void {
    std::cout << pair.first << " " << pair.second << ' ';
});

// 遍历
for (const auto &t : mymap) {
    std::cout << t.first << ' ' << t.second << std::endl;
}

// transform
std::map<int, std::string> test;
std::transform(p, s, std::inserter(test, test.end()), [](auto &pair) -> auto {
    auto new_p = pair.second;
    std::transform(new_p.begin(), new_p.end(), new_p.begin(), ::toupper);
    return std::make_pair(pair.first, new_p);
});
```

**键是自定义类型时**，需提供比较函数：

```cpp
// 方式1：类重载 <
class M {
    bool operator<(M a) {
        return __ > a.__;
    }
};
std::map<type, type>

// 方式2：提供可调用对象
struct comp {
    bool operator(type a, type b) {
        return a.__ > b.__;
    }
};
std::map<type, type, comp>

// 方式3：使用 function（不可直接使用 lambda 函数，可以通过 function 间接使用）
std::function<bool(type a, type b)> comp = [](type a, type b) -> bool {
    return a < b;
};
std::map<type, other_type, std::function<bool(type a, type b)>> a(comp);
// e.g. std::map<Person, std::string, decltype(comp)> qw(comp);
```

### unordered_map（无序映射）

无序列表，常数时间的查找、插入、删除，但不保证顺序

**基本模板定义：**

```cpp
std::unordered_map<KeyType, ValueType, Hash=std::hash<KeyType>, KeyEqual=std::equal_to<KeyType>, Allocator=std::allocator<std::pair<const KeyType, ValueType>>>
// hash: 哈希函数
// keyequal: 键值相等时
// allocator: 内存分配器
```

**操作：**

```cpp
// 通过 [] 访问 map 中，没有的对象自动构建对象，然后给默认值
// insert() 插入函数 insert({}), emplace(key, value)
// erase()，根据参数内容返回对象，迭代器->删除后所指的迭代器，键->值
// bucket_count() 返回桶数量
// rehash() 调整前者数量
```

**自定义键**，需要定义比较函数、哈希函数

---

## 模板特化

### 模板模板参数

```cpp
template <template <typename, typename> class node, typename R>
class add {
    void print(const node<R, std::allocator<R>> &c) {
        for (auto &t : c) {
            std::cout << t << std::endl;
        }
    }
};

add<std::vector, int> ma;
```

### 模板特化

1. **全特化**
2. **偏特化**

函数只能全特化，编译器优先级：偏特化 > 全特化 > 通用

```cpp
// 通用模板
template <typename a, typename b>
class text {
public:
    void print() {
        std::cout << "我一样";
    }
};

// 全特化
template <>
class text<std::string, int> {
public:
    void print() {
        std::cout << "我不一样";
    }
};

// 偏特化
template <typename a>
class text<a, int*> {
public:
    void print() {
        std::cout << "我不一样";
    }
};

// 函数特化
template<typename a>
void print(const a& tri) {}

template<>
void print<int*>(int* const& value) {}
```

### 变参模板

使用参数包，通过 `...` 语法来表示

```cpp
template <typename... Args>
class as {};

template <typename T, typename... Args>
void myfun(T first, Args... args) {
    std::cout << first;
}

// 递归展开
void printl() {
    std::cout << "all";
}

template <typename a, typename... Args>
void printl(const a &first, const Args &...args) {
    std::cout << first << " ";
    printl(args...);  // int a = 1, b = 1, c = 1, d = 1, e = 1, f = 1;
}
```

**C++17 优化：折叠表达式**

```cpp
template <typename... Args>
void ip(const Args &...args) {
    ((std::cout << args), ...);
}
```

**折叠表达式：**

1. **一元**：对内部参数使用一个一元操作符
   - `(op...pack)`, `(pack...op)` // 不常用

```cpp
bool sum(const Args &...args) {
    return (!args && ...);  // 取非，再与； (!a)&&(!b)&&...
}
```

2. **二元**：使用二元操作符
   - **左折叠** `(init op ... op pack)`：从左到右结合
   - **右折叠** `(pack op ... op init op ...)`

```cpp
// 左折叠
auto sum(const Args&... args) -> decltype((args * ...)) {
    return (args + ...)
}

// 右折叠
auto sum(const Args &...args) -> decltype((... * args)) {
    return (... * args);
}

void print(const Args &...args) {
    //((std::cout << args), ...);
    //(std::cout << ... << args);
}

template <typename... Args, typename a>
auto maxn(const a &t, const Args &...args) -> decltype(std::max(t, (args, ...))) {
    return std::max(t, (args, ...));
}
```

### SFINAE（Substitution Is Not An Error）

替换失败不是错误，编译器实例化时，在替换模板参数时失败，不会将其视作编译错误，而是寻找其他可能的模板或者重载

**应用：**
1. 函数重载选择
2. 类型特性检测
3. 条件编译

**示例：使用 enable_if**

```cpp
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_type(T value) {
    std::cout << "int " << std::endl;
}

template <typename T>
typename std::enable_if<std::is_same<T, const char*>::value, void>::type
print_type(T value) {
    // ...
}

// is_point<>::value 确定是否指针
```

**C++20：concept**

```cpp
concept Integral = std::is_integral_v<T>;

// requires() 搭配使用，前者限定条件，后者排除条件
template <typename T>
concept printable = requires(T a) {
    { std::cout << a } -> std::same_as<std::ostream &>;
};

template <printable T>
void print(T a) {
    std::cout << a << std::endl;
}
```

**判断是否含有该成员**

```cpp
template <typename T>
class has_foo {
private:
    typedef char yes[1];
    typedef char no[2];
    template <typename U, void (U::*)()>  // u::* u的成员函数指针
    struct SFINAE {};

    template <typename U>
    static yes &test(SFINAE<U, &U::foo> *);

    template <typename U>
    static no &test(...);  // ...指任意参数

public:
    static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
};

template <typename T>
typename std::enable_if<has_foo<T>::value, void>::type
call_foo(T &obj) {
    obj.foo();
    std::cout << "foo()" << std::endl;
}

class Withfoo {
public:
    void foo() { std::cout << "has\n"; }
};

class nofoo {};
```

**检查类型是否有 value_type**

```cpp
template <typename T, typename = void>
struct has_non_void_value_type : std::false_type {};

template <typename T>
struct has_non_void_value_type<T, std::__enable_if_t<!std::is_void_v<typename T::value_type>>> : std::true_type {};

template <typename T, bool HasValueType = has_non_void_value_type<T>::value>
struct TypePrint {};

template <typename T>  // T有成员类型
struct TypePrint<T, true> {  // 显式添加typename是为了告诉编译器这是类型，而不是对象
    static void print() {
        std::cout << "T has a member type value_type";
    }
};

template <typename T>
struct TypePrint<T, false> {
    static void print() {
        std::cout << "no type";
    }
};

struct Withint {
    using value_type = int;
};

struct noint {};
```

---

## 模板元编程

在编译期通过模板机制进行代码生成与计算的技术，利用编译器的模板实例化机制，在编译期执行代码逻辑，提高性能和灵活性

**优点：**
- 提高代码的可重复性与泛化能力
- 减小运行时开销
- 实现类型安全的高级抽象

**基础：** 模板特化、递归模板

### 示例1：阶乘计算

```cpp
template <int N>
struct Factorial {
    // 改为 inline static const int value = ... 就不需要类外定义，来防止取内存崩
    static const int value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static const int value = 1;
};

// 为 Factorial<0>::value 提供定义，使其可以取地址
template <int N>
const int Factorial<N>::value;

template <>
const int Factorial<0>::value;  // 静态编译会报错，但不影响运行
```

### 示例2：判断类型是否可相加

```cpp
// 实现编译阶段，判断编译类型是否具有一定能力
template <typename T, typename = void>
struct is_addable : std::false_type {};

template <typename T>
struct is_addable<T, decltype(void(std::declval<T>() + std::declval<T>()))> : std::true_type {};
// declval: 不创建实际对象，但返回一个对应类型的右值引用

static_assert(is_addable<int>::value, "int should be added");
static_assert(!is_addable<void>::value, "no");
```

### 示例3：类型列表

```cpp
// 类型检测
template <typename... Ts>
struct typelist {};

template <typename list, std::size_t N>
struct TypeAt {};

template <typename Head, typename... Tail>
struct TypeAt<typelist<Head, Tail...>, 0> {
    using type = Head;
};

template <typename Head, typename... Tail, std::size_t N>
struct TypeAt<typelist<Head, Tail...>, N> {
    using type = typename TypeAt<typelist<Tail...>, N - 1>::type;
};
```

---

## 左值与右值

### 定义

| 类型 | 说明 | 示例 |
|------|------|------|
| **lvalue** | 具有持久存储的对象，可出现在赋值语句左侧，可被取地址 | 变量名、引用 |
| **rvalue** | 临时对象或没有持久存储的值，通常出现在赋值语句右侧，不能被取地址 | 字面量、临时对象、表达式结果 |

### C++11 细分

- **纯右值 (prvalue)**：临时对象、字面量
- **将亡值 (xvalue, expiring value)**：表示即将被移动的对象，`std::move()` 的结果

### 类型判断

```cpp
std::is_lvalue_reference<decltype()>::value  // 判断是否是左值引用
std::is_lvalue_reference_v<decltype()>        // 简写版本

// 判断左值时 decltype((a)) -> 两层括号

int a = 10;  // 右值
// std::is_lvalue_reference_v<decltype((a))> -> true;

int &b = a;
// std::is_lvalue_reference_v<decltype(b)> -> true;
```

### 模板万能引用（转发引用）

C++11 引入的引用类型

1. 表现形式为 `T&&`：T 是模板参数
2. 编译器会解析其为左值引用还是右值引用
   - T 被推导为普通类型 → 右值引用
   - T 被推导为引用类型 → 左值引用
3. 能统一处理左值与右值

**引用折叠规则：** 处理引用类型的嵌套

```cpp
// 只要有一个是左值引用，结果就是左值引用
// 只有两个都是右值引用，结果才是右值引用

// & &&、&& &、& & -> &  左值引用
// && && -> && 右值引用
```

**重要：** 右值引用类型做表达式时 → 左值

```cpp
template <typename T>
void fun1(T &&x) {
    std::cout << "lv " << std::is_lvalue_reference_v<T> << std::endl;
    std::cout << "rv " << std::is_rvalue_reference_v<T> << std::endl;
    std::cout << "x is rv " << std::is_rvalue_reference_v<T &&> << std::endl;
    std::cout << "x is lv " << std::is_lvalue_reference_v<decltype(x)> << std::endl;
    std::cout << "x is rv " << std::is_rvalue_reference_v<decltype(x)> << std::endl;
}

fun1(a);              // 传入左值
fun1(c);              // 传入左值
fun1(std::move(c));   // 传入右值
```

**重载函数与 std::forward**

```cpp
void process(int &x) {
    std::cout << "(int&)" << std::endl;
}

void process(int &&x) {
    std::cout << "(int&&)" << std::endl;
}

template <typename T>
void wrapper(T &&x) {
    process(std::forward<T>(x));  // 保持类型
}

void display_type(T &&param) {
    std::cout << "T is lv reference: " << std::is_lvalue_reference_v<T> << std::endl;
    std::cout << "param is lvalue reference: " << std::is_lvalue_reference_v<decltype(param)> << std::endl;
    std::cout << "T traits normal type is int " << std::is_integral_v<typename std::remove_reference_t<T>> << std::endl;
}
```

### 类型推导规则

模板编程中，编译器根据传入的实参自动推导模板参数类型

1. 当模板参数与函数参数匹配时，自动推导
2. 引用的处理：引用折叠
3. cv 限定符的去除：const 与 volatile 修饰会去除

```cpp
// std::remove_reference_t<> 去除引用
```

### 原样转发

在模板函数中，将接收到的参数以其原有的值类别（左值、右值）传递给另一个函数，确保泛型代码可以正确处理参数

**目的：**
1. 保持参数值类别不变，确保正确调用函数重载
2. 利用移动语义，避免不必要的拷贝，提升性能
3. 编写通用、复用性强的代码

**实现：** 通过模板万能引用、`std::forward<T>()`

```cpp
template <typename F, typename T1, typename T2>
void flip1(F f, T1 t1, T2 t2) {
    f(t2, t1);
}

template <typename F, typename T1, typename T2>
void flip2(F f, T1 &&t1, T2 &&t2) {
    f(t2, t1);
}

void ftemp(int v1, int &v2) {
    std::cout << v1 << ' ' << ++v2 << std::endl;
}

void use_ftemp1() {
    int i = 100, j = 99;
    flip1(ftemp, j, 42);
    std::cout << j << std::endl;
}

void use_ftemp2() {
    int i = 100, j = 99;
    flip2(ftemp, j, 42);
    std::cout << j << std::endl;
}

void gtemp(int &&i, int &j) {
    std::cout << i << ' ' << ++j << std::endl;
}

void use_ftemp3() {
    int i = 100, j = 99;
    flip2(gtemp, i, j);
    std::cout << j << std::endl;
    std::cout << i << std::endl;
}
```

---

## 单例模式

创建型设计模式，确保一个类在整个程序的生命周期中有且只有一个实例，并提供一个全局访问点来获取该实例

### 作用

1. **控制实例数量**：确保只有一个，防止创建多个而导致的资源浪费或状态不一致
2. **提供全局访问点**：使得程序在任何位置都可以方便的访问实例
3. **延迟实例化**：通常采用懒加载模式，即第一次需要才创建
4. **避免命名冲突**：减少命名冲突的风险
5. **管理共享资源**：在多线程模式下，单例可以有效管理共享资源

### 使用场景

1. 需要确保全局只有一个实例：配置管理、日志系统、设备驱动
2. 需要全局访问点来协调系统中的多个部分：缓存、线程池

### 实现方式

#### 1. 局部静态变量方式（推荐）

C++98 时存在隐患（多线程时可能生成多个），C++11 编译器优化，避免了隐患（静态成员多次调用只会创建一次）

```cpp
class Single2 {
private:
    Single2() {}
    Single2(const Single2 &) = delete;
    Single2 &operator=(const Single2 &) = delete;

public:
    static Single2 &GetInst() {
        static Single2 single;  // 第一次调用生成对象，后续调用共用
        return single;
    }
    ~Single2() {
        std::cout << "destory\n";
    }
};
```

#### 2. 指针方式（饿汉式）

开始就创建，在对应的 .cpp 文件中或 main() 函数中：

```cpp
// Single2Hungry* Single2Hungry::single = Single2Hungry::GetInst();

class Single2 {
private:
    Single2() {}
    Single2(const Single2 &) = delete;
    Single2 &operator=(const Single2 &) = delete;
    static Single2 *single;

public:
    static Single2* GetInst() {
        if (single == nullptr) {
            single = new Single2();
        }
        return single;
    }
    ~Single2() {
        std::cout << "destory\n";
    }
};
// 析构存在问题
```

#### 3. 指针方式（懒汉式）

调用时才创建

```cpp
class Single2 {
private:
    Single2() {}
    Single2(const Single2 &) = delete;
    Single2 &operator=(const Single2 &) = delete;
    static Single2 *single;
    static std::mutex _mtx;

public:
    static Single2 *GetInst() {
        std::lock_guard<std::mutex> lock(_mtx);  // 存在串行的可能
        if (single == nullptr) {
            single = new Single2();
        }
        return single;
    }
    ~Single2() {
        std::cout << "destory\n";
    }
};
```

**改进：双重锁**

仍然存在线程不安全问题：因为 new 的关系，在多线程时可能取到开辟了地址但没有构造的结果，导致数据混乱或者直接崩溃

```cpp
static Single2 *GetInst() {
    if (single != nullptr) {
        return single;
    }
    _mtx.lock();
    if (single != nullptr) {
        _mtx.unlock();
        single = new Single2();
    }
    single = new Single2();
    _mtx.unlock();
    return single;
}
```

#### 4. C++11 改进：once_flag

```cpp
class singleOnceFlag {
private:
    singleOnceFlag() {}
    static std::shared_ptr<singleOnceFlag> _instance;

public:
    ~singleOnceFlag() {
        std::cout << "desorty\n";
    }
    singleOnceFlag &operator=(const singleOnceFlag &) = delete;
    singleOnceFlag(const singleOnceFlag &) = delete;

    static std::shared_ptr<singleOnceFlag> GetInst() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            _instance = std::shared_ptr<singleOnceFlag>(new singleOnceFlag());
        });
        return _instance;
        // 为什么不用 make_shared() -> 要调用构造函数，但是私有无法使用
    }
};

std::shared_ptr<singleOnceFlag> singleOnceFlag::_instance = nullptr;
// 注意静态成员的类外初始化，链接时可能出错
```

### CRTP 单例模板

CRTP 将派生类作为模板参数传递给基类，即一个类继承自一个以自身为模板参数的基类

常用于：静态多态、接口的默认实现、编译时策略选择

```cpp
template<typename T>
class TempClass {};

class RealClass : public TempClass<RealClass> {};
```

---

> 文档生成时间：2025年
> 整理者：蕾姆 💙
