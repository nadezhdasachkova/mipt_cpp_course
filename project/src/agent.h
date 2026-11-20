#ifndef NANO_EDR_AGENT_H
#define NANO_EDR_AGENT_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "alert_sink.h"
#include "entity_model.h"
#include "event.h"
#include "ring_buffer.h"
#include "event_source.h"
#include "response.h"
#include "window_entry.h"
#include "rule_engine.h"


namespace nano_edr {
class Agent : public IEventHandler {
 public:
 void EnableResponse(OsHandle* os, bool dry_run, const std::string& quarantine_dir);
    static constexpr std::size_t kWindowCapacity = 256;
    Agent(bool quiet, uint64_t dedup_period_ms);
    void Subscribe(AlertSink sink);  
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    void PrintSummary() const;

 private:
    void PrintContextOnce();
    RingBuffer<WindowEntry, kWindowCapacity> window_;
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
    std::unique_ptr<ResponsePlanner> planner_;
    std::unique_ptr<Responder> responder_;
};
}  // namespace nano_edr
#endif