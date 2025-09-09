#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <csignal>
#include "vsomeip_ids.hpp"

// Global application pointer for Client1
std::shared_ptr<vsomeip::application> app;
bool running = true;

// Signal handler for graceful shutdown
void signal_handler(int signum) {
    std::cout << "[Client1] Received signal " << signum << ", stopping..." << std::endl;
    running = false;
    if (app) {
        app->stop();
    }
}

// Function to convert RPM to km/h
float convert_rpm_to_kmh(int rpm) {
    const float wheel_radius_m = 0.3f;  // 30cm wheel radius
    const float gear_ratio = 3.5f;      // Final drive ratio
    float wheel_rpm = rpm / gear_ratio;
    float wheel_circumference_m = 2 * M_PI * wheel_radius_m;
    float speed_m_per_min = wheel_rpm * wheel_circumference_m;
    float speed_kmh = (speed_m_per_min * 60.0f) / 1000.0f;
    return speed_kmh;
}

void handle_speed_response(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
   
    if (len < sizeof(int)) {
        std::cerr << "[Client1] Received invalid payload from Client2." << std::endl;
        return;
    }
   
    int rpm;
    std::memcpy(&rpm, data, sizeof(int));
    std::cout << "[Client1] Received RPM from Client2: " << rpm << std::endl;
   
    float speed_kmh = convert_rpm_to_kmh(rpm);
    std::cout << "[Client1] Converted speed: " << speed_kmh << " km/h" << std::endl;
   
    auto speed_request = vsomeip::runtime::get()->create_request();
    speed_request->set_service(SERVICE_SPEEDVALUE);
    speed_request->set_instance(INSTANCE_SPEEDVALUE);
    speed_request->set_method(METHOD_SPEEDVALUE);
   
    auto speed_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> speed_data(sizeof(float));
    std::memcpy(speed_data.data(), &speed_kmh, sizeof(float));
    speed_payload->set_data(speed_data);
    speed_request->set_payload(speed_payload);
   
    app->send(speed_request);
    std::cout << "[Client1] Sent converted speed to Server: " << speed_kmh << " km/h" << std::endl;
}

void handle_server_response(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
   
    std::string ack_msg(data, data + len);
    std::cout << "[Client1] Received from Server: " << ack_msg << std::endl;
}

void handle_speed_alert(const std::shared_ptr<vsomeip::message>& notification) {
    auto payload = notification->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
   
    std::string alert_msg(data, data + len);
    std::cout << "[Client1] Received notification: " << alert_msg << std::endl;
}

int main() {
    // Register signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create the vsomeip application for Client1
    app = vsomeip::runtime::get()->create_application("Client1");
   
    if (!app->init()) {
        std::cerr << "[Client1] Failed to initialize application." << std::endl;
        return -1;
    }
   
    // Register message handlers
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_response);
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_server_response);
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, handle_speed_alert);
   
    // Request services
    app->request_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);
    app->request_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);
   
    // Subscribe to speed alert events
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_SPEEDALERT);
    app->request_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, eventgroups, vsomeip::event_type_e::ET_EVENT);
    app->subscribe(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT);
   
    // Start the application event loop in a separate thread
    std::thread vsomeip_thread([&]() {
        app->start();
    });
   
    // Wait for initialization
    std::this_thread::sleep_for(std::chrono::seconds(3));
   
    while (running) {
        // Send request to Client2
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
   
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
   
    app->stop();
    vsomeip_thread.join();
   
    return 0;
}
