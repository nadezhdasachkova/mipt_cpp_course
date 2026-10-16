#include "agent_rules.h"

#include <iterator>
#include <string>
#include <cstddef>

#include "fields.h"
namespace nano_edr {
static bool ImageEndsWith(const std::string& image, const char* name) {
    return image.ends_with(name);
}
static const std::string* FilePathField(const Event& event) {
    if (event.type() == "file_create" || event.type() == "file_write") {
        return FindField(event, "path");
    }
    if (event.type() == "file_move") {
        return FindField(event, "to");
    }
    return nullptr;
}
bool ScriptHostFromTemp(const Event& event) {
    if (!IsProcessStart(event)) {
        return false;
    }
    const std::string* image = FindField(event, "image");
    if (image == nullptr) {
        return false;
    }
    const std::string norm = NormalizePath(*image);
    if (!ImageEndsWith(norm, "wscript.exe") && !ImageEndsWith(norm, "cscript.exe")) {
        return false;
    }
    return CommandLineContains(event, "\\appdata\\local\\temp\\") || CommandLineContains(event, "\\windows\\temp\\");
}
bool LolbinDownload(const Event& event) {
    if (!IsProcessStart(event)) {
        return false;
    }
    const std::string norm = NormalizePath(GetRequiredField(event, "image"));
    if (!ImageEndsWith(norm, "certutil.exe") && !ImageEndsWith(norm, "bitsadmin.exe")) {
        return false;
    }
    return CommandLineContains(event, "urlcache") || CommandLineContains(event, "transfer") || CommandLineContains(event, "http:") || CommandLineContains(event, "https:");
}
bool HiddenPowershell(const Event& event) {
    if (!IsProcessStart(event)) {
        return false;
    }
    const std::string norm = NormalizePath(GetRequiredField(event, "image"));
    if (!ImageEndsWith(norm, "powershell.exe") && !ImageEndsWith(norm, "pwsh.exe")) {
        return false;
    }
    return CommandLineContains(event, "-w hidden") || CommandLineContains(event, "-windowstyle hidden") || CommandLineContains(event, "-enc") || CommandLineContains(event, "-encodedcommand");
}
bool AutostartWrite(const Event& event) {
    const std::string* path = FilePathField(event);
    if (path == nullptr) {
        return false;
    }
    return NormalizePath(*path).find("\\start menu\\programs\\startup\\") != std::string::npos;
}
bool RansomExtension(const Event& event) {
    const std::string* path = FilePathField(event);
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
}  // namespace nano_edr