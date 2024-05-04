#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"   // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h"  // support for user defined types
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define log_trace(format,...)  Logger::getInstance().log(__FILE__,__LINE__,__FUNCTION__,spdlog::level::trace,format,##__VA_ARGS__)
#define log_debug(format,...)  Logger::getInstance().log(__FILE__,__LINE__,__FUNCTION__,spdlog::level::debug,format,##__VA_ARGS__)
#define log_info(format,...)   Logger::getInstance().log(__FILE__,__LINE__,__FUNCTION__,spdlog::level::info,format,##__VA_ARGS__)
#define log_warn(format,...)   Logger::getInstance().log(__FILE__,__LINE__,__FUNCTION__,spdlog::level::warn,format,##__VA_ARGS__)
#define log_error(format,...)  Logger::getInstance().log(__FILE__,__LINE__,__FUNCTION__,spdlog::level::err,format,##__VA_ARGS__)
#define log_fatal(format,...)  Logger::getInstance().log(__FILE__,__LINE__,__FUNCTION__,spdlog::level::critical,format,##__VA_ARGS__)

class Logger {
private:
    std::unique_ptr<spdlog::logger> logger;
public:
    Logger() {};
    ~Logger() {};
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // 禁止拷贝构造函数和赋值运算符
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void configure(spdlog::level::level_enum level,std::string tag,int maxSize,int maxFile) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/logfile", 1024 * 1024 * maxSize, maxFile);

        logger = std::make_unique<spdlog::logger>(spdlog::logger(tag, {console_sink, file_sink}));
        logger->set_level(level);  
        logger->set_pattern("%Y-%m-%d %H:%M:%S.%e [tid %t] [%^%L%$] %s:%# %v");

        spdlog::flush_every(std::chrono::seconds(3));
    }
    
    template <typename... Args>
    void log(const char *file, int line, const char *function, spdlog::level::level_enum level,spdlog::format_string_t<Args...> fmt,Args &&...args) {
       logger->log(spdlog::source_loc{file, line, function}, level, fmt , std::forward<Args>(args)...);
    }

    template <typename T>
    void log(const char *file, int line, const char *function, spdlog::level::level_enum level,const T &msg) {
       logger->log(spdlog::source_loc{file, line, function}, level, msg);
    }
};

#endif