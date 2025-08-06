#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include "vsomeip_ids.hpp"

// Global application pointer
std::shared_ptr<vsomeip::application> app;

void handle_speed_request(const std::shared_ptr<vsomeip::message>& request) {
    std::cout << "[Client2] Received speed request from Client1." << std::endl;
    
    int rpm = 5000;
    
    std::vector<vsomeip::byte_t> payload_data(sizeof(int));
    std::memcpy(payload_data.data(), &rpm, sizeof(int));
    
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(payload_data);
    
    auto response = vsomeip::runtime::get()->create_response(request);
    response->set_payload(payload);
    
    app->send(response);
    
    std::cout << "[Client2] Sent RPM response: " << rpm << std::endl;
}

int main() {
    app = vsomeip::runtime::get()->create_application("Client2");
    
    if (!app->init()) {
        std::cerr << "[Client2] Failed to initialize application. Error: " << app->get_error() << std::endl;
        return -1;
    }
    
    std::cout << "[Client2] Initialized, offering service..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for network setup
    
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_request);
    app->offer_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);
    
    std::cout << "[Client2] Service offered, starting event loop..." << std::endl;
    
    app->start();
    
    return 0;
}
