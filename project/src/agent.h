#ifndef NANO_EDR_AGENT_H
#define NANO_EDR_AGENT_H
#include <cstddef>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>

#include "ring_buffer.h"
#include "event.h"
#include "event_list.h"
#include "event_source.h"
#include "rule_engine.h"
namespace nano_edr {
constexpr std::size_t kWindowSize = 256;
struct WindowEntry {
    std::string raw_ts;
    std::string type;
    std::string pid;
};
class Agent : public IEventHandler{
    public:
        explicit Agent(bool quiet);
        void HandleEvent(const Event& event) override;
        void OnSkipped() override;
        void OnBroken() override;
        void OnTick() override;
        void PrintSummary() const;
        std::size_t detections() const {
            return detections_;
        }
        std::size_t skipped() const {
            return skipped_;
        }

    private:
        void PrintContext() const;
        RingBuffer<WindowEntry, kWindowSize> window_;
        std::size_t total_events_ = 0;
        std::size_t detections_ = 0;
        std::size_t skipped_ = 0;
        std::size_t broken_ = 0;
        std::vector<std::pair<std::string, std::size_t>> counts_by_type_;
        RuleEngine engine_;
        bool quiet_ = false;
};
}  // namespace nano_edr
#endif
