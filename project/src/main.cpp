#include <cstdio>
#include <memory>
#include <print>
#include <stdexcept>
#include <string>

#include "agent.h"
#include "event.h"
#include "event_source.h"
#include "sources.h"
#include "type_filter.h"

using namespace nano_edr;

int main(int argc, char** argv) {
    try {
        bool quiet = false;
        bool use_file = false;
        bool has_only = false;
        EventType only_type = EventType::kOther;
        std::string path;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--quiet") {
                quiet = true;
            } else if (arg == "--file") {
                use_file = true;
            } else if (arg == "--only") {
                if (i + 1 >= argc) {
                    std::print(stderr, "нужен тип события после --only\n");
                    return 2;
                }
                std::string name = argv[++i];
                EventType type = EventTypeFromString(name);
                if (type == EventType::kOther) {
                    std::print(stderr, "неизвестный тип события: {}\n", name);
                    return 2;
                }
                has_only = true;
                only_type = type;
            } else if (!arg.empty() && arg[0] == '-') {
                std::print(stderr, "неизвестная опция: {}\n", arg);
                return 2;
            } else if (!path.empty()) {
                std::print(stderr, "путь указан дважды: {}\n", arg);
                return 2;
            } else {
                path = arg;
            }
        }
        if (path.empty()) {
            std::print(stderr,
                       "использование: nano-edr [--quiet] [--file] "
                       "<сценарий>\n");
            return 2;
        }
        Agent agent(quiet);
        std::unique_ptr<IEventSource> source;
        if (use_file) {
            source = std::make_unique<FileSource>(path);
        } else {
            source = std::make_unique<OsSource>(path);
        }
        IEventHandler* handler = &agent;
        std::unique_ptr<TypeFilter> filter;
        if (has_only) {
            filter = std::make_unique<TypeFilter>(&agent, only_type);
            handler = filter.get();
        }
        source->Run(handler);

        agent.PrintSummary();
        if (filter && !quiet) {
            std::print("отфильтровано событий: {}\n", filter->filtered());
        }
        return 0;
    } catch (const std::exception& e) {
        std::print(stderr, "ошибка: {}\n", e.what());
        return 1;
    }
}