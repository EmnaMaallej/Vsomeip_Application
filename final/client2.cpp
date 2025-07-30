#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include "vsomeip_ids.hpp"

// Global application pointer
std::shared_ptr<vsomeip::application> app;

void handle_speed_request(const std::shared_ptr<vsomeip::message>& request) {
    std::cout << "[Client2] Received speed request from Client1." << std::endl;
    
    // For example, let’s say Client2 always responds with rpm = 2500.
    int rpm = 2500;
    
    // Prepare a byte vector with the integer.
    std::vector<vsomeip::byte_t> payload_data(sizeof(int));
    std::memcpy(payload_data.data(), &rpm, sizeof(int));
    
    // Create a payload.
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(payload_data);
    
    // Create a response message to the incoming request.
    auto response = vsomeip::runtime::get()->create_response(request);
    response->set_payload(payload);
    
    // Send response.
    app->send(response);
    
    std::cout << "[Client2] Sent RPM response: " << rpm << std::endl;
}

int main() {
    // Create the vsomeip application for Client2.
    app = vsomeip::runtime::get()->create_application("Client2");
    
    if (!app->init()) {
        std::cerr << "[Client2] Failed to initialize application." << std::endl;
        return -1;
    }
    
    // Register a message handler for requests on our service, instance, method.
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_request);
    
    // Offer the service (Client2 is the provider for svc_SpeedRequest).
    app->offer_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);
    
    // Start the event loop (this will block indefinitely).
    app->start();
    
    return 0;
}
