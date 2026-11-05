#include "field_traits.h"
#include <cctype>
namespace nano_edr {
    bool FieldTraits<std::string>::Parse(const std::string& text, std::string* out) {
        *out = text;
        return true;
    }
    bool FieldTraits<uint64_t>::Parse(const std::string& text, uint64_t* out) {
        if (!text.empty()) {
            uint64_t value =0;
            for (auto x: text) {
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
        return false;
    }
    bool FieldTraits<bool>::Parse(const std::string& text, bool* out) {
        std::string lower = text;
        for (auto& x : lower) {
            x = std::tolower(static_cast<unsigned char>(x));
        }
        if (lower == "1" || lower == "true" || lower == "yes") {
            *out = true;
            return true;
        } else if (lower == "0" || lower == "false" || lower == "no") {
            *out = false;
            return true;
        } else {
            return false;
        }
    }
}