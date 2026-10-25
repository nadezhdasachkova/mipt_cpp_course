#include "agent_rules.h"
#include "conditions.h"
namespace nano_edr {
void RegisterAgentRules(RuleEngine* engine) {
    {
        auto rule = std::make_unique<MatchRule>("script_host_from_temp", Severity::kHigh);
        auto all = std::make_unique<AllOf>();
        all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
        auto images = std::make_unique<AnyOf>();
        images->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
        images->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));
        all->Add(std::move(images));
        auto paths = std::make_unique<AnyOf>();
        paths->Add(std::make_unique<FieldContains>("cmdline", "\\appdata\\local\\temp\\"));
        paths->Add(std::make_unique<FieldContains>("cmdline", "/appdata/local/temp/"));
        paths->Add(std::make_unique<FieldContains>("cmdline", "\\windows\\temp\\"));
        paths->Add(std::make_unique<FieldContains>("cmdline", "/windows/temp/"));
        paths->Add(std::make_unique<FieldContains>("cmdline", "%temp%"));
        paths->Add(std::make_unique<FieldContains>("cmdline", "%tmp%"));
        all->Add(std::move(paths));
        rule->AddCondition(std::move(all));
        rule->AddAction("kill_process");
        engine->AddRule(std::move(rule));
    }
    {
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
        engine->AddRule(std::move(rule));
    }
    {
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
        engine->AddRule(std::move(rule));
    }
    {
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
        engine->AddRule(std::move(rule));
    }
    {
        auto rule = std::make_unique<MatchRule>("ransom_extension", Severity::kCritical);
        auto all = std::make_unique<AllOf>();
        all->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kFileCreate, EventType::kFileWrite, EventType::kFileMove}));
        auto paths = std::make_unique<AnyOf>();
        paths->Add(std::make_unique<FieldEndsWith>("path", ".locked"));
        paths->Add(std::make_unique<FieldEndsWith>("to", ".locked"));
        all->Add(std::move(paths));
        rule->AddCondition(std::move(all));
        rule->AddAction("quarantine_file");
        engine->AddRule(std::move(rule));
    }
    {
        auto rule = std::make_unique<SequenceRule>("script_host_beacon", Severity::kCritical, 10000);
        auto first = std::make_unique<AllOf>();
        first->Add(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
        auto images = std::make_unique<AnyOf>();
        images->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
        images->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));
        first->Add(std::move(images));
        rule->SetFirst(std::move(first));
        rule->SetSecond(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kNetConnect}));
        rule->AddAction("kill_process");
        engine->AddRule(std::move(rule));
    }
}
}  // namespace nano_edr