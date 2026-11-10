#include "sources.h"
#include "parse.h"
#include "os.h"
#include "os_handle.h"
#include <fstream>
#include <print>
#include <stdexcept>
#include <utility>
namespace nano_edr {
FileSource::FileSource(std::string path) : path_(std::move(path)) {}
void FileSource::Run(IEventHandler* handler) {
    if (!handler) {
        throw std::invalid_argument("handler is null");
    }
    std::ifstream log(path_);
    if (!log) {
        throw std::runtime_error("не удалось открыть журнал: " + path_);
    }
    std::string line;
    while (std::getline(log, line)) {
        if (IsBlankOrComment(line)) {
            handler->OnSkipped();
            continue;
        }
        EventParts parts;
        if (!ParseEventParts(line, &parts)) {
            handler->OnBroken();
            continue;
        }
        try {
            Event event(parts);
            handler->HandleEvent(event);
        } catch (const std::exception&) {
            handler->OnBroken();
        }
    }
}
OsSource::OsSource(std::string scenario_path) : scenario_path_(std::move(scenario_path)) {}
static void Trampoline(const os_event* ev, void* ctx) noexcept {
    auto* handler = static_cast<IEventHandler*>(ctx);
    try {
        EventParts parts;
        parts.ts = std::to_string(ev->ts);
        parts.type = ev->type ? ev->type : "";
        if (ev->pid != 0) {
            parts.pid = std::to_string(ev->pid);
        }
        for (std::size_t i = 0; i < ev->field_count; ++i) {
            const os_kv& kv = ev->fields[i];
            parts.fields.push_back(Field{kv.key ? kv.key : "", kv.value ? kv.value : ""});
        }
        Event event(parts);
        handler->HandleEvent(event);
    } catch (...) {
        handler->OnBroken();
    }
}
void OsSource::Run(IEventHandler* handler) {
    if (!handler) {
        throw std::invalid_argument("handler is null");
    }
    OsHandle handle(scenario_path_);
    handle.Subscribe(Trampoline, handler);
    handle.Start();
    os_status status;
    while ((status = handle.Wait(100)) == OS_TIMEOUT) {
        handler->OnTick();
    }
    if (status != OS_OK && status != OS_STOPPED) {
        throw std::runtime_error(std::string("os: ") + os_status_str(status));
    }
}
}