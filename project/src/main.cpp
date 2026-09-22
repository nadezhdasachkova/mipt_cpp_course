#include <charconv>
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>
#include "event.h"
#include "event_list.h"
#include "parse.h"
using namespace nano_edr;
static void PrintContext(const EventList& window) {
    const EventNode* prev = nullptr;
    const EventNode* last = nullptr; 

    for (const EventNode* node = window.head; node != nullptr; node = node->next) {
        prev = last;
        last = node;
    }

    if (prev != nullptr) {
        std::print("[CTX] -2: ts={} type={} pid={}\n",
                   prev->event.ts, prev->event.type, prev->event.pid);
    }
    if (last != nullptr) {
        std::print("[CTX] -1: ts={} type={} pid={}\n",
                   last->event.ts, last->event.type, last->event.pid);
    }
}
int main(int argc, char** argv) {
    bool quiet = false;
    std::string path;
    std::size_t windowSize = 64;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--window-size") {
            if (i + 1 >= argc) {
                std::print(stderr, "использование: --window-size требует число\n");
                return 2;
            }
            std::string num = argv[++i];
            std::size_t value = 0;
            auto [ptr, ec] = std::from_chars(num.data(),
                                             num.data() + num.size(), value);
            if (ec != std::errc{} || ptr != num.data() + num.size()) {
                std::print(stderr, "неверное значение --window-size: {}\n", num);
                return 2;
            }
            windowSize = value;
        } else {
            path = arg;
        }
    }
    if (path.empty()) {
        std::print(stderr, "использование: nano-edr [--quiet] [--window-size N] <журнал.log>\n");
        return 2;
    }
    std::ifstream log(path);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", path);
        return 2;
    }
    EventList window;
    window.capacity = windowSize;
    long long lines = 0;
    std::string line;
    const std::vector<std::string> signs = {
        "wscript.exe",
        ".locked",
        "certutil.exe",
        "\\Startup\\",
    };
    while (std::getline(log, line)) {
        ++lines;
        if (IsBlankOrComment(&line)) {
            continue;
        }
        Event event;
        if (!ParseEventLine(&line, &event)) {
            continue;
        }
        bool detectedAny = false;
        for (const std::string& s : signs) {
            if (line.find(s) != std::string::npos) {
                std::print("[DETECT] строка {}, признак {}: {}\n", lines, s, line);
                detectedAny = true;
            }
        }
        if (detectedAny && !quiet) {
            PrintContext(window);
        }
        ListPushBack(&window, &event);
    }
    if (!quiet) {
        std::print("строк {}\n", lines);
    }
    return 0;
}