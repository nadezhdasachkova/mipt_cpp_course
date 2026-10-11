#include "os_handle.h"
#include <stdexcept>
namespace nano_edr {
OsHandle::OsHandle(const std::string& config_path) {
    os_status s = os_init(config_path.c_str(), &handle_);
    if (s != OS_OK) {
        throw std::runtime_error("os_init: " + config_path + ": " + os_status_str(s));
    }
}
OsHandle::~OsHandle() {
    if (handle_ != nullptr) {
        os_free(handle_);
        handle_ = nullptr;
    }
}
void OsHandle::Subscribe(os_event_cb callback, void* context) {
    os_status s = os_event_subscribe(handle_, callback, context);
    if (s != OS_OK) {
        throw std::runtime_error(std::string("os_event_subscribe: ") + os_status_str(s));
    }
}
void OsHandle::Start() {
    os_status s = os_start(handle_);
    if (s != OS_OK) {
        throw std::runtime_error(std::string("os_start: ") + os_status_str(s));
    }
}
os_status OsHandle::Wait(uint32_t timeout_ms) {
    return os_wait(handle_, timeout_ms);
}
void OsHandle::Stop() {
    os_stop(handle_);
}
}