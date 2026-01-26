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
#include <iostream>
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
}
