
#include "logger.h"
#include "boost_coroutine_http_server.h"
#include <memory>
#include <memory.h>
#include "element/ringbuf.h"
#include "module_factory.h"

int main(int argc, char **argv) {
	Logger::getInstance().configure(spdlog::level::trace, "default", 1, 5);

//	utils::boost::async::http::Server service(4);
//	service.start(10086);
//	sleep(10000);
//	service.stop();

//    service.start(10086);
//	sleep(10000);
//	service.stop();


	std::unique_ptr<module::Module> service = module::Factory::createModule("");
	service->init("");
	service->start();
	for (;;) {
		std::shared_ptr<module::ModuleRequest> request = service->pop();
		if (request != nullptr) {
			GeneralService::Response response;
			request->future_(std::move(response));
		}
	}
	service->stop();
	service->exit();
	return 0;
}

