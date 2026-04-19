#include "log.h"
#include "TaskManager.h"
#include "command.h"
#include <unordered_map>
int main()
{
    Logger::getInstance().log("程序启动");
    TaskManager manager;

    std::unordered_map<std::string, std::unique_ptr<CommandBase>> commandmap;
    commandmap["add"] = std::unique_ptr<CommandBase>(new AddCommand(manager));
    commandmap["delete"] = std::unique_ptr<CommandBase>(new DeleteCommand(manager));
    commandmap["list"] = std::unique_ptr<CommandBase>(new ListCommand(manager));
    commandmap["update"] = std::unique_ptr<CommandBase>(new UpdateCommand(manager));

    std::cout << "欢迎来到任务管理系统！\n";
    std::cout << "请输入命令(add, delete, list, update, exit)和参数" << std::endl;
    std::string input;
    while (true)
    {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        if (input.empty())
        {
            continue;
        }

        // 检查退出命令
        if (input == "exit" || input == "quit")
        {
            std::cout << "再见！\n";
            Logger::getInstance().log("程序退出");
            break;
        }

        size_t pos = input.find(' ');
        std::string commandName = input.substr(0, pos);
        std::string args = (pos != std::string::npos) ? input.substr(pos + 1) : "";
        auto it = commandmap.find(commandName);
        if (it != commandmap.end())
        {
            it->second->execute(args);
        }
        else
        {
            std::cout << "未知命令，请重新输入。\n";
        }
    }

    return 0;
}