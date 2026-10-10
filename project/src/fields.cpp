#include <cstdint>
#include <string>
#include <stdexcept>
#include <charconv>

#include "event.h"
namespace nano_edr {
const std::string* FindField(const Event& event, const std::string& key) {
    for (const auto& field : event.fields) {
        if (field.key == key) {
            return &field.value;
        }
    }
    return nullptr;
}
const std::string& GetRequiredField(const Event& event, const std::string& key) {
    const std::string* found = FindField(event, key);
    if (found == nullptr) {
        throw std::invalid_argument("required field missing: " + key);
    }
    return *found;
}
bool GetIntField(const Event& event, const std::string& key, uint64_t* out) {
    const std::string* value = FindField(event, key);
    if (value == nullptr || value -> empty()) {
        return false;
    }
    if (!out) {
        return false;
    }
    uint64_t n = 0;
    const auto& s = *value;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), n);
    if (ec == std::errc() && ptr == s.size() + s.data()) {
        *out = n;
        return true;
    }
    return false;
}
uint64_t GetIntField(const Event& event, const std::string& key, uint64_t fallback) {
    uint64_t value = 0;
    if (GetIntField(event, key, &value)) {
        return value;
    }
    return fallback;
}
bool IsProcessStart(const Event& event) {
    return event.type == "process_start";
}
bool IsFileWrite(const Event& event) {
    return event.type == "file_write";
}
bool IsNetConnect(const Event& event) {
    return event.type == "net_connect";
}
std::string NormalizePath(const std::string& path) {
    std::string result;
    result.reserve(path.size());
    for (char c : path) {
        result.push_back(static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))));
    }

    const std::string kTemp = "\\appdata\\local\\temp";

    std::size_t pos = result.find("%temp%");
    if (pos != std::string::npos) {
        result.replace(pos, 6, kTemp);
    }
    pos = result.find("%tmp%");
    if (pos != std::string::npos) {
        result.replace(pos, 5, kTemp);
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
    const std::string normalizedPath = NormalizePath(*path);
    const std::string normalizedSuffix = NormalizePath(suffix);
    if (normalizedPath.size() < normalizedSuffix.size()) {
        return false;
    }
    return normalizedPath.compare(normalizedPath.size() - normalizedSuffix.size(),normalizedSuffix.size(), normalizedSuffix) == 0;
}
bool CommandLineContains(const Event& event, const std::string& needle) {
    const std::string* cmdline = FindField(event, "cmdline");
    if (cmdline == nullptr) {
        return false;
    }
    const std::string normalizedCmdline = NormalizePath(*cmdline);
    const std::string normalizedNeedle = NormalizePath(needle);
    return normalizedCmdline.find(normalizedNeedle) != std::string::npos;
}
}