// 异步日志系统

#include "log.h"

int main()
{
    try
    {
        Logger logger("log.txt");
        logger.log(loglevel::DEBUG, "Starting application");
        int use_id = 43;
        std::string action = "login";
        double duration = 3.5;
        std::string world = "World";
        logger.log(loglevel::INFO, "User {} perform {} in {} seconds", use_id, action, duration);
        logger.log(loglevel::WARN, "hello {}", world);
        logger.log(loglevel::ERROR, "this");
        logger.log(loglevel::INFO, "Multiple placeholders :{},{},", 1, 2, 3);
    }
    catch (const std::exception &a)
    {
        std::cerr << "error is " << a.what() << std::endl;
    }

    return 0;
}