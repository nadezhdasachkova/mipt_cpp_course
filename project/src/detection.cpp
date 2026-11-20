#include "detection.h"

#include <string>

#include "rules.h"
namespace nano_edr {
std::string ToString(const Detection& detection) {
    std::string out = "[DETECT] ";
    out += SeverityName(detection.severity);
    out += "  ";
    out += detection.rule;
    out += "  ts=";
    out += std::to_string(detection.ts.ms);
    out += " pid=";
    out += detection.pid;
    if (!detection.chain.empty()) {
        out += " chain=";
        out += detection.chain;
    }
    return out;
}
}  // namespace nano_edr