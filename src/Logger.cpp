//
// Created by qq574 on 2024/12/26.
//
#include <chrono>
#include "Logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

Logger::Logger(std::string name,int maxSize,int maxFile) : spdlog::logger(std::move(name)) {
    this->sinks().push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    this->sinks().push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/application.log", 1024 * 1024 * maxSize, maxFile));
    this->set_level(spdlog::level::info);
    this->set_pattern("%Y-%m-%d %H:%M:%S.%e [tid %t] [%^%L%$] %s:%# %v");
    spdlog::flush_every(std::chrono::seconds(3L));
}