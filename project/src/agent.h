#ifndef NANO_EDR_AGENT_H
#define NANO_EDR_AGENT_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "event.h"
#include "event_source.h"
#include "ring_buffer.h"
#include "rule_engine.h"
namespace nano_edr {
struct WindowEntry {
    Timestamp ts;
    std::string type;
    std::string pid;
};
class Agent : public IEventHandler {
 public:
    static constexpr std::size_t kWindowCapacity = 256;
    explicit Agent(bool quiet);
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    void PrintSummary() const;
    std::size_t detections() const { return detections_; }
    std::size_t skipped() const { return skipped_; }

 private:
    void PrintContext() const;
    RingBuffer<WindowEntry, kWindowCapacity> window_;
    std::size_t total_events_ = 0;
    std::size_t detections_ = 0;
    std::size_t skipped_ = 0;
    std::size_t broken_ = 0;
    std::vector<std::pair<std::string, std::size_t>> counts_by_type_;
    RuleEngine engine_;
    bool quiet_ = false;
};
}  // namespace nano_edr
#endif  // NANO_EDR_AGENT_H