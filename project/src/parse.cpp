#include "parse.h"
#include <utility>
#include <vector>
namespace nano_edr {
namespace {
bool IsSpace(char c) {
    return c == ' ' || c == '\t';
}
}
bool IsBlankOrComment(const std::string& line) {
    std::size_t i = 0;
    while (i < line.size() && IsSpace(line[i])) {
        ++i;
    }
    if (i ==line.size()) {
        return true;
    }
    return line[i] == '#' || line[i] == ';';
}
bool ParseEventParts(const std::string& line, EventParts* out) {
    if (out == nullptr) {
        return false;
    }
    if (IsBlankOrComment(line)) {
        return false;
    }
    std::vector<Field> pairs;
    const std::size_t n = line.size();
    std::size_t i = 0;
    while (i < n) {
        while (i < n && IsSpace(line[i])) {
            ++i;
        }
        if (i == n) {
            break;
        }
        std::size_t keyStart = i;
        while (i < n && !IsSpace(line[i]) && line[i] != '=') {
            ++i;
        }
        std::size_t keyEnd = i;
        if (keyEnd == keyStart) {
            return false;
        }
        if (i == n) {
            return false;
        }
        if (line[i] != '=') {
            return false;
        }
        ++i;
        std::string value;
        if (i < n && line[i] == '"') {
            ++i;
            while (i < n && line[i] != '"') {
                value.push_back(line[i]);
                ++i;
            }
            if (i == n) {
                return false;
            }
            ++i;
            if (i < n && !IsSpace(line[i])) {
                return false;
            }
        } else {
            std::size_t valStart = i;
            while (i < n && !IsSpace(line[i])) {
                ++i;
            }
            value.assign(line, valStart, i - valStart);
        }
        pairs.push_back(Field{
            line.substr(keyStart, keyEnd - keyStart),
            std::move(value)
        });
    }
    bool haveTs = false;
    bool haveType = false;
    bool havePid = false;
    for (const Field& f : pairs) {
        if (!haveTs && f.key == "ts") {
            out->ts = f.value;
            haveTs = true;
        } else if (!haveType && f.key == "type") {
            out->type = f.value;
            haveType = true;
        } else if (!havePid && f.key == "pid") {
            out->pid = f.value;
            havePid = true;
        } else {
            out->fields.push_back(f);
        }
    }
    return true;
}
}
