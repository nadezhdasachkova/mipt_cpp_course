#include <charconv>
#include <cstdio>
#include <print>
#include <stdexcept>
#include <string>

#include "agent.h"
#include "sources.h"
using namespace nano_edr;
int main(int argc, char** argv) {
    try {
        bool quiet = false;
        bool use_file = false;
        std::string path;
        std::size_t window_size = 64;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--quiet") {
                quiet = true;
            } else if (arg == "--file") {
                use_file = true;
            } else if (arg == "--window-size") {
                if (i + 1 >= argc) {
                    std::print(stderr, "нужно число после --window-size\n");
                    return 2;
                }
                std::string num = argv[++i];
                std::size_t v = 0;
                auto [ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), v);
                if (ec != std::errc{} || ptr != num.data() + num.size()) {
                    std::print(stderr, "неверное значение --window-size: {}\n", num);
                    return 2;
                }
                window_size = v;
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
                       "использование: nano-edr [--quiet] [--file] [--window-size N] <сценарий>\n");
            return 2;
        }
        Agent agent(window_size, quiet);
        if (use_file) {
            FileSource source(path);
            source.Run(&agent);
        } else {
            OsSource source(path);
            source.Run(&agent);
        }
        agent.PrintSummary();
        return 0;
    } catch (const std::exception& e) {
        std::print(stderr, "ошибка: {}\n", e.what());
        return 1;
    }
}