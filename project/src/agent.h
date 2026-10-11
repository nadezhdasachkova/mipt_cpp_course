#pragma once
#include <cstddef>
#include "event.h"
#include "event_list.h"
#include "os.h"
namespace nano_edr {
class Agent {
public: 
    explicit Agent(std::size_t window_size, bool quiet);
    void OnEvent(const Event& event);
    static void Trampoline(const os_event* ev, void* ctx) noexcept;
    std::size_t detections() const {
        return detections_;
    }
    std::size_t skipped() const {
        return skipped_;
    }
private:
    void PrintContext() const;
    EventList window_;
    const EventNode* prev_ = nullptr;
    std::size_t detections_ = 0;
    std::size_t skipped_ = 0;
    bool quiet_ = false;
};
}