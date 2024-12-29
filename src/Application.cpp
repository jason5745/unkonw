#include "Logger.h"
#include "ControllerRouter.h"
#include "User.pb.h"


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

    auto method = ControllerRouter::getInstance()->getController("user")->method("login");
    if (method != nullptr) {
        std::string jsonStr = R"({"username": "1234","password": "4321"})";
        auto request = method->request();
        auto response = method->response();
        if (google::protobuf::json::JsonStringToMessage(jsonStr, request.get()).ok()) {
            method->invoke(request.get(),response.get());
            std::string output;

            google::protobuf::util::JsonPrintOptions options {0};
            options.always_print_fields_with_no_presence = true;
            options.preserve_proto_field_names = true;
            if (google::protobuf::json::MessageToJsonString(*response, &output, options).ok()) {
                std::cout << output << std::endl;
            }

        } else {
            log->warn("入参错误");
        }

    }
    return 0;
}