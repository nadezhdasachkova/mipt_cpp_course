#include "event.h"

#include <charconv>
#include <ostream>
#include <stdexcept>
namespace nano_edr {
EventType EventTypeFromString(const std::string& type) {
    if (type == "process_start") {
        return EventType::kProcessStart;
    }
    if (type == "process_end") {
        return EventType::kProcessEnd;
    }
    if (type == "file_create") {
        return EventType::kFileCreate;
    }
    if (type == "file_write") {
        return EventType::kFileWrite;
    }
    if (type == "file_delete") {
        return EventType::kFileDelete;
    }
    if (type == "file_move") {
        return EventType::kFileMove;
    }
    if (type == "net_connect") {
        return EventType::kNetConnect;
    }
    return EventType::kOther;
}
const char* EventTypeName(EventType type) {
    switch (type) {
        case EventType::kProcessStart: 
            return "process_start";
        case EventType::kProcessEnd:   
            return "process_end";
        case EventType::kFileCreate:   
            return "file_create";
        case EventType::kFileWrite:    
            return "file_write";
        case EventType::kFileDelete:  
             return "file_delete";
        case EventType::kFileMove:    
             return "file_move";
        case EventType::kNetConnect:  
             return "net_connect";
        case EventType::kOther:     
               return "other";
    }
    return "other";
}
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
    event_type_ = EventTypeFromString(parts.type); // Новое поле
    pid_ = parts.pid;
    fields_ = parts.fields;
}
std::string ToString(const Event& event) {
    std::string out;
    out += "ts="; out += event.raw_ts();
    out += " type="; out += event.type();
    if (!event.pid().empty()) {
        out += " pid="; out += event.pid();
    }
    for (const Field& f : event.fields()) {
        out += ' '; out += f.key; out += '='; out += f.value;
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const Event& event) {
    return out << ToString(event);
}
}  // namespace nano_edr