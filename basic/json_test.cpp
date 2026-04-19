#include <json/json.h>
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    // 构造 JSON 对象
    Json::Value root;
    root["name"] = "Rem";
    root["role"] = "Maid Engineer";
    root["age"] = 17;
    root["skills"].append("C++");
    root["skills"].append("Debug");
    root["skills"].append("Cooking");

    // 序列化为字符串
    Json::StreamWriterBuilder writer;
    std::string output = Json::writeString(writer, root);
    std::cout << "=== 序列化输出 ===" << std::endl;
    std::cout << output << std::endl;

    // 反序列化
    Json::Value parsed;
    Json::CharReaderBuilder reader;
    std::istringstream iss(output);
    std::string errors;
    if (Json::parseFromStream(reader, iss, &parsed, &errors)) {
        std::cout << "=== 反序列化成功 ===" << std::endl;
        std::cout << "name: " << parsed["name"].asString() << std::endl;
        std::cout << "age: " << parsed["age"].asInt() << std::endl;
        std::cout << "skills[0]: " << parsed["skills"][0].asString() << std::endl;
    } else {
        std::cerr << "解析错误: " << errors << std::endl;
    }

    // 文件读写测试
    std::ofstream ofs("test.json");
    ofs << output;
    ofs.close();

    std::ifstream ifs("test.json");
    Json::Value fileParsed;
    if (Json::parseFromStream(reader, ifs, &fileParsed, &errors)) {
        std::cout << "=== 文件读取成功 ===" << std::endl;
        std::cout << "role: " << fileParsed["role"].asString() << std::endl;
    }

    std::cout << "jsoncpp 配置成功！" << std::endl;
    return 0;
}
