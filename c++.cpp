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
    malloc只能与free搭配

*/
