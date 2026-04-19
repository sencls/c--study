#pragma once

#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <chrono>

class Logger
{
public:
    static Logger &getInstance()
    {
        static Logger logger;
        return logger;
    }
    Logger(const Logger &other) = delete;
    Logger &operator=(const Logger &other) = delete;
    void log(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (logFile.is_open())
        {
            auto now = std::chrono::system_clock::now();
            auto now_time = std::chrono::system_clock::to_time_t(now);
            char buffer[100];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
            logFile << std::string(buffer) << ": " << message << std::endl;
        }
    }
    ~Logger()
    {
        if (logFile.is_open())
            logFile.close();
    }

private:
    Logger()
    {
        logFile.open("log.txt", std::ios::app);
        if (!logFile.is_open())
        {
            throw std::runtime_error("open file fail");
        }
    }
    std::ofstream logFile;
    std::mutex mutex;
};
