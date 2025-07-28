#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include "vsomeip_ids.hpp"

// Global application pointer for Client1.
std::shared_ptr<vsomeip::application> app;

void handle_speed_response(const std::shared_ptr<vsomeip::message>& response) {
    // Extract payload data.
    auto payload = response->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
    
    if(len < sizeof(int)) {
        std::cerr << "[Client1] Received an invalid payload." << std::endl;
        return;
    }
    
    int rpm;
    std::memcpy(&rpm, data, sizeof(int));
    
    std::cout << "[Client1] Received RPM from Client2: " << rpm << std::endl;
    }

int main() {
    // Create the vsomeip application for Client1.
    app = vsomeip::runtime::get()->create_application("Client1");
    
    if (!app->init()) {
        std::cerr << "[Client1] Failed to initialize application." << std::endl;
        return -1;
    }
    
    // Register a message handler to process the response from Client2.
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_response);
    
    app->request_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);
    
    // Start the application event loop in a separate thread.
    std::thread t([&](){ app->start(); });
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Create a request message to ask for the RPM.
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_SPEEDREQUEST);
    request->set_instance(INSTANCE_SPEEDREQUEST);
    request->set_method(METHOD_SPEEDREQUEST);
    
    auto payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> empty_data;
    payload->set_data(empty_data);
    request->set_payload(payload);
    
    app->send(request);
    std::cout << "[Client1] Sent speed request to Client2." << std::endl;
    
    t.join();
    return 0;
}
