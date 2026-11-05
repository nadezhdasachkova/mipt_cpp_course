#include "agent.h"

#include <vector>
#include <print>
#include <string>
#include <utility>

#include "agent_rules.h"
#include "rules.h"
namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet)
    : window_(window_size), quiet_(quiet) {RegisterAgentRules(&engine_);}

void Agent::PrintContext() const {
    if (prev_ != nullptr) {
        std::print("[CTX] -2: ts={} type={} pid={}\n",
                   prev_->event.raw_ts(),
                   prev_->event.type(),
                   prev_->event.pid());
    }
    const EventNode* last = window_.back();
    if (last != nullptr) {
        std::print("[CTX] -1: ts={} type={} pid={}\n",
                   last->event.raw_ts(),
                   last->event.type(),
                   last->event.pid());
    }
}
void Agent::HandleEvent(const Event& event) {
    ++total_events_;
    bool found = false;
    for (auto& [type, count] : counts_by_type_) {
        if (type == event.type()) {
            ++count;
            found = true;
            break;
        }
    }
    if (!found) {
        counts_by_type_.emplace_back(event.type(), 1);
    }
    std::vector<const IRule*> fired;
    engine_.ProcessEvent(event, &fired);
    if (!fired.empty() && !quiet_) {
        PrintContext();
    }
    for (const IRule* rule : fired) {
        std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(rule->severity()), rule->id(), event.raw_ts(), event.pid());
    }
    detections_ += fired.size();
    const EventNode* old_tail = window_.back();
    window_.PushBack(event);
    prev_ = (window_.size() >= 2) ? old_tail : nullptr;
}
void Agent::OnSkipped() {
    ++skipped_;
}
void Agent::OnBroken() {
    ++broken_;
    std::print(stderr, "пропущено событие: битая строка\n");
}
void Agent::OnTick() {}
void Agent::PrintSummary() const {
    if (quiet_) {
        return;
    }
    std::print("событий: {}\n", total_events_);
    for (const auto& [type, count] : counts_by_type_) {
        std::print(" {}: {}\n", type, count);
    }
    std::print("детектов: {}\n", detections_);
    std::print("пропущено: {}\n", skipped_);
    std::print("битых: {}\n", broken_);
    std::print("правила, детектов:\n");
    for (std::size_t i = 0; i < engine_.size(); ++i){
        std::print("  {}  {}\n", engine_.rule(i).id(), engine_.rule(i).hits());
    }
}
}  // namespace nano_edr