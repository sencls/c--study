#pragma once

#include "task.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include "log.h"

class TaskManager
{
public:
    TaskManager() : nextId(1)
    {
        LoadTasks();
    }
    void AddTask(const std::string &description, const int &priority, const std::string &date)
    {
        Task task;
        task.id = nextId++;
        task.description = description;
        task.priority = priority;
        task.dueDate = date;
        tasks.push_back(task);
        Logger::getInstance().log("Task added: " + task.toString());
        SaveTasks();
    }
    bool RemoveTask(int id)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(), [&](const Task &task)
                               { return task.id == id; });
        if (it != tasks.end())
        {
            tasks.erase(it);
            Logger::getInstance().log("Task remove: " + std::to_string(id));
            SaveTasks();
            return true;
        }
        return false;
    }
    bool UpdateTask(int id, const std::string &description, int priority, const std::string &date)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(), [&](const Task &task)
                               { return task.id == id; });
        if (it != tasks.end())
        {
            it->description = description;
            it->priority = priority;
            it->dueDate = date;
            Logger::getInstance().log("Task updated: " + it->toString());
            SaveTasks();
            return true;
        }
        return false;
    }
    void ListTasks(int sortOption) const
    {
        std::vector<Task> sortedTask = tasks;
        switch (sortOption)
        {
        case 1:
            std::sort(sortedTask.begin(), sortedTask.end(), compareByPriority);
            break;
        case 2:
            std::sort(sortedTask.begin(), sortedTask.end(), compareByDueDate);
            break;
        default:
            break;
        }

        // 输出任务列表
        if (sortedTask.empty())
        {
            std::cout << "任务列表为空。\n";
            return;
        }

        for (const auto &task : sortedTask)
        {
            std::cout << task.toString() << std::endl;
        }
    }
    void SaveTasks()
    {
        std::ofstream outFile("task.txt");
        if (!outFile.is_open())
        {
            Logger::getInstance().log("Fail to open tasks file for writing");
            return;
        }

        for (const auto &t : tasks)
        {
            outFile << t.id << "," << t.description << ',' << t.priority << ',' << t.dueDate << std::endl;
        }
        outFile.close();
        Logger::getInstance().log("Tasks saved successfully");
    }
    void LoadTasks()
    {
        std::ifstream inFIle("task.txt");
        if (!inFIle.is_open())
        {
            Logger::getInstance().log("Failed to open file");
            return;
        }
        std::string line;
        while (std::getline(inFIle, line))
        {
            if (line.empty())
                continue;
            std::istringstream iss(line);
            Task task;
            char delimiter = ',';
            iss >> task.id >> delimiter;
            std::getline(iss, task.description, delimiter);
            iss >> task.priority >> delimiter;
            std::getline(iss, task.dueDate);
            tasks.push_back(task);
            if (task.id >= nextId)
                nextId = task.id + 1;
        }

        inFIle.close();
        Logger::getInstance().log("load successfully");
    }

private:
    std::vector<Task> tasks;
    int nextId;
    static bool compareByPriority(const Task &a, const Task &b)
    {
        return a.priority > b.priority;
    }
    static bool compareByDueDate(const Task &a, const Task &b)
    {
        return a.dueDate > b.dueDate;
    }
};