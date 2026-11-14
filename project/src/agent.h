#ifndef NANO_EDR_AGENT_H
#define NANO_EDR_AGENT_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "alert_sink.h"
#include "entity_model.h"
#include "event.h"
#include "event_list.h"
#include "event_source.h"
#include "rule_engine.h"

namespace nano_edr {
class Agent : public IEventHandler {
 public:
    Agent(bool quiet, uint64_t dedup_period_ms);
    void Subscribe(AlertSink sink);  // Новое!
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    void PrintSummary() const;

 private:
    void PrintContextOnce();
    EventList window_;
    const EventNode* prev_ = nullptr;
    std::size_t total_events_ = 0;
    std::size_t detections_ = 0;
    std::size_t skipped_ = 0;
    std::size_t broken_ = 0;
    std::size_t suppressed_ = 0;
    bool context_printed_ = false;
    EntityModel model_;
    RuleEngine engine_;
    bool quiet_ = false;
    uint64_t dedup_period_ms_ = 0;
    std::vector<std::pair<std::string, std::size_t>> counts_by_type_;
};
}  // namespace nano_edr
#endif