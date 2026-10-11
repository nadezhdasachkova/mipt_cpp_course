#include "sources.h"
#include "os_handle.h"
#include "parse.h"
#include <fstream>
#include <print>
#include <stdexcept>
#include <string>
namespace nano_edr {
int RunFileSource(const std::string& path, Agent& agent) {
    std::ifstream log(path);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", path);
        return 1;
    }
    std::string line;
    while (std::getline(log, line)) {
        if (IsBlankOrComment(line)) continue;
        EventParts parts;
        if (!ParseEventParts(line, &parts)) continue;
        try {
            Event event(parts);
            agent.OnEvent(event);
        } catch (const std::exception&) {
        }
    }
    return 0;
}
int RunOsSource(const std::string& scenario_path, Agent& agent) {
    try {
        OsHandle handle(scenario_path);
        handle.Subscribe(&Agent::Trampoline, &agent);
        handle.Start();
        os_status status = OS_OK;
        while ((status = handle.Wait(100)) == OS_TIMEOUT) {
        }
        handle.Stop();
        if (status == OS_OK || status == OS_STOPPED) return 0;
        std::print(stderr, "граница: {}\n", os_status_str(status));
        return 1;
    } catch (const std::exception& e) {
        std::print(stderr, "отказ границы: {}\n", e.what());
        return 1;
    }
}
} 