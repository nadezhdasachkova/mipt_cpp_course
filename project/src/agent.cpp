#include "agent.h"

#include <print>
#include <string>
#include <utility>
#include <vector>

#include "agent_rules.h"
#include "dedup_sink.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(bool quiet, uint64_t dedup_period_ms)
    : quiet_(quiet), dedup_period_ms_(dedup_period_ms) {
    engine_ = BuildAgentRules(&model_);
    engine_.SetModel(&model_);
}
void Agent::Subscribe(AlertSink sink) {
    FanOutSink all;
    all.Add([this](const Detection&) { PrintContextOnce(); });
    all.Add(std::move(sink));
    engine_.Subscribe(DedupSink(std::move(all), dedup_period_ms_, &suppressed_));
}
void Agent::PrintContextOnce() {
    if (quiet_ || context_printed_) {
        return;
    }
    context_printed_ = true;

    if (window_.size() >= 2) {
        const WindowEntry& prev = window_[window_.size() - 2];
        std::print("[CTX] -2: {} {} pid={}\n", prev.ts.ms, prev.type, prev.pid);
    }
    if (window_.size() >= 1) {
        const WindowEntry& last = window_.back();
        std::print("[CTX] -1: {} {} pid={}\n", last.ts.ms, last.type, last.pid);
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
    if (!found) counts_by_type_.emplace_back(event.type(), 1);

    model_.Observe(event);
    context_printed_ = false;

    std::vector<const IRule*> fired;
    engine_.ProcessEvent(event, &fired);
    detections_ += fired.size();

    window_.PushBack({event.ts(), event.type(), event.pid()});
}
void Agent::OnSkipped() { ++skipped_; }
void Agent::OnBroken() {
    ++broken_;
    std::print(stderr, "пропущено событие: битая строка\n");
}
void Agent::OnTick() {}
void Agent::PrintSummary() const {
    if (quiet_) return;
    std::print("событий: {}\n", total_events_);
    for (const auto& [type, count] : counts_by_type_) {
        std::print(" {}: {}\n", type, count);
    }
    std::print("детектов: {}\n", detections_);
    std::print("пропущено: {}\n", skipped_);
    std::print("битых: {}\n", broken_);
    std::print("[STAT] подавлено дубликатов {} (срок {} мс)\n", suppressed_, dedup_period_ms_);
    std::print("[STAT] в модели процессов {}, файлов {}\n", model_.process_count(), model_.file_count());
    std::print("правила, детектов:\n");
    for (std::size_t i = 0; i < engine_.size(); ++i) {
        std::print("  {}  {}\n", engine_.rule(i).id(), engine_.rule(i).hits());
    }
}
}  // namespace nano_edr