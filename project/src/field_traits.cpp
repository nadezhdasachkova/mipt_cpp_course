#include "field_traits.h"

#include <cctype>
#include <cstdint>
#include <string>

#include "text.h"

namespace nano_edr {

bool FieldTraits<std::string>::Parse(const std::string& text, std::string* out) {
    *out = text;
    return true;
}
bool FieldTraits<uint64_t>::Parse(const std::string& text, uint64_t* out) {
    if (text.empty()) {
        return false;
    }
    uint64_t value = 0;
    for (char x : text) {
        if (!std::isdigit(static_cast<unsigned char>(x))) {
            return false;
        }
        uint64_t digit = x - '0';
        uint64_t next = value * 10 + digit;
        if (next / 10 != value || next % 10 != digit) {
            return false;
        }
        value = next;
    }
    *out = value;
    return true;
}
bool FieldTraits<bool>::Parse(const std::string& text, bool* out) {
    if (EqualsIgnoreCase(text, "1") || EqualsIgnoreCase(text, "true") ||
        EqualsIgnoreCase(text, "yes")) {
        *out = true;
        return true;
    }
    if (EqualsIgnoreCase(text, "0") || EqualsIgnoreCase(text, "false") ||
        EqualsIgnoreCase(text, "no")) {
        *out = false;
        return true;
    }
    return false;
}
}  // namespace nano_edr