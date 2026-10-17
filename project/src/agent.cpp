#include "agent.h"

#include <cstddef>
#include <exception>
#include <print>
#include <string>
#include <utility>

#include "agent_rules.h"
#include "parse.h"
#include "rules.h"
namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet)
    : window_(window_size), rules_(BuildAgentRules()), quiet_(quiet) {}

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
    std::size_t fired = 0;
    for (MatchRule& rule : rules_) {
        if (rule.Check(event)) {
            std::print("[DETECT] {}  {}  ts={} pid={}\n",
                       SeverityName(rule.severity()),
                       rule.id(),
                       event.raw_ts(),
                       event.pid());
            ++fired;
        }
    }
    detections_ += fired;
    if (fired > 0 && !quiet_) {
        PrintContext();
    }
    const EventNode* old_tail = window_.back();
    window_.PushBack(event);
    prev_ = (window_.size() >= 2) ? old_tail : nullptr;
}
void Agent::HandleSkipped(const std::string& reason) {
    ++skipped_;
    std::print(stderr, "пропущено событие: {}\n", reason);
}
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
    std::print("правила, детектов:\n");
    for (const MatchRule& rule : rules_) {
        std::print("  {}  {}\n", rule.id(), rule.hits());
    }
}
void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    auto* self = static_cast<Agent*>(ctx);
    if (self == nullptr || ev == nullptr) {
        return;
    }
    try {
        EventParts parts;
        parts.ts = std::to_string(ev->ts);
        parts.type = ev->type ? ev->type : "";
        if (ev->pid != 0) {
            parts.pid = std::to_string(ev->pid);
        }
        for (std::size_t i = 0; i < ev->field_count; ++i) {
            const os_kv& kv = ev->fields[i];
            parts.fields.push_back(Field{
                kv.key ?kv.key : "",
                kv.value ? kv.value : ""
            });
        }
        Event event(parts);
        self->HandleEvent(event);
    } catch (const std::exception& e) {
        self->HandleSkipped(e.what());
    } catch (...) {
        self->HandleSkipped("неизвестная ошибка");
    }
}
}  // namespace nano_edr