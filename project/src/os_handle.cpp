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
os_status OsHandle::Kill(uint32_t pid, uint64_t expected_start_time) {
    return os_process_kill(handle_, pid, expected_start_time);
}
os_status OsHandle::MoveFile(const std::string& from, const std::string& to) {
    return os_file_move(handle_, from.c_str(), to.c_str());
}
os_status OsHandle::DeleteFile(const std::string& path) {
    return os_file_delete(handle_, path.c_str());
}
os_status OsHandle::ProcessInfo(uint32_t pid, os_process_info* out) {
    return os_process_get_info(handle_, pid, out);
}
}  // namespace nano_edr