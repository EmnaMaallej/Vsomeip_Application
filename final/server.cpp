#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include "vsomeip_ids.hpp"

// Global application pointer for Server
std::shared_ptr<vsomeip::application> app;

void handle_speed_value_request(const std::shared_ptr<vsomeip::message>& request) {
    std::cout << "[Server] Received speed value request from Client1." << std::endl;
    
    auto payload = request->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
    
    if (len < sizeof(float)) {
        std::cerr << "[Server] Received invalid payload for speed value." << std::endl;
        return;
    }
    
    float speed_kmh;
    std::memcpy(&speed_kmh, data, sizeof(float));
    
    std::cout << "[Server] Received converted speed: " << speed_kmh << " km/h" << std::endl;
    
    auto response = vsomeip::runtime::get()->create_response(request);
    auto ack_payload = vsomeip::runtime::get()->create_payload();
    
    std::string ack_msg = "ACK";
    std::vector<vsomeip::byte_t> ack_data(ack_msg.begin(), ack_msg.end());
    ack_payload->set_data(ack_data);
    response->set_payload(ack_payload);
    
    app->send(response);
    std::cout << "[Server] Sent ACK response." << std::endl;
    
    bool send_alert = speed_kmh > 100.0f;
    
    if (send_alert) {
        std::cout << "[Server] Speed > 100 km/h, sending ALERT notification!" << std::endl;
        
        auto alert_message = vsomeip::runtime::get()->create_notification();
        alert_message->set_service(SERVICE_SPEEDVALUE);
        alert_message->set_instance(INSTANCE_SPEEDVALUE);
        alert_message->set_method(EVENT_SPEEDALERT);
        
        auto alert_payload = vsomeip::runtime::get()->create_payload();
        std::string alert_data = "SPEED_ALERT";
        std::vector<vsomeip::byte_t> alert_bytes(alert_data.begin(), alert_data.end());
        alert_payload->set_data(alert_bytes);
        alert_message->set_payload(alert_payload);
        
        app->notify(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, alert_payload);
        std::cout << "[Server] Alert notification sent to subscribers." << std::endl;
    } else {
        std::cout << "[Server] Speed <= 100 km/h, no alert needed." << std::endl;
    }
}

bool handle_subscription(vsomeip::service_t service, 
                         const vsomeip_sec_client_t *sec_client,
                         const std::string &client_identifier,
                         bool subscribed) {
    (void)sec_client; // Suppress unused parameter warning
    std::cout << "[Server] Subscription change - Service: 0x" << std::hex << service
              << ", Client ID: " << client_identifier
              << ", Subscribed: " << (subscribed ? "true" : "false") << std::dec << std::endl;
    
    return true;
}

int main() {
    app = vsomeip::runtime::get()->create_application("Server");
    
    if (!app->init()) {
        std::cerr << "[Server] Failed to initialize application." << std::endl;
        return -1;
    }
    
    std::cout << "[Server] Initialized, preparing to offer service..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for network setup
    
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_speed_value_request);
    app->register_subscription_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT, handle_subscription);
    
    app->offer_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);
    
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_SPEEDALERT);
    app->offer_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, eventgroups, vsomeip::event_type_e::ET_EVENT);
    
    std::cout << "[Server] Service and event offered, starting event loop..." << std::endl;
    
    std::thread vsomeip_thread([&]() { app->start(); });
    vsomeip_thread.detach();
    
    std::this_thread::sleep_for(std::chrono::seconds(30)); // Run for 30 seconds to observe traffic
    
    app->stop();
    return 0;
}
