/*
//list 双端链表
    List<int> l;
    l.push_back(22);         // 尾部加
    l.push_front(11);        // 前向加
    l.insert(--l.end(), 12); // 迭代器前面加,返回值是添加元素所在的迭代器
    l.print();
    std::cout << std::endl;
     l.remove(2);           // 遍历全部，删除所有等值元素
    l.erase(--l.end()); // 删除迭代器所指位置，并返回删除后迭代器所指
    l.print();
    std::cout << std::endl;
    l.pop_back();  // 删除尾部
    l.pop_front(); // 删除头部
    l.print();
    int a = l.front(); // 取头
    int b = l.back();  // 取尾
//deque 双端链表 支持前端后端以Θ(1)进行插入与删除，访问常数时间，但中间插入是Θ(n);
    内部原理： 由大小固定的数组块组成，这些块通过一个中央映射数组管理;
              1.中央映射数组(map)：指针数组 指向各数组块，本身也是动态分配，可以在两端添加新的数据块；
              2.数据块：本身是固定大小连续的内存区域的，大小通常与平台和编译器有关
              3.起始与结束指针：维护指向map的第一个有效数据块与第一个无效数据块，以此来达到双端的增减；
    函数：push_front/back;pop_front/back;
    std::deque<std::string> dq;
    dq.push_back("hello"); // 后插
    dq.push_back("back");
    dq.push_front("c++"); // 前插
    dq.push_front("Java");
    dq.pop_front(); // 前删
    dq.pop_back();  // 后删
    std::string a = dq.front();
//map 关联容器，存储键值对，键是唯一的，并按特定顺序(通常是升序)自动排序,常基于红黑树实现；
                内容有序，键唯一(重复插入会报错)，通过键查询在对数时间复杂度，动态插入和删除

    std::map<int, std::string> mymap = {{1, "www"}, {3, "qqqq"}, {2, "eeee"}};
    mymap.insert(std::pair<int, std::string>(12, "hello"));
    mymap.insert(std::make_pair(5, "for"));
    mymap.insert({23, "alal"});
    mymap.insert({23, "al"}); // 重复插入，不会复写，不会报错，但不操作
    mymap[12] = "aaaa";       // 可以修改
    mymap[24] = "qwe";        // 没有对应的键就添加
    // auto r = mymap.at(11);    // 安全访问，没有会抛出异常
    auto f = mymap[12];     // 不存在的话，会默认生成一个null值的键对;
    auto t = mymap.find(3); // 利用键搜索，返回迭代器
    if (t != mymap.end())
        std::cout << t->second << std::endl;
    mymap.erase(mymap.begin(), mymap.find(12)); // 可以传迭代器或者键值,两个参数(迭代器)时删除符合参数范围左闭右开区间
    std::cout << mymap.size() << std::endl;
    mymap.count(1);                 // 返回当前键值的出现次数；
    auto p = mymap.lower_bound(12); // 返回第一个不小于该键值的迭代器
    auto s = mymap.upper_bound(24); // 返回第一个大于的迭代器
    // 泛型函数的使用
    for_each(p, s, [](auto pair) -> void
             { std::cout << pair.first << " " << pair.second << ' '; });
    std::cout << std::endl;
    for (const auto &t : mymap)
    {
        std::cout << t.first << ' ' << t.second << std::endl;
    }
    std::cout << 1 << std::endl;
    std::cout << "+++++++++++++++++++++" << std::endl;
    std::map<int, std::string> test;
    std::transform(p, s, std::inserter(test, test.end()), [](auto &pair) -> auto
                   { auto new_p=pair.second;
                      std::transform(new_p.begin(),new_p.end(),new_p.begin(),::toupper);
                    return std::make_pair(pair.first,new_p); });
    for (const auto &t : test)
    {
        std::cout << t.first << ' ' << t.second << std::endl;
    }
    键是自定义类型时，需提供比较函数，类：重载<; 其他 提供可调用对象,不可直接使用lambda函数(可以通过function间接使用))
    {
            class M
            {
                bool operator<(M a)
                    return __>a.__
            }
                std::map<type,type>
            struct comp
            {
                bool operator(type a，type b)
                   return a.__>b.__;
            }
                std::map<type,type,comp>

            std::function<bool(type a,type b)>comp =[](type a,type b)->bool {return a<b;}
            std::map<type,other_type,std::function<bool(type a,type b)>>a(comp)
            e.g. std::map<Person, std::string, decltype(comp)> qw(comp);
    }
*/
#include <iostream>
#include "AVLmap.h"
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>
using namespace std;

int main()
{

    return 0;
}