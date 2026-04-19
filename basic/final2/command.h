#pragma once

#include <iostream>
#include <string>
#include <memory>
#include "TaskManager.h"
#include "log.h"

class CommandBase
{
public:
    virtual void execute(const std::string &args) = 0;
};

template <typename Deriver>
class Command : public CommandBase
{
public:
    void execute(const std::string &args)
    {
        static_cast<Deriver *>(this)->executeImpl(args);
    }
};

class AddCommand : public Command<AddCommand>
{
public:
    AddCommand(TaskManager &manager) : manager(manager) {}
    void executeImpl(const std::string &args)
    {
        size_t pos1 = args.find(',');
        size_t pos2 = args.find(',', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos)
        {
            std::cout << "参数格式错误,正确格式: 描述,优先级,截止日期\n";
            return;
        }

        std::string description = args.substr(0, pos1);
        int priority = std::stoi(args.substr(pos1 + 1, pos2 - pos1 - 1));
        std::string Duedate = args.substr(pos2 + 1);
        manager.AddTask(description, priority, Duedate);
        std::cout << "任务添加成功。\n";
    }

private:
    TaskManager &manager;
};

class DeleteCommand : public Command<DeleteCommand>
{
public:
    DeleteCommand(TaskManager &manager) : manager(manager) {}
    void executeImpl(const std::string &args)
    {
        size_t pos;
        int id = std::stoi(args, &pos);
        if (pos != args.length())
        {
            std::cout << "参数格式错误,正确格式: 任务ID\n";
            return;
        }
        if (manager.RemoveTask(id))
        {
            std::cout << "任务删除成功\n";
        }
        else
        {
            std::cout << "任务未找到，删除失败\n";
        }
    }

private:
    TaskManager &manager;
};

class ListCommand : public Command<ListCommand>
{
public:
    ListCommand(TaskManager &manager) : manager(manager) {}
    void executeImpl(const std::string &args)
    {
        try
        {
            int sortOption = 0;
            size_t pos;
            sortOption = std::stoi(args, &pos);
            if (pos != args.length())
            {
                std::cout << "参数格式错误,正确格式: 排序选项(0-3)\n";
                return;
            }

            std::cout << "任务列表：\n";
            manager.ListTasks(sortOption);
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().log("参数格式错误");
        }
    }

private:
    TaskManager &manager;
};

class UpdateCommand : public Command<UpdateCommand>
{
public:
    UpdateCommand(TaskManager &manager) : manager(manager) {}
    void executeImpl(const std::string &args)
    {
        size_t pos1 = args.find(',');
        size_t pos2 = args.find(',', pos1 + 1);
        size_t pos3 = args.find(',', pos2 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos)
        {
            std::cout << "参数格式错误,正确格式: 任务ID,描述,优先级,截止日期\n";
            return;
        }

        int id = std::stoi(args.substr(0, pos1));
        std::string description = args.substr(pos1 + 1, pos2 - pos1 - 1);
        int priority = std::stoi(args.substr(pos2 + 1, pos3 - pos2 - 1));
        std::string Duedate = args.substr(pos3 + 1);
        if (manager.UpdateTask(id, description, priority, Duedate))
        {
            std::cout << "任务更新成功。\n";
        }
        else
        {
            std::cout << "任务未找到，更新失败。\n";
        }
    }

private:
    TaskManager &manager;
};
