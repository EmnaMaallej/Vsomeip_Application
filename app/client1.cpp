#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <cmath>
#include "vsomeip_ids.hpp"

std::shared_ptr<vsomeip::application> app;

float convert_rpm_to_kmh(int rpm) {
    const float wheel_radius_m = 0.3f;
    const float gear_ratio = 3.5f;
    float wheel_rpm = rpm / gear_ratio;
    float wheel_circumference_m = 2 * M_PI * wheel_radius_m;
    return (wheel_rpm * wheel_circumference_m * 60.0f) / 1000.0f;
}

void handle_speed_response(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    if (payload->get_length() < sizeof(int)) {
        std::cerr << "[Client1] Invalid RPM payload" << std::endl;
        return;
    }

    int rpm;
    std::memcpy(&rpm, payload->get_data(), sizeof(int));
    std::cout << "[Client1] Got RPM: " << rpm << std::endl;

    float speed_kmh = convert_rpm_to_kmh(rpm);
    std::cout << "[Client1] Converted speed: " << speed_kmh << " km/h" << std::endl;

    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_SPEEDVALUE);
    request->set_instance(INSTANCE_SPEEDVALUE);
    request->set_method(METHOD_SPEEDVALUE);

    auto speed_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data(sizeof(float));
    std::memcpy(data.data(), &speed_kmh, sizeof(float));
    speed_payload->set_data(data);
    request->set_payload(speed_payload);

    app->send(request);
    std::cout << "[Client1] Sent speed to Server" << std::endl;
}

void handle_server_response(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    std::string msg(payload->get_data(), payload->get_data() + payload->get_length());
    std::cout << "[Client1] Server says: " << msg << std::endl;
}

void handle_speed_alert(const std::shared_ptr<vsomeip::message>& notification) {
    auto payload = notification->get_payload();
    std::string msg(payload->get_data(), payload->get_data() + payload->get_length());
    std::cout << "[Client1] *** ALERT: " << msg << " ***" << std::endl;
}

int main() {
    app = vsomeip::runtime::get()->create_application("Client1");

    if (!app->init()) {
        std::cerr << "[Client1] Init failed" << std::endl;
        return -1;
    }

    // Handlers
    app->register_message_handler(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST, METHOD_SPEEDREQUEST, handle_speed_response);
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, METHOD_SPEEDVALUE, handle_server_response);
    app->register_message_handler(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, handle_speed_alert);

    // Request both services
    app->request_service(SERVICE_SPEEDREQUEST, INSTANCE_SPEEDREQUEST);
    app->request_service(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE);

    // Subscribe to alerts
    std::set<vsomeip::eventgroup_t> groups = { EVENTGROUP_SPEEDALERT };
    app->request_event(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENT_SPEEDALERT, groups, vsomeip::event_type_e::ET_EVENT);
    app->subscribe(SERVICE_SPEEDVALUE, INSTANCE_SPEEDVALUE, EVENTGROUP_SPEEDALERT);

    // Start SOME/IP loop in separate thread
    std::thread t([]() { app->start(); });

    // Give services time to become available
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Ask Client2 for RPM
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_SPEEDREQUEST);
    request->set_instance(INSTANCE_SPEEDREQUEST);
    request->set_method(METHOD_SPEEDREQUEST);

    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data({});
    request->set_payload(payload);

    app->send(request);
    std::cout << "[Client1] Sent RPM request to Client2" << std::endl;

    t.join();
    return 0;
}
