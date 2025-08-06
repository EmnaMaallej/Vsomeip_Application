#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include "vsomeip_ids.hpp"

// Global application pointer for Client1
std::shared_ptr<vsomeip::application> app;

// Function to convert RPM to km/h (assuming wheel radius and gear ratios)
// This is a simplified conversion - you may need to adjust based on actual vehicle parameters
float convert_rpm_to_kmh(int rpm) {
    // Example conversion: assuming direct drive and wheel circumference
    // You can adjust these parameters based on your specific requirements
    const float wheel_radius_m = 0.3f;  // 30cm wheel radius
    const float gear_ratio = 3.5f;      // Final drive ratio
    
    // Calculate wheel RPM
    float wheel_rpm = rpm / gear_ratio;
    
    // Calculate wheel circumference in meters
    float wheel_circumference_m = 2 * M_PI * wheel_radius_m;
    
    // Calculate speed in m/min
    float speed_m_per_min = wheel_rpm * wheel_circumference_m;
    
    // Convert to km/h
    float speed_kmh = (speed_m_per_min * 60.0f) / 1000.0f;
    
    return speed_kmh;
}

void handle_speed_response(const std::shared_ptr<vsomeip::message>& response) {
    // Extract payload data from Client2
    auto payload = response->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
    
    if(len < sizeof(int)) {
        std::cerr << "[Client1] Received an invalid payload from Client2." << std::endl;
        return;
    }
    
    int rpm;
    std::memcpy(&rpm, data, sizeof(int));
    
    std::cout << "[Client1] Received RPM from Client2: " << rpm << std::endl;
    
    // Convert RPM to km/h
    float speed_kmh = convert_rpm_to_kmh(rpm);
    std::cout << "[Client1] Converted speed: " << speed_kmh << " km/h" << std::endl;
    
    // Send the converted speed to the Server
    auto speed_request = vsomeip::runtime::get()->create_request();
    speed_request->set_service(SERVICE_SPEEDVALUE);
    speed_request->set_instance(INSTANCE_SPEEDVALUE);
    speed_request->set_method(METHOD_SPEEDVALUE);
    
    // Create payload with converted speed
    auto speed_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> speed_data(sizeof(float));
    std::memcpy(speed_data.data(), &speed_kmh, sizeof(float));
    speed_payload->set_data(speed_data);
    speed_request->set_payload(speed_payload);
    
    // Send to server
    app->send(speed_request);
    std::cout << "[Client1] Sent converted speed to Server: " << speed_kmh << " km/h" << std::endl;
}

void handle_server_response(const std::shared_ptr<vsomeip::message>& response) {
    // Handle ACK from server
    auto payload = response->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
    
    std::string ack_msg(data, data + len);
    std::cout << "[Client1] Received from Server: " << ack_msg << std::endl;
}

void handle_speed_alert(const std::shared_ptr<vsomeip::message>& notification) {
    // Handle speed alert notification from server
    auto payload = notification->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
    
    std::string alert_msg(data, data + len);
    std::cout << "[Client1] *** SPEED ALERT RECEIVED: " << alert_msg << " ***" << std::endl;
}

int main() {
    // Create the vsomeip application for Client1
    app = vsomeip::runtime::get()->create_application("Client1");
    
    if (!app->init()) {
        std::cerr << "[Client1] Failed to initialize application." << std::endl;
        return -1;
    }
    
    // Register message handlers
    // For Client2 speed request response
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_response);
    
    // For Server ACK response
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_server_response);
    
    // For Server speed alert notification
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, handle_speed_alert);
    
    // Request services
    app->request_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);  // From Client2
    app->request_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);      // From Server
    
    // Subscribe to speed alert events
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_SPEEDALERT);
    app->request_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, eventgroups, vsomeip::event_type_e::ET_EVENT);
    app->subscribe(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT);
    
    // Start the application event loop in a separate thread
    std::thread vsomeip_thread([&](){ 
    app->start();
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Create a request message to ask Client2 for RPM
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
    
    // Keep the application running
    std::this_thread::sleep_for(std::chrono::seconds(10));

    //app->stop();
    //vsomeip_thread.join();
    
    return 0;
}
