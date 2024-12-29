#include "Logger.h"
#include "Router.h"

class GatewayApplication
{
private:
    /* data */
public:
    GatewayApplication();
    ~GatewayApplication();
};

GatewayApplication::GatewayApplication() = default;
GatewayApplication::~GatewayApplication() = default;

int main(int argc, char **argv) {
    Router::getInstance()->getController("user")->invokeMethod("login", nullptr, nullptr);
    Router::getInstance()->getController("user")->invokeMethod("logout", nullptr, nullptr);

    auto instance = std::make_shared<Logger>("logs/test",1024,10);
    instance->warn("1234");
    return 0;
}