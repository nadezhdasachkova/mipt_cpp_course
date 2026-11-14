#include "rule.h"

namespace nano_edr {
RuleBase::RuleBase(const std::string& id, Severity severity)
    : id_(id), severity_(severity) {}

void RuleBase::AddAction(const std::string& action) {
    actions_.push_back(action);
}
MatchRule::MatchRule(const std::string& id, Severity severity)
    : RuleBase(id, severity) {}
void MatchRule::AddCondition(std::unique_ptr<ICondition> condition) {
    conditions_.push_back(std::move(condition));
}
bool MatchRule::Check(const Event& event) {
    for (const auto& cond : conditions_) {
        if (!cond->Matches(event)) return false;
    }
    CountHit();
    return true;
}
SequenceRule::SequenceRule(const std::string& id, Severity severity, uint64_t window_ms)
    : RuleBase(id, severity), window_ms_(window_ms) {}
void SequenceRule::SetFirst(std::unique_ptr<ICondition> cond) { first_ = std::move(cond); }
void SequenceRule::SetSecond(std::unique_ptr<ICondition> cond) { second_ = std::move(cond); }
bool SequenceRule::Linked(const Pending& first, const Event& second) const {
    if (child_of_prev_) {
        if (!model_) return false;
        if (first.pid == second.pid()) return false;  // не может быть сам себе предком
        auto chain = model_->GetAncestorChain(second.pid());
        for (const auto& pid : chain) {
            if (pid == first.pid) return true;
        }
        return false;
    }
    if (same_pid_) return first.pid == second.pid();
    return true;
}
bool SequenceRule::Check(const Event& event) {
    if (!first_ || !second_) return false;
    uint64_t now = event.ts().ms;
    for (auto it = pending_.begin(); it != pending_.end();) {
        if (now > it->ts_ms && now - it->ts_ms > window_ms_) {
            it = pending_.erase(it);
        } else {
            ++it;
        }
    }
    if (second_->Matches(event)) {
        for (auto it = pending_.begin(); it != pending_.end(); ++it) {
            if (Linked(*it, event)) {
                pending_.erase(it);
                CountHit();
                return true;
            }
        }
    }
    if (first_->Matches(event)) {
        pending_.push_back({now, event.pid()});
    }
    return false;
}
LambdaRule::LambdaRule(const std::string& id, Severity severity, Predicate predicate)
    : RuleBase(id, severity), predicate_(std::move(predicate)) {}

bool LambdaRule::Check(const Event& event) {
    if (predicate_ && predicate_(event)) {
        CountHit();
        return true;
    }
    return false;
}
}  // namespace nano_edr