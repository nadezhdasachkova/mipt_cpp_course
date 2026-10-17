#include "fields.h"

#include <cctype>
#include <charconv>
#include <stdexcept>
#include "text.h"
namespace nano_edr {
const std::string* FindField(const Event& event, const std::string& key) {
    if (key == "ts") {
        return &event.raw_ts();
    }
    if (key == "type") {
        return &event.type();
    }
    if (key == "pid") {
        return &event.pid();
    }
    for (const Field& f : event.fields()) {
        if (f.key == key) {
            return &f.value;
        }
    }
    return nullptr;
}
const std::string& GetRequiredField(const Event& event, const std::string& key) {
    const std::string* found = FindField(event, key);
    if (found == nullptr) {
        throw std::invalid_argument("обязательное поле отсутствует: " + key);
    }
    return *found;
}
bool GetIntField(const Event& event, const std::string& key, uint64_t* out) {
    if (out == nullptr) {
        return false;
    }
    const std::string* value = FindField(event, key);
    if (value == nullptr || value->empty()) {
        return false;
    }
    uint64_t n = 0;
    const char* first = value->data();
    const char* last = value->data() + value->size();
    auto [ptr, ec] = std::from_chars(first, last, n);
    if (ec != std::errc{} || ptr != last) {
        return false;
    }
    *out = n;
    return true;
}
uint64_t GetIntField(const Event& event, const std::string& key, uint64_t fallback) {
    uint64_t value = 0;
    return GetIntField(event, key, &value) ? value : fallback;
}
bool IsProcessStart(const Event& event) {
    return event.type() == "process_start";
}
bool IsFileWrite(const Event& event) {
    return event.type() == "file_write";
}
bool IsNetConnect(const Event& event) {
    return event.type() == "net_connect";
}
std::string NormalizePath(const std::string& path) {
    std::string result;
    result.reserve(path.size());
    for (char c : path) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    const std::string kTemp = "\\appdata\\local\\temp";
    std::size_t pos = result.find("%temp%");
    while (pos != std::string::npos) {
        result.replace(pos, 6, kTemp);
        pos = result.find("%temp%", pos + kTemp.size());
    }
    pos = result.find("%tmp%");
    while (pos != std::string::npos) {
        result.replace(pos, 5, kTemp);
        pos = result.find("%tmp%", pos + kTemp.size());
    }
    std::string out;
    out.reserve(result.size());
    for (char c : result) {
        if (c == '/') {
            c = '\\';
        }
        if (c == '\\' && !out.empty() && out.back() == '\\') {
            continue;
        }
        out.push_back(c);
    }
    return out;
}
bool PathEndsWith(const Event& event, const std::string& suffix) {
    const std::string* path = FindField(event, "path");
    if (path == nullptr) {
        return false;
    }
    const std::string np = NormalizePath(*path);
    const std::string ns = NormalizePath(suffix);
    if (np.size() < ns.size()) {
        return false;
    }
    return EndsWithIgnoreCase(NormalizePath(*path), NormalizePath(suffix));
}
bool CommandLineContains(const Event& event, const std::string& needle) {
    const std::string* cmdline = FindField(event, "cmdline");
    if (cmdline == nullptr) {
        return false;
    }
    return ContainsIgnoreCase(NormalizePath(*cmdline), NormalizePath(needle));
}
}  // namespace nano_edr