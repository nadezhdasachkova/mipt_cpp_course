#include "rule_engine.h"

#include <utility>

namespace nano_edr {
void RuleEngine::AddRule(std::unique_ptr<IRule> rule) {
    rules_.push_back(std::move(rule));
}
void RuleEngine::Subscribe(AlertSink sink) {
    sinks_.push_back(std::move(sink));
}
Detection RuleEngine::MakeDetection(const IRule& rule, const Event& event) const {
    Detection det;
    det.rule = rule.id();
    det.severity = rule.severity();
    det.ts = event.ts();
    det.pid = event.pid();
    det.actions = rule.actions();
    if (model_) {
        det.chain = model_->DescribeChain(event.pid());
    }
    return det;
}
std::size_t RuleEngine::ProcessEvent(const Event& event, std::vector<const IRule*>* fired) {
    std::size_t count = 0;
    for (const auto& rule : rules_) {
        if (rule->Check(event)) {
            ++count;
            if (fired) {
                fired->push_back(rule.get());
            }
            Detection det = MakeDetection(*rule, event);
            for (const auto& sink : sinks_) {
                sink(det);
            }
        }
    }
    return count;
}
}  // namespace nano_edr