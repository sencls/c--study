#pragma once
#include <queue>
#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include <fstream>
#include <atomic>
#include <string>
#include <stdexcept>
#include <sstream>

// 单个参数->字符串，辅助函数
template <typename T>
std::string to_string(T &&arg)
{
    std::ostringstream oss;
    oss << std::forward<T>(arg);
    return oss.str();
}

class logQueue
{
public:
    void push(const std::string msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(msg);
        cond_var.notify_one();
    }
    bool pop(std::string &msg) // 防止虚假唤醒
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var.wait(lock, [this]()
                      { return !queue_.empty() || is_shutdown_; });
        /*同理，前者更modern
            while(queue_.empty())
        {
            cond_var.wait(lock);
        }
        */
        if (is_shutdown_ && queue_.empty())
        {
            return false;
        }

        msg = queue_.front();
        queue_.pop();
        return true;
    }
    void shutdown()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        is_shutdown_ = true;
        cond_var.notify_all();
    }

private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cond_var;
    bool is_shutdown_ = false;
};
enum class loglevel
{
    INFO,
    DEBUG,
    WARN,
    ERROR
};
class Logger
{
public:
    Logger(const std::string &filename) : log_file_(filename, std::ios::out | std::ios::app), exit_flag_(false)
    {
        if (!log_file_.is_open())
        {
            throw std::runtime_error("Fail to open log file");
        }

        worker_thread_ = std::thread([this]()
                                     {std::string msg;
                                        while(log_queue_.pop(msg))
                                        {
                                            log_file_<<msg<<std::endl;
                                        } });
    }
    ~Logger()
    {
        exit_flag_ = true;
        log_queue_.shutdown();
        if (worker_thread_.joinable())
        {
            worker_thread_.join();
        }

        if (log_file_.is_open())
        {
            log_file_.close();
        }
    }
    template <typename... Args>
    void log(loglevel level, const std::string &format, Args &&...args)
    {
        std::string level_str;
        switch (level)
        {
        case loglevel::INFO:
            level_str = "[INFO]";
            break;
        case loglevel::DEBUG:
            level_str = "[DEBUG]";
            break;
        case loglevel::WARN:
            level_str = "[WARN]";
            break;
        case loglevel::ERROR:
            level_str = "[ERROR]";
            break;
        }
        log_queue_.push(level_str + formatMessage(format, std::forward<Args>(args)...));
    }

private:
    std::string getCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        return std::string(buffer);
    }
    template <typename... Args>
    std::string formatMessage(const std::string &format, Args... args)
    {
        std::vector<std::string> arg_strings = {to_string(std::forward<Args>(args))...};
        std::ostringstream oss;
        size_t arg_index = 0;
        size_t pos = 0, placeholder = format.find("{}", pos);
        while (placeholder != std::string::npos)
        {
            oss << format.substr(pos, placeholder - pos);
            if (arg_index < arg_strings.size())
                oss << arg_strings[arg_index++];
            else
            {
                oss << "{}";
            }

            pos = placeholder + 2;
            placeholder = format.find("{}", pos);
        }
        oss << format.substr(pos);
        while (arg_index < arg_strings.size())
        {
            oss << arg_strings[arg_index++];
        }
        return " [" + getCurrentTime() + "] " + oss.str();
    }

    logQueue log_queue_;
    std::thread worker_thread_;
    std::ofstream log_file_;
    std::atomic<bool> exit_flag_;
};