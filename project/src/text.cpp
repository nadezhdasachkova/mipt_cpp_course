#include "text.h"
#include <cctype>
#include <cstddef>
namespace nano_edr {
namespace {
char ToLowerAscii(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
}
bool EqualsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (ToLowerAscii(a[i]) != ToLowerAscii(b[i])) {
            return false;
        }
    }
    return true;
}
bool ContainsIgnoreCase(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) {
        return true;
    }
    if (needle.size() > haystack.size()) {
        return false;
    }
    for (std::size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
        bool match = true;
        for (std::size_t j = 0; j < needle.size(); ++j) {
            if (ToLowerAscii(haystack[i + j]) != ToLowerAscii(needle[j])) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}
bool EndsWithIgnoreCase(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) {
        return false;
    }
    const std::size_t offset = str.size() - suffix.size();
    for (std::size_t i = 0; i < suffix.size(); ++i) {
        if (ToLowerAscii(str[offset + i]) != ToLowerAscii(suffix[i])) {
            return false;
        }
    }
    return true;
}
}