#include "agent_rules.h"
#include "fields.h"
#include <iterator>
namespace nano_edr {
bool ScriptHostFromTemp(const Event& event) {
    if (!IsProcessStart(event)) {
        return false;
    }
    const std::string* image_ptr = FindField(event, "image");
    if (image_ptr == nullptr) {
        return false;
    }
    const std::string image = NormalizePath(*image_ptr);
    if (image.find("wscript.exe") == std::string::npos &&
        image.find("cscript.exe") == std::string::npos) {
        return false;
    }
    if (CommandLineContains(event, "\\appdata\\local\\temp\\") ||
        CommandLineContains(event, "\\windows\\temp\\")) {
        return true;
    }
    return false;
}
bool LolbinDownload(const Event& event) {
    if (!IsProcessStart(event)) {
        return false;
    }
    const std::string& image = GetRequiredField(event, "image");
    std::string image_normalize = NormalizePath(image);
    if (image_normalize.ends_with("certutil.exe") || image_normalize.ends_with("bitsadmin.exe")) {
        if (CommandLineContains(event, "urlcache") || CommandLineContains(event, "transfer") || CommandLineContains(event, "http:") || CommandLineContains(event, "https:")) {
            return true;
        }
    }
    return false;
}
bool HiddenPowershell(const Event& event) {
    if (!IsProcessStart(event)) {
        return false;
    }
    const std::string& image = GetRequiredField(event, "image");
    std::string image_normalize = NormalizePath(image);
    if (image_normalize.ends_with("powershell.exe") || image_normalize.ends_with("pwsh.exe")) {
        if (CommandLineContains(event, "-w hidden") || CommandLineContains(event, "-windowstyle hidden") || CommandLineContains(event, "-enc") || CommandLineContains(event, "-encodedcommand")) {
            return true;
        }
    }
    return false;
}
bool AutostartWrite(const Event& event) {
    const char* field = nullptr;
    if (event.type() == "file_create" || event.type() == "file_write") {
        field = "path";
    } else if (event.type() == "file_move") {
        field = "to";
    } else {
        return false;
    }
    const std::string* path = FindField(event, field);
    if (path == nullptr) {
        return false;
    }
    const std::string path_normalize = NormalizePath(*path);
    return path_normalize.find("\\start menu\\programs\\startup\\") != std::string::npos;
}
bool RansomExtension(const Event& event) {
    const char* field = nullptr;
    if (event.type() == "file_create" || event.type() == "file_write") {
        field = "path";
    } else if (event.type() == "file_move") {
        field = "to";
    } else {
        return false;
    }
    const std::string* path = FindField(event, field);
    if (path == nullptr) {
        return false;
    }
    return NormalizePath(*path).ends_with(".locked");
}
constexpr Rule kRules[] = {
    {"script_host_from_temp", ScriptHostFromTemp, Severity::kHigh},
    {"lolbin_download", LolbinDownload, Severity::kHigh},
    {"hidden_powershell", HiddenPowershell, Severity::kMedium},
    {"autostart_write", AutostartWrite, Severity::kHigh},
    {"ransom_extension", RansomExtension, Severity::kCritical},
};
const Rule* AgentRules() {
    return kRules;
}
size_t AgentRuleCount() {
    return std::size(kRules);
}
}