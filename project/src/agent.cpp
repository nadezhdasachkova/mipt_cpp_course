#include "agent.h"
#include "agent_rules.h"
#include "parse.h"
#include "rules.h"
#include <print>
#include <stdexcept>
#include <string>
namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet)
    : window_(window_size), quiet_(quiet) {}

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
void Agent::OnEvent(const Event& event) {
    const std::size_t fired = CheckRules(event, AgentRules(), AgentRuleCount());

    if (fired > 0) {
        ++detections_;
        if (!quiet_) PrintContext();
    }
    const EventNode* old_tail = window_.back();
    window_.PushBack(event);
    prev_ = (window_.size() >= 2) ? old_tail : nullptr;
}
void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    try {
        auto* self = static_cast<Agent*>(ctx);
        if (self == nullptr || ev == nullptr) return;
        EventParts parts;
        parts.ts   = std::to_string(ev->ts);
        parts.type = ev->type ? ev->type : "";
        if (ev->pid != 0) {
            parts.pid = std::to_string(ev->pid);
        }
        for (std::size_t i = 0; i < ev->field_count; ++i) {
            const os_kv& kv = ev->fields[i];
            parts.fields.push_back(Field{
                kv.key   ? kv.key   : "",
                kv.value ? kv.value : ""
            });
        }
        try {
            Event event(parts);
            self->OnEvent(event);
        } catch (const std::exception&) {
            ++self->skipped_;
        }
    } catch (...) {
    }
}
}