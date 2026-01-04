#include <variant>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <charconv>
#include <regex> //正则表达式
#include <iostream>
#include "print.h"

struct JSONObject;
using JSONDict = std::unordered_map<std::string, JSONObject>;
using JSONList = std::vector<JSONObject>;

struct JSONObject
{
    std::variant< // 相当于是一个更安全的变量类型，会根据数据类型，自行匹配变量类型
        std::nullptr_t,
        bool, int, double,
        std::string,
        JSONList, // vector实际存储的是指针，所以可以在内部使用，但单独的声明JSONObject就不行了，得是指针才可以
        // 或者说，JSONObject* a可以，用vector是用来容器来存储，是可以在运行动态变换的，它延缓了对对象的声明。（出现的问题在编译时）
        // 用unique_ptr<JSONObject>也是可以的，智能指针
        JSONDict // g++8.0版本，11以上才不报错;
        >
        inner;

    void do_print() const
    {
        printnl(inner);
    }

    template <class T>
    bool is() const
    {
        return std::holds_alternative<T>(inner); // 用于检查variant中是否含有这样的对象。
    }
    template <class T>
    T const &get() const
    {
        return std::get<T>(inner);
    }

    template <class T>
    T &get()
    {
        return std::get<T>(inner);
    }
};

template <class T>
std::optional<T> try_parse_num(std::string_view str) // std::optional<T>是可变返回值，根据实际返回值确定，nullopt是无值，它表示一个可能包含值（类型为 T）也可能不包含值的容器
{
    T value;
    auto res = std::from_chars(str.data(), str.data() + str.size(), value); // 函数原型中value的位置是引用，故字符串读出来后会存到value中
    if (res.ec == std::errc() && res.ptr == str.data() + str.size())        // ec是from_chars的一个返回值，指的是错误，没有的话为0，而std::errc()直接就是0
    {                                                                       // 后半段判断的是数字是否有垃圾值，比如对int类小数点，或者其他的字母等等；
        return value;
    }
    return std::nullopt;
}
char unescaped_char(char c)
{
    switch (c)
    {
    case 'n':
        return '\n';
        break;
    case 'r':
        return '\r';
        break;
    case '0':
        return '\0';
        break;
    case 't':
        return '\t';
        break;
    case 'v':
        return '\v';
        break;
    case 'f':
        return '\f';
        break;
    case 'b':
        return '\b';
        break;
    case 'a':
        return '\a';
        break;
    default:
        return c;
    }
}

std::pair<JSONObject, size_t> parse(std::string_view json) // 参数可以是const char* ，string，string_view本身是比较高效的，只有字符的首指针与长度，不涉及拷贝
{                                                          // string_view本身可以裁剪字符串，substr（a，b），从a+1位开始截取b长度，不给b默认末尾，其不会访问越界
    if (json.empty())
        return {JSONObject{std::nullptr_t{}}, 0};
    else if (size_t off = json.find_first_not_of(" \n\r\t\v\f\0"); off != 0 && off != json.npos) // 去除无效空格，换行等
    {
        auto [obj, eaten] = parse(json.substr(off));
        return {std::move(obj), eaten + off};
    }
    else if (json[0] >= '0' && json[0] <= '9' || json[0] == '+' || json[0] == '-') // 0-9,e,.,+-
    {

        std::regex num_re{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
        std::cmatch match;                                                            // std::cmath通过regex_search来得到符合正则表达式的例子，本身可视作数组，通过下标访问不同匹配结果。
        if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) // regex_search 子串匹配
        {
            std::string str = match.str();                           // 将match中的第一个匹配结果（默认是全部匹配的最长结果）转换为string了，如果要其他匹配结果 ，match.str(1);
            if (auto num = try_parse_num<int>(str); num.has_value()) // has_value()是对optional<T>是否有值的判断等价于bool类型
            {
                return {JSONObject{num.value()}, str.size()};
            }
            if (auto num = try_parse_num<double>(str); num.has_value())
            {
                return {JSONObject{num.value()}, str.size()};
            }
        }
    }
    else if (json[0] == '"')
    {
        std::string str;
        enum
        {
            Raw,
            Escaped
        } phase = Raw;
        size_t i;
        for (i = 1; i < json.size(); i++)
        {
            char ch = json[i];
            if (phase == Raw)
            {
                if (ch == '\\')
                {
                    phase = Escaped;
                }
                else if (ch == '"')
                {
                    i += 1;
                    break;
                }
                else
                {
                    str += ch;
                }
            }
            else if (phase == Escaped)
            {
                str += unescaped_char(ch);
                phase = Raw;
            }
        }
        return {JSONObject{std::move(str)}, i}; // move()函数，不涉及拷贝，直接将对象转移过去，更高效
    }
    else if (json[0] == '[')
    {
        std::vector<JSONObject> res;
        size_t i;
        for (i = 1; i < json.size();)
        {
            if (json[i] == ']')
            {
                i += 1;
                break;
            }

            auto [obj, eaten] = parse(json.substr(i));
            if (eaten == 0)
                break;
            res.push_back(std::move(obj));
            i += eaten;
            if (json[i] == ',')
                i += 1;
        }
        return {JSONObject{std::move(res)}, i};
    }
    else if (json[0] == '{')
    {
        std::unordered_map<std::string, JSONObject> res;
        size_t i;
        for (i = 1; i < json.size();)
        {
            if (json[i] == '}')
            {
                i += 1;
                break;
            }
            auto [keyobj, keyeaten] = parse(json.substr(i));
            if (keyeaten == 0)
            {
                i = 0;
                break;
            }
            i += keyeaten;
            if (!std::holds_alternative<std::string>(keyobj.inner))
            {
                i = 0;
                break;
            }
            if (json[i] == ':')
            {
                i += 1;
            }
            std::string key = std::move(std::get<std::string>(keyobj.inner));
            auto [valobj, valeaten] = parse(json.substr(i));
            if (valeaten == 0)
            {
                i = 0;
                break;
            }
            i += valeaten;
            res.insert_or_assign(std::move(key), std::move(valobj));
            if (json[i] == ',')
            {
                i += 1;
            }
        }
        return {JSONObject{std::move(res)}, i};
    }
    return {JSONObject{std::nullptr_t{}}, 0};
}
void dovisit(JSONObject const &school)
{
    std::visit([](auto const &school)
               {
            if constexpr (std::is_same_v<std::decay_t<decltype(school)>, JSONList>)
            {
                for (auto const &subschool : school)
                {
                    dovisit(subschool);
                }
            }
            else
            {
                print("I purchased my diploma from", school);
            } }, school.inner);
}
int main()
{
    std::string_view str = R"JSON({   
    "school":[985,211],
    "work":996}
    )JSON"; // R"()"表达式,按照"()"去匹配内容，所以R"JSON( )JSON",也是可以的
    // print(std::get<std::string>(parse(str3).inner).c_str());
    auto [obj, eaten] = parse(str);
    print(obj);
    auto const &dict = obj.get<JSONDict>(); // get<>中的是静态变量类型，动态时使用visit;
    print("The capitalist make me work in", dict.at("work").get<int>());
    auto const &school = dict.at("school");
    dovisit(school);
    return 0;
}
