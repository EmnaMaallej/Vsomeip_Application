#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include "vsomeip_ids.hpp"

std::shared_ptr<vsomeip::application> app;

void handle_speed_request(const std::shared_ptr<vsomeip::message>& request) {
    std::cout << "[Client2] Received request" << std::endl;

    int rpm = 5000;
    auto payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data(sizeof(int));
    std::memcpy(data.data(), &rpm, sizeof(int));
    payload->set_data(data);

    auto response = vsomeip::runtime::get()->create_response(request);
    response->set_payload(payload);
    app->send(response);

    std::cout << "[Client2] Sent RPM: " << rpm << std::endl;
}

int main() {
    app = vsomeip::runtime::get()->create_application("Client2");

    if (!app->init()) {
        std::cerr << "[Client2] Init failed" << std::endl;
        return -1;
    }

    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_request);
    app->offer_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);

    std::cout << "[Client2] Offering service 0x" << std::hex << SERVICE_SPEEDREQUEST << std::endl;

    app->start();
    return 0;
}
