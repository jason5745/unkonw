#include "module_factory.h"
#include "http_rpc_module.h"

namespace module {
std::unique_ptr<Module> Factory::createModule(std::string_view name) {
    std::unique_ptr<Module> p = std::make_unique<HttpRpcModule>();
    return p;
}
}
