#include "agent_rules.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "combinators.h"
#include "conditions.h"
#include "make_rule.h"

namespace nano_edr {
namespace {
std::unique_ptr<ICondition> MakeTempCmdlineCondition() {
    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldContains>("cmdline", "\\appdata\\local\\temp\\"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "/appdata/local/temp/"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "\\windows\\temp\\"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "/windows/temp/"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "%temp%"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "%tmp%"));
    return temp;
}
std::unique_ptr<MatchRule> MakeScriptHostFromTemp() {
    auto rule = std::make_unique<MatchRule>("script_host_from_temp", Severity::kHigh);
    auto all = std::make_unique<AllOf>();
    all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto images = std::make_unique<AnyOf>();
    images->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
    images->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));
    all->Add(std::move(images));
    all->Add(MakeTempCmdlineCondition());
    rule->AddCondition(std::move(all));
    rule->AddAction("kill_process");
    return rule;
}
std::unique_ptr<MatchRule> MakeLolbinDownload() {
    auto rule = std::make_unique<MatchRule>("lolbin_download", Severity::kHigh);
    auto all = std::make_unique<AllOf>();
    all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto images = std::make_unique<AnyOf>();
    images->Add(std::make_unique<FieldEndsWith>("image", "certutil.exe"));
    images->Add(std::make_unique<FieldEndsWith>("image", "bitsadmin.exe"));
    all->Add(std::move(images));
    auto cmds = std::make_unique<AnyOf>();
    cmds->Add(std::make_unique<FieldContains>("cmdline", "urlcache"));
    cmds->Add(std::make_unique<FieldContains>("cmdline", "transfer"));
    cmds->Add(std::make_unique<FieldContains>("cmdline", "http:"));
    cmds->Add(std::make_unique<FieldContains>("cmdline", "https:"));
    all->Add(std::move(cmds));
    rule->AddCondition(std::move(all));
    rule->AddAction("kill_process");
    return rule;
}
std::unique_ptr<MatchRule> MakeHiddenPowershell() {
    auto rule = std::make_unique<MatchRule>("hidden_powershell", Severity::kMedium);
    auto all = std::make_unique<AllOf>();
    all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto images = std::make_unique<AnyOf>();
    images->Add(std::make_unique<FieldEndsWith>("image", "powershell.exe"));
    images->Add(std::make_unique<FieldEndsWith>("image", "pwsh.exe"));
    all->Add(std::move(images));
    auto cmds = std::make_unique<AnyOf>();
    cmds->Add(std::make_unique<FieldContains>("cmdline", "-w hidden"));
    cmds->Add(std::make_unique<FieldContains>("cmdline", "-windowstyle hidden"));
    cmds->Add(std::make_unique<FieldContains>("cmdline", "-enc"));
    cmds->Add(std::make_unique<FieldContains>("cmdline", "-encodedcommand"));
    all->Add(std::move(cmds));
    rule->AddCondition(std::move(all));
    rule->AddAction("kill_process");
    return rule;
}
std::unique_ptr<MatchRule> MakeAutostartWrite() {
    auto rule = std::make_unique<MatchRule>("autostart_write", Severity::kHigh);
    auto all = std::make_unique<AllOf>();
    all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kFileCreate, EventType::kFileWrite, EventType::kFileMove}));
    auto paths = std::make_unique<AnyOf>();
    paths->Add(std::make_unique<FieldContains>("path", "\\start menu\\programs\\startup\\"));
    paths->Add(std::make_unique<FieldContains>("path", "/start menu/programs/startup/"));
    paths->Add(std::make_unique<FieldContains>("to", "\\start menu\\programs\\startup\\"));
    paths->Add(std::make_unique<FieldContains>("to", "/start menu/programs/startup/"));
    all->Add(std::move(paths));
    rule->AddCondition(std::move(all));
    rule->AddAction("quarantine_file");
    return rule;
}
std::unique_ptr<MatchRule> MakeRansomExtension() {
    auto rule = std::make_unique<MatchRule>("ransom_extension", Severity::kCritical);
    auto all = std::make_unique<AllOf>();
    all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kFileCreate, EventType::kFileWrite, EventType::kFileMove}));
    auto paths = std::make_unique<AnyOf>();
    paths->Add(std::make_unique<FieldEndsWith>("path", ".locked"));
    paths->Add(std::make_unique<FieldEndsWith>("to", ".locked"));
    all->Add(std::move(paths));
    rule->AddCondition(std::move(all));
    rule->AddAction("quarantine_file");
    return rule;
}
std::unique_ptr<SequenceRule> MakeScriptHostBeacon() {
    auto rule = std::make_unique<SequenceRule>("script_host_beacon", Severity::kCritical, 10000);
    auto first = std::make_unique<AllOf>();
    first->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto images = std::make_unique<AnyOf>();
    images->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
    images->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));
    first->Add(std::move(images));
    first->Add(MakeTempCmdlineCondition());
    rule->SetFirst(std::move(first));
    rule->SetSecond(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kNetConnect}));
    rule->AddAction("kill_process");
    return rule;
}
std::unique_ptr<ThresholdRule<128>> MakeRansomwareMassWrite() {
    auto rule = MakeRule<ThresholdRule<128>>("ransomware_mass_write", Severity::kCritical, 20, 10000, "pid");
    rule->SetCondition(MakeCondition(
        AllOfStatic(EventTypeIs({EventType::kFileCreate, EventType::kFileWrite, EventType::kFileMove}),
                    AnyOfStatic(FieldContains("path", "\\documents\\"),
                                FieldContains("path", "/documents/"),
                                FieldContains("to", "\\documents\\"),
                                FieldContains("to", "/documents/"),
                                FieldContains("path", "\\desktop\\"),
                                FieldContains("path", "/desktop/"),
                                FieldContains("to", "\\desktop\\"),
                                FieldContains("to", "/desktop/")))));
    rule->AddAction("kill_process");
    rule->AddAction("quarantine_file");
    return rule;
}
std::unique_ptr<SequenceRule> MakeShellSpawnsScriptHost(const EntityModel* model) {
    auto rule = std::make_unique<SequenceRule>("shell_spawns_script_host", Severity::kHigh, 30000);
    auto first = std::make_unique<AllOf>();
    first->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto sh_images = std::make_unique<AnyOf>();
    sh_images->Add(std::make_unique<FieldEndsWith>("image", "cmd.exe"));
    sh_images->Add(std::make_unique<FieldEndsWith>("image", "powershell.exe"));
    sh_images->Add(std::make_unique<FieldEndsWith>("image", "pwsh.exe"));
    first->Add(std::move(sh_images));
    rule->SetFirst(std::move(first));
    auto second = std::make_unique<AllOf>();
    second->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto host_images = std::make_unique<AnyOf>();
    host_images->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
    host_images->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));
    second->Add(std::move(host_images));
    rule->SetSecond(std::move(second));
    rule->set_child_of_prev(true);
    rule->SetModel(model);
    rule->AddAction("kill_process");
    return rule;
}
std::unique_ptr<LambdaRule> MakeSpawnedFromDroppedFile(const EntityModel* model) {
    auto rule = MakeRule<LambdaRule>(
        "spawned_from_dropped_file", Severity::kHigh,
        [model](const Event& event) {
            if (event.event_type() != EventType::kProcessStart) return false;
            const FileRecord* source = model->FindSpawnSource(event.pid());
            return source != nullptr && source->writer_pid != event.pid();
        });
    rule->AddAction("kill_process");
    rule->AddAction("quarantine_file");
    return rule;
}
}  // namespace
RuleEngine BuildAgentRules(const EntityModel* model) {
    RuleEngine engine;
    engine.AddRule(MakeScriptHostFromTemp());
    engine.AddRule(MakeLolbinDownload());
    engine.AddRule(MakeHiddenPowershell());
    engine.AddRule(MakeAutostartWrite());
    engine.AddRule(MakeRansomExtension());
    engine.AddRule(MakeScriptHostBeacon());
    engine.AddRule(MakeRansomwareMassWrite());
    engine.AddRule(MakeShellSpawnsScriptHost(model));
    engine.AddRule(MakeSpawnedFromDroppedFile(model));
    return engine;
}
}  // namespace nano_edr