// 内存管理
#include <cstdlib>
// c风格
void *malloc(size_t size); // 返回万能指针，size是开辟的字节数，malloc在于堆上分配
void free(void *ptr);      // 释放原来由malloc，calloc，realloc分配的内存