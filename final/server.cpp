#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include <set>
#include "vsomeip_ids.hpp"

// Global application pointer for Server
std::shared_ptr<vsomeip::application> app;
std::set<vsomeip::client_t> subscribers_;

void handle_speed_value_request(const std::shared_ptr<vsomeip::message>& request) {
    std::cout << "[Server] Received speed value request from Client1." << std::endl;
    
    // Extract the converted speed (km/h) from Client1
    auto payload = request->get_payload();
    auto data = payload->get_data();
    size_t len = payload->get_length();
    
    if(len < sizeof(float)) {
        std::cerr << "[Server] Received invalid payload for speed value." << std::endl;
        return;
    }
    
    float speed_kmh;
    std::memcpy(&speed_kmh, data, sizeof(float));
    
    std::cout << "[Server] Received converted speed: " << speed_kmh << " km/h" << std::endl;
    
    // Send ACK response
    auto response = vsomeip::runtime::get()->create_response(request);
    auto ack_payload = vsomeip::runtime::get()->create_payload();
    
    // Simple ACK message
    std::string ack_msg = "ACK";
    std::vector<vsomeip::byte_t> ack_data(ack_msg.begin(), ack_msg.end());
    ack_payload->set_data(ack_data);
    response->set_payload(ack_payload);
    
    app->send(response);
    std::cout << "[Server] Sent ACK response." << std::endl;
    
    // Analyze speed and send alert notification if needed
    bool send_alert = speed_kmh > 100.0f;
    
    if (send_alert) {
        std::cout << "[Server] Speed > 100 km/h, sending ALERT notification!" << std::endl;
        
        // Create alert notification
        auto alert_message = vsomeip::runtime::get()->create_notification();
        alert_message->set_service(SERVICE_SPEEDVALUE);
        alert_message->set_instance(INSTANCE_SPEEDVALUE);
        alert_message->set_method(EVENT_SPEEDALERT);
        
        // Create alert payload
        auto alert_payload = vsomeip::runtime::get()->create_payload();
        std::string alert_data = "SPEED_ALERT";
        std::vector<vsomeip::byte_t> alert_bytes(alert_data.begin(), alert_data.end());
        alert_payload->set_data(alert_bytes);
        alert_message->set_payload(alert_payload);
        
        // Send notification to all subscribers
        app->notify(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, alert_payload);
        std::cout << "[Server] Alert notification sent to subscribers." << std::endl;
    } else {
        std::cout << "[Server] Speed <= 100 km/h, no alert needed." << std::endl;
    }
}

void handle_subscription(vsomeip::service_t service, vsomeip::instance_t instance,
                        vsomeip::eventgroup_t eventgroup, vsomeip::client_t client, bool subscribed) {
    std::cout << "[Server] Subscription change - Service: 0x" << std::hex << service 
              << ", Instance: 0x" << instance << ", Client: 0x" << client 
              << ", Subscribed: " << (subscribed ? "true" : "false") << std::dec << std::endl;
    
    if (subscribed) {
        subscribers_.insert(client);
    } else {
        subscribers_.erase(client);
    }
}

int main() {
    // Create the vsomeip application for Server
    app = vsomeip::runtime::get()->create_application("Server");
    
    if (!app->init()) {
        std::cerr << "[Server] Failed to initialize application." << std::endl;
        return -1;
    }
    
    // Register message handler for speed value service
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_speed_value_request);
    
    // Register subscription handler for event notifications
    app->register_subscription_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT,
        [](vsomeip::service_t service, vsomeip::instance_t instance, vsomeip::eventgroup_t eventgroup,
           vsomeip::client_t client, bool subscribed) {
            handle_subscription(service, instance, eventgroup, client, subscribed);
        });
    
    // Offer the speed value service
    app->offer_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);
    
    // Offer the event
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_SPEEDALERT);
    app->offer_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, eventgroups, vsomeip::event_type_e::ET_EVENT);
    
    std::cout << "[Server] Server started and offering svc_SpeedValue service." << std::endl;
    
    // Start the event loop (this will block indefinitely)
    app->start();
    
    return 0;
}
