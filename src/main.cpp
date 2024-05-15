
#include "logger.h"
#include "boost_async_http_server.h"
#include "element/ringbuf.h"
#include "module_factory.h"

int main(int argc, char **argv) {
    long i = 0;
    Logger::getInstance().configure(spdlog::level::trace, "default", 1, 5);

    utils::boost::async::http::Server service(4);
    service.start(10086);
    sleep(10000);
    service.stop();
//

//    std::unique_ptr<module::Module> service = module::Factory::createModule("");
//    service->init("");
//    service->start();
//    std::unique_ptr<module::ModuleRequest> request = std::make_unique<module::ModuleRequest>();
//    while (true) {
//        while (service->pop(request)) {
//            GeneralService::Response response;
//            request->future_(std::move(response));
//        }
//        usleep(1);
//        if (++i == 0) {
//            break;
//        }
//    }
//    service->stop();
//    service->exit();
    return 0;
}

