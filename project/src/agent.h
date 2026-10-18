#ifndef NANO_EDR_AGENT_H
#define NANO_EDR_AGENT_H
#include <cstddef>
#include <vector>
#include <string>
#include <utility>


#include "event.h"
#include "event_list.h"
#include "os.h"
#include "rule.h"
namespace nano_edr {
class Agent {
 public:
    explicit Agent(std::size_t window_size, bool quiet);
    void HandleEvent(const Event& event);
    void HandleSkipped(const std::string& reason);
    void PrintSummary() const;
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
    std::size_t total_events_ = 0;
    std::size_t detections_ = 0;
    std::size_t skipped_ = 0;
    std::vector<std::pair<std::string, std::size_t>> counts_by_type_;
    std::vector<MatchRule> rules_;
    bool quiet_ = false;
};
}  // namespace nano_edr
#endif