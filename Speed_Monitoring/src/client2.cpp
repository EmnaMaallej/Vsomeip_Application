ip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <csignal>
#include <random>
#include "vsomeip_ids.hpp"

// Global application Client2
std::shared_ptr<vsomeip::application> app;
bool running = true;

// Signal handler for graceful shutdown
void signal_handler(int signum) {
    std::cout << "[Client2] Received signal " << signum << ", stopping..." << std::endl;
    running = false;
    if (app) {
        app->stop();
    }
}

void handle_speed_request(const std::shared_ptr<vsomeip::message>& request) {
    std::cout << "[Client2] Received speed request from Client1." << std::endl;
   
    // Simulate random RPM value
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1000, 10000);
    int rpm = dist(gen);
   
    // Prepare payload
    std::vector<vsomeip::byte_t> payload_data(sizeof(int));
    std::memcpy(payload_data.data(), &rpm, sizeof(int));
   
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(payload_data);
   
    // Create response message
    auto response = vsomeip::runtime::get()->create_response(request);
    response->set_message_type(vsomeip::message_type_e::MT_RESPONSE); // Explicitly set to RESPONSE
    response->set_payload(payload);
   
    // Send response
    app->send(response);
    std::cout << "[Client2] Sent RPM response: " << rpm << std::endl;
}

int main() {
    // Register signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create the vsomeip application for Client2
    app = vsomeip::runtime::get()->create_application("Client2");
   
    if (!app->init()) {
        std::cerr << "[Client2] Failed to initialize application." << std::endl;
        return -1;
    }
   
    // Register message handler
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_request);
   
    // Offer the service
    app->offer_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);
   
    // Start the event loop
    app->start();
   
    return 0;
}
