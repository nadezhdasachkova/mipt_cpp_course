#include "sources.h"
#include "os.h"
#include <fstream>
#include <print>
#include <stdexcept>
#include <string>

#include "os_handle.h"
#include "parse.h"
namespace nano_edr {
FileSource::FileSource(std::string path) 
    : path_(std::move(path)) {}
void FileSource::Run(Agent* agent) {
    if (agent == nullptr) {
        throw std::invalid_argument("FileSource::Run: agent is null");
    }
    std::ifstream log(path_);
    if (!log) {
        throw std::runtime_error("не удалосб открыть журнал: " + path_);
    }
    std::string line;
    while (std::getline(log, line)) {
        if (IsBlankOrComment(line)) {
            continue;
        }
        EventParts parts;
        if (!ParseEventParts(line, &parts)) {
            agent->HandleSkipped("битая строка журнала");
            continue;
        }
        try {
            Event event(parts);
            agent->HandleEvent(event);
        } catch (const std::exception& e) {
            agent->HandleSkipped(e.what());
        }
    }
}
OsSource::OsSource(std::string scenario_path)
    : scenario_path_(std::move(scenario_path)) {}
void OsSource::Run(Agent* agent) {
    if (agent == nullptr) {
        throw std::invalid_argument("OsSource::Run: agent is null");
    }
    OsHandle handle(scenario_path_);
    handle.Subscribe(&Agent::Trampoline, agent);
    handle.Start();
    os_status status;
    while ((status = handle.Wait(100)) == OS_TIMEOUT) {

    }
    handle.Stop();
    if (status != OS_OK && status != OS_STOPPED) {
        throw std::runtime_error(std::string("os: ") + os_status_str(status));
    }
}
}  