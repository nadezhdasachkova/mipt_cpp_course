#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>
int main(int argc, char** argv) {
    bool quiet = false;
    std::string path;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quiet") {
            quiet = true;
        } else {
            path = arg;
        }
    }
    if (path.empty()) {
        std::print(stderr, "использование: nano-edr [--quiet] <журнал.log>\n");
        return 2;
    }
    std::ifstream log(path);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", path);
        return 2;
    }

    long long lines = 0;
    long long comments = 0;
    std::string line;
    long long lines = 0;
    long long comments = 0;
    long long events = 0;
    std::string line;
    std::vector<std::pair<std::string, long long>> stats;
    const std::vector<std::string> signs = {
        "wscript.exe",
        ".locked",
        "certutil.exe",
        "\\Startup\\",
    };
    while (std::getline(log, line)) {
        ++lines;

        std::size_t i = 0;
        while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
            ++i;
        }
        if (i == line.size()) {
            continue;
        }
        if (line[i] == '#' || line[i] == ';') {
            ++comments;
            continue;
        }

        for (const std::string& s: signs) {
        ++events;
        std::size_t tpos = line.find("type=");
        if (tpos != std::string::npos) {
            std::size_t start = tpos + 5;
            std::size_t end = line.find(' ', start);
            std::string type = (end == std::string::npos)
                                   ? line.substr(start)
                                   : line.substr(start, end - start);

            bool found = false;
            for (auto& kv : stats) {
                if (kv.first == type) {
                    ++kv.second;
                    found = true;
                    break;
                }
            }
            if (!found) {
                stats.push_back({type, 1});
            }
        }
        for (const std::string& s : signs) {
            if (line.find(s) != std::string::npos) {
                std::print("[DETECT] строка {}, признак {}: {}\n", lines, s, line);
            }
        }
    }
        if (!quiet) {
            std::print("строк {}, из них комментариев {}\n", lines, comments);
        }
    if (!quiet) {
        std::print("строк {}, из них комментариев {}\n", lines, comments);
        std::print("событий {}\n", events);
        for (const auto& [type, count] : stats) {
            std::print("тип {}: {}\n", type, count);
        }
    }
    return 0;
}
