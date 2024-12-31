#include "Logger.h"
#include "AwaitHypertextService.h"
#include <chrono>

using namespace std::literals::chrono_literals;

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
    auto log = std::make_shared<Logger>("logs/test",1024,10);
    AwaitHypertextService await_hypertext_service(4);
    await_hypertext_service.start(18088);
    std::this_thread::sleep_for(1000s);
    return 0;
}