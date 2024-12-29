//
// Created by qq574 on 2024/12/26.
//

#ifndef GATEWAYAPPLICATION_SRC_LOGGER_H_
#define GATEWAYAPPLICATION_SRC_LOGGER_H_

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"   // support for loading levels from the environment variable

class Logger : public spdlog::logger {
private:
public:
    Logger(std::string name,int maxSize,int maxFile);
};
#endif //GATEWAYAPPLICATION_SRC_LOGGER_H_
