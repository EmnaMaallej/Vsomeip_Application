
#include <thread>
#include <csignal>
#include <functional> // For std::function
#include "vsomeip_ids.hpp"

// Global application pointer for Server
std::shared_ptr<vsomeip::application> app;
bool running = true;

// Signal handler for graceful shutdown
void signal_handler(int signum) {
    std::cout << "[Server] Received signal " << signum << ", stopping..." << std::endl;
    running = false;
    if (app) {
        app->stop();
    }
}

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
   
    // Send ACK response
    auto response = vsomeip::runtime::get()->create_response(request);
    response->set_message_type(vsomeip::message_type_e::MT_RESPONSE);
    auto ack_payload = vsomeip::runtime::get()->create_payload();
    std::string ack_msg = "ACK";
    std::vector<vsomeip::byte_t> ack_data(ack_msg.begin(), ack_msg.end());
    ack_payload->set_data(ack_data);
    response->set_payload(ack_payload);
   
    app->send(response);
    std::cout << "[Server] Sent ACK response." << std::endl;
   
    // Always send notification
    std::string alert_msg = (speed_kmh > 100.0f) ? "Alert" : "No Alert";
    std::cout << "[Server] Sending notification: " << alert_msg << std::endl;
   
    auto alert_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> alert_bytes(alert_msg.begin(), alert_msg.end());
    alert_payload->set_data(alert_bytes);
   
    // Send notification to all subscribers
    app->notify(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, alert_payload, true); // Force reliable
    std::cout << "[Server] Notification sent to subscribers." << std::endl;
}

// Subscription handler matching subscription_handler_ext_t
bool handle_subscription(vsomeip::service_t service,
                        vsomeip::instance_t instance,
                        vsomeip::eventgroup_t eventgroup,
                        const std::string& client_identifier,
                        bool is_subscribed) {
    std::cout << "[Server] Subscription change - Service: 0x" << std::hex << service
              << ", Instance: 0x" << instance
              << ", Eventgroup: 0x" << eventgroup
              << ", Client Identifier: " << client_identifier
              << ", Subscribed: " << (is_subscribed ? "true" : "false") << std::dec << std::endl;
   
    return true; // Accept all subscriptions
}

int main() {
    // Register signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create the vsomeip application for Server
    app = vsomeip::runtime::get()->create_application("Server");
   
    if (!app->init()) {
        std::cerr << "[Server] Failed to initialize application." << std::endl;
        return -1;
    }
   
    // Register message handler
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_speed_value_request);
   
    // Register subscription handler
    app->register_subscription_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT, handle_subscription);
   
    // Offer the service
    app->offer_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);
   
    // Offer the event
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_SPEEDALERT);
    app->offer_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, eventgroups,
                     vsomeip::event_type_e::ET_EVENT, std::chrono::milliseconds::zero(),
                     false, true, nullptr, vsomeip::reliability_type_e::RT_RELIABLE);
   
    std::cout << "[Server] Server started and offering svc_SpeedValue service." << std::endl;
   
    // Start the event loop
    app->start();
   
    return 0;
}
