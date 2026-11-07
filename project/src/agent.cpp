#include "agent.h"

#include <print>
#include <string>
#include <utility>
#include <vector>

#include "agent_rules.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(bool quiet) : engine_(BuildAgentRules()), quiet_(quiet) {}
void Agent::PrintContext() const {
    std::size_t n = window_.size();
    if (n >= 2) {
        const WindowEntry& entry = window_[n - 2];
        std::print("[CTX] -2: {} {} pid={}\n", entry.ts.ms, entry.type,
                   entry.pid);
    }
    if (n >= 1) {
        const WindowEntry& entry = window_[n - 1];
        std::print("[CTX] -1: {} {} pid={}\n", entry.ts.ms, entry.type,
                   entry.pid);
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
        std::print("[DETECT] {}  {}  ts={} pid={}\n",
                   SeverityName(rule->severity()), rule->id(), event.raw_ts(),
                   event.pid());
    }
    detections_ += fired.size();
    WindowEntry entry;
    entry.ts = event.ts();
    entry.type = event.type();
    entry.pid = event.pid();
    window_.PushBack(entry);
}
void Agent::OnSkipped() { ++skipped_; }
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
    std::print("[STAT] в окне {} из {} мест\n", window_.size(),
               window_.capacity());
    std::print("правила, детектов:\n");
    for (std::size_t i = 0; i < engine_.size(); ++i) {
        std::print("  {}  {}\n", engine_.rule(i).id(), engine_.rule(i).hits());
    }
}
}  // namespace nano_edr