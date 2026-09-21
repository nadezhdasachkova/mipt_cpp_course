#include "parse.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace nano_edr {

bool ParseFields(const std::string* text, std::vector<Field>* out) {
    if (text == nullptr || out == nullptr) return false;

    const std::string& s = *text;
    std::size_t i = 0;
    const std::size_t n = s.size();

    while (i < n) {
        while (i < n && (s[i] == ' ' || s[i] == '\t')) ++i;
        if (i == n) break;

        std::size_t keyStart = i;
        while (i < n && s[i] != '=' && s[i] != ' ' && s[i] != '\t') ++i;
        std::size_t keyEnd = i;

        if (keyEnd == keyStart) return false;
        if (i == n) return false;
        if (s[i] != '=') return false;

        ++i;

        std::string value;
        if (i < n && s[i] == '"') {
            ++i;
            while (i < n && s[i] != '"') {
                value.push_back(s[i]);
                ++i;
            }
            if (i == n) return false;
            ++i;
            if (i < n && s[i] != ' ' && s[i] != '\t') return false;
        } else {
            std::size_t valStart = i;
            while (i < n && s[i] != ' ' && s[i] != '\t') ++i;
            value.assign(s, valStart, i - valStart);
        }

        out->push_back(Field{ s.substr(keyStart, keyEnd - keyStart), std::move(value) });
    }

    return true;
}

bool IsBlankOrComment(const std::string* line) {
    if (line == nullptr) return true;
    std::size_t i = 0;
    while (i < line->size() && ((*line)[i] == ' ' || (*line)[i] == '\t')) ++i;
    if (i == line->size()) return true;
    char c = (*line)[i];
    return c == '#' || c == ';';
}

bool ParseEventLine(const std::string* line, Event* out) {
    if (line == nullptr || out == nullptr) return false;
    if (IsBlankOrComment(line)) return false;

    std::vector<Field> fields;
    if (!ParseFields(line, &fields)) return false;

    bool haveTs = false, haveType = false, havePid = false;
    for (const Field& f : fields) {
        if (!haveTs && f.key == "ts") { out->ts = f.value; haveTs = true; }
        else if (!haveType && f.key == "type") { out->type = f.value; haveType = true; }
        else if (!havePid && f.key == "pid") { out->pid = f.value; havePid = true; }
        else { out->fields.push_back(f); }
    }

    if (!haveTs || !haveType) return false;
    return true;
}

}  // namespace nano_edr
