#include "rules.h"
#include <string>
#include <print>
namespace nano_edr {
const char* SeverityName(Severity severity) {
    switch (severity) {
        case Severity::kLow: 
            return "low";
        case Severity::kMedium:
            return "medium";
        case Severity::kHigh:
            return "high";
        case Severity::kCritical:
            return "critical";
    }
    return "?";
}
std::size_t CheckRules(const Event& event, const Rule* rules, std::size_t count) {
    std::size_t fired = 0;
    for (std::size_t i = 0; i < count; ++i) {
        if (rules[i].check(event)) {
            std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(rules[i].severity), rules[i].id, event.ts, event.pid);
            ++fired;
        }
    }
    return fired;
}
}