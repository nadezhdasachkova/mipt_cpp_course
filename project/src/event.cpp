#include "event.h"
#include <charconv>
#include <sstream>
#include <stdexcept>
namespace nano_edr {
Event::Event(const EventParts& parts) {
    if (parts.ts.empty()) {
        throw std::invalid_argument("Event: ts пуст");
    }
    if (parts.type.empty()) {
        throw std::invalid_argument("Event: type пуст");
    }
    uint64_t ms = 0;
    const char* first = parts.ts.data();
    const char* last = parts.ts.data() + parts.ts.size();
    auto [ptr, ec] = std::from_chars(first, last, ms);
    if (ec != std::errc{} || ptr != last) {
        throw std::invalid_argument("Event: ts не число: " + parts.ts);
    }
    ts_ = Timestamp{ms};
    raw_ts_ = parts.ts;
    type_ = parts.type;
    pid_ = parts.pid;
    fields_ = parts.fields;
}
std::string ToString(const Event& event) {
    std::string out;
    out.reserve(64);
    out += "ts=";
    out += event.raw_ts();
    out += " type=";
    out += event.type();
    if (!event.pid().empty()) {
        out += " pid=";
        out += event.pid();
    }
    for (const Field& f : event.fields()) {
        out += ' ';
        out += f.key;
        out += '=';
        out += f.value;
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const Event& event) {
    return out << ToString(event);
}
}