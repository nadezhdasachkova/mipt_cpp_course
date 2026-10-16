#include "file_source.h"
#include <fstream>
#include <print>
#include <stdexcept>
#include <string>

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
}  