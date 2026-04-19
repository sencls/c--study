// 内存管理

// c风格
/*
void *malloc(size_t size); // 返回万能指针，size是开辟的字节数，malloc在于堆上分配
void free(void *ptr);      // 释放原来由malloc，calloc，realloc分配的内存

e.g.
  int *p = (int *)malloc(sizeof(int));//malloc返回值是void* 故强转
    if (p == nullptr)
    {
        std::cerr << "Memory allocation failed" << std::endl;
        return -1;
    }
    *p = 42;
    std::cout << *p << std::endl;
    free(p);

  //malloc只开辟内存，内容不定，如果是类的话，不调用构造函数 ，Student *t =(Student*)malloc(sizeof(Student));
  //malloc只能与free搭配

  开辟数组：malloc(5*sizeof(int ))==int[5];
  e.g.
    int *arr_p = (int *)(malloc(5 * sizeof(int)));
    if (arr_p == nullptr)
    {
      std::cerr << "memory failed" << std::endl;
      return -1;
    }

    for (size_t i = 0; i < 5; i++)
    {
      arr_p[i] = i * 2; //*(arr_p+i)=i*2;
    }

    for (size_t i = 0; i < 5; i++)
    {
      std::cout << arr_p[i] << std::endl;
    }
    free(arr_p);
  */

// c++风格
/*
new delete; 直接调用构造函数
开辟数组 new type[size]; 对类 ：new student[2] { {},{} }; delete []
new(std::nothrow) type->不抛出异常直接令其返回nullptr(include<new>)
*/

// 区别 ：1.返回类型不同 2.new delete调用构造析构 3.分配失败时new 默认抛出std::bad_alloc,malloc直接返回nullptr

/* c的realloc void*(void* ptr,size_t new_size)对内存重新分配

*/
/*简单实现动态数组class dynamicArray
{
  size_t size;
  size_t capacity;
  int *data;
  void resize(size_t num)
  {
    int *temp = (int *)realloc(data, num * sizeof(int));
    if (temp == nullptr)
    {
      throw std::bad_alloc{};
    }
    data = temp;
    capacity = num;
  }

public:
  dynamicArray() : size(0), capacity(2), data((int *)malloc(capacity * sizeof(int)))
  {
    if (data == nullptr)
    {
      std::cerr << "memory failed" << std::endl;
      throw std::bad_alloc{};
    }
  }
  ~dynamicArray()
  {
    free(data);
  }
  void add(int val)
  {
    if (size == capacity)
    {
      resize(capacity * 2);
    }
    *(data + size) = val;
    size++;
  }
  int get(size_t index)
  {
    if (index >= size)
      throw std::out_of_range{"index out of range"};
    return data[index];
  }
  size_t getsize()
  {
    return size;
  }
};*/
/*简单认识内存池#include <iostream>
#include <stack>
#include <string>
class Student
{
public:
  int id;
  std::string name;
  Student(int a, std::string name) : id(a), name(name) {}
};
class memoryPool
{
  size_t objsize;
  size_t totalsize;
  char *pool;
  std::stack<void *> freelist;

public:
  memoryPool(size_t objsize, size_t total) : objsize(objsize), totalsize(total)
  {
    pool = (char *)malloc(objsize * totalsize);
    if (pool == nullptr)
      throw std::bad_alloc();
    for (size_t i = 0; i < totalsize; i++)
    {
      freelist.push(pool + i * objsize);
    }
  }
  ~memoryPool()
  {
    free(pool);
    std::cout << "release";
  }

  void *allocate()
  {
    if (freelist.empty())
      throw std::bad_alloc();
    void *p = freelist.top();
    freelist.pop();
    return p;
  }
  void deallocate(void *p)
  {
    freelist.push(p);
  }
};
int main()
{
  try
  {
    memoryPool pool(sizeof(Student), 3);
    void *p1 = pool.allocate();
    void *p2 = pool.allocate();
    void *p3 = pool.allocate();
    Student *s1 = new (p1) Student(20, "Tom");
    Student *s2 = new (p2) Student(34, "Jim");
    std::cout << s2->id << ' ' << s2->name << std::endl;
    std::cout << s1->id << ' ' << s1->name << std::endl;
    void *p4 = pool.allocate();
    s1->~Student();
    s2->~Student();

    pool.deallocate(p1);
    pool.deallocate(p2);
  }
  catch (const std::out_of_range &e)
  {
    std::cerr << "array access error" << e.what() << std::endl;
    return -1;
  }
  catch (const std::bad_alloc &e)
  {
    std::cerr << "memory failed" << e.what() << std::endl;
    return -1;
  }
  catch (const std::exception &e)
  {
    std::cerr << "error" << e.what() << std::endl;
    return -1;
  }

  return 0;
}*/
// 避免内存泄漏： 确保new与delete配对存在，使用RAII与智能指针，使用Valgrind检测内存泄漏
/*封装简单的智能指针#include "simpleSharedPtr.h"
#include <string>
#include <iostream>
#include <thread>
struct Student
{
  int age;
  std::string name;
  Student(int a, std::string b) : age(a), name(b) {}
  ~Student() { std::cout << name << std::endl; }
};

int main()
{
  simpleSharedPtr<Student> ptr1;
  std::cout << ptr1.use_count() << std::endl;

  simpleSharedPtr<Student> ptr2(new Student(25, "jim"));
  std::cout << ptr2.use_count() << std::endl;

  simpleSharedPtr ptr3(ptr2);
  std::cout << ptr2.use_count() << std::endl;
  std::cout << ptr3.use_count() << std::endl;

  ptr1 = ptr3;
  std::cout << ptr1.use_count() << std::endl;
  std::cout << ptr3.use_count() << std::endl;

  ptr2.reset(new Student(44, "tom"));
  std::cout << ptr2.use_count() << std::endl;
  std::cout << ptr3.use_count() << std::endl;

  std::thread t1([ptr2]() -> void
                 { ptr2->name = "RRR"; });

  t1.join();
}*/
/*智能指针(memory库中)
  unique_ptr<> 只能绑定一个对象，不存在赋值()，只能通过std::move()来传递，构造时可以直接传裸指针;
    std::unique_ptr<Student> ptr = std::make_unique<Student>(20, "tim");
    std::unique_ptr<Student> p(new Student(12, "jim"));
    std::unique_ptr<Student> ptr2 = std::move(p);
    std::unique_ptr<Student> ptr2 =p;//错误的
  shared_ptr<> 多对一的存在，有controlblock控制使用数量，避免出现悬空指针德情况。
    有use_count统计强引用，weak_count统计弱引用；
    std::shared_ptr<Student> ptr = std::make_shared<Student>(20, "tim");
    std::shared_ptr<Student> p(new Student(12, "jim"));
    std::shared_ptr<Student> ptr2 =p;
    std::shared_ptr<Student> ptr2 = std::move(p);
  weak_ptr<> 适用于避免循环引用(存在双向关联，如父子关系)，可以通过其访问shared_ptr管理的对象(使用lock()函数);bool expired()当前所指对象是否已被回收;
  e.g.
    class B;
    class A
    {
      public:
      std::shared_ptr<B> ptr_b;
      A() { std::cout << "A()" << std::endl; }
      ~A() { std::cout << "~A()" << std::endl; }
    };
    class B
    {
      public:
          std::shared_ptr<A> ptr_a;//改：weak_ptr
          B() { std::cout << "B()" << std::endl; }
          ~B() { std::cout << "~B()" << std::endl; }
    };

    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();
    a->ptr_b = b;
    b->ptr_a = a;
    //内存泄漏，没有调用A，B的析构函数，

*/
/* 小练习1 #include <iostream>
#include <memory>
class ResourcesManager
{
  std::unique_ptr<int> ps;

public:
  ResourcesManager(int val)
  {
    ps = std::make_unique<int>(val);
  }
  int getValue() const
  {
    return *ps;
  }
  void setValue(int newValue)
  {
    *ps = newValue;
  }

  ResourcesManager(const ResourcesManager &other) = delete;
  ResourcesManager &operator=(const ResourcesManager &other) = delete;
  ResourcesManager(ResourcesManager &&other) noexcept
  {
    ps = std::move(other.ps);
    other.ps = nullptr;
  }
  ResourcesManager &operator=(ResourcesManager &&other)
  {
    if (this != &other)
    {
      ps = std::move(other.ps);
      other.ps = nullptr;
    }
    return *this;
  }
};

int main()
{
  ResourcesManager a(10);
  std::cout << a.getValue() << std::endl;
  a.setValue(20);
  std::cout << a.getValue() << std::endl;
  ResourcesManager b(std::move(a));
  std::cout << b.getValue() << std::endl;
  return 0;
}*/
