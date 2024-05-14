#ifndef __MODULE_FACTORY_H_
#define __MODULE_FACTORY_H_

#include "module.h"
namespace module {
class Factory {
public:
    static std::unique_ptr<Module> createModule(std::string_view name);
};
}

#endif