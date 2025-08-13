#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include "vsomeip_ids.hpp"

std::shared_ptr<vsomeip::application> app;

void handle_speed_value_request(const std::shared_ptr<vsomeip::message>& request) {
    auto payload = request->get_payload();
    if (payload->get_length() < sizeof(float)) {
        std::cerr << "[Server] Invalid speed payload" << std::endl;
        return;
    }

    float speed_kmh;
    std::memcpy(&speed_kmh, payload->get_data(), sizeof(float));
    std::cout << "[Server] Received speed: " << speed_kmh << " km/h" << std::endl;

    // Send ACK
    auto response = vsomeip::runtime::get()->create_response(request);
    std::string ack = "ACK";
    auto ack_payload = vsomeip::runtime::get()->create_payload();
    ack_payload->set_data(std::vector<vsomeip::byte_t>(ack.begin(), ack.end()));
    response->set_payload(ack_payload);
    app->send(response);
    std::cout << "[Server] Sent ACK" << std::endl;

    // Send alert if > 100 km/h
    if (speed_kmh > 100.0f) {
        std::cout << "[Server] Sending speed alert" << std::endl;
        std::string alert = "SPEED_ALERT";
        auto alert_payload = vsomeip::runtime::get()->create_payload();
        alert_payload->set_data(std::vector<vsomeip::byte_t>(alert.begin(), alert.end()));
        app->notify(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, alert_payload);
    }
}

bool handle_subscription(vsomeip::service_t service,
                         const vsomeip_sec_client_t*,
                         const std::string &client_identifier,
                         bool subscribed) {
    std::cout << "[Server] Subscription change for service 0x"
              << std::hex << service << " from " << client_identifier
              << " -> " << (subscribed ? "Subscribed" : "Unsubscribed") << std::endl;
    return true; // Accept all
}

int main() {
    app = vsomeip::runtime::get()->create_application("Server");

    if (!app->init()) {
        std::cerr << "[Server] Init failed" << std::endl;
        return -1;
    }

    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_speed_value_request);
    app->register_subscription_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT, handle_subscription);

    // Offer service & event
    app->offer_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);
    std::set<vsomeip::eventgroup_t> eventgroups = { EVENTGROUP_SPEEDALERT };
    app->offer_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, eventgroups, vsomeip::event_type_e::ET_EVENT);

    std::cout << "[Server] Started and offering service 0x" << std::hex << SERVICE_SPEEDVALUE << std::endl;

    app->start();
    return 0;
}
