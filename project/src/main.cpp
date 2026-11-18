#include <charconv>
#include <cstdio>
#include <memory>
#include <print>
#include <stdexcept>
#include <string>
#include <vector>

#include "agent.h"
#include "alert_sink.h"
#include "edr_handle.h"
#include "event_source.h"
#include "field_traits.h"
#include "sources.h"

using namespace nano_edr;
struct Options {
    bool quiet = false;
    bool use_file = false;
    std::string path;
    std::string out;
};
int main(int argc, char** argv) {
    try {
        Options opts;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--quiet") {
                opts.quiet = true;
            } else if (arg == "--file") {
                opts.use_file = true;
            } else if (arg == "--out") {
                if (i + 1 >= argc) {
                    std::print(stderr, "нужно имя файла после --out\n");
                    return 2;
                }
                opts.out = argv[++i];
            } else if (!arg.empty() && arg[0] == '-') {
                std::print(stderr, "неизвестная опция : {}\n", arg);
                return 2;
            } else if (!opts.path.empty()) {
                std::print(stderr, "путь указан дважды: {}\n", arg);
                return 2;
            } else
                opts.path = arg;
        }
        if (opts.path.empty()) {
            std::print(stderr, "использование: nano-edr [--quiet] [--file] [--out <файл>] <сценарий>\n");
            return 2;
        }
        EdrHandle edr(opts.path);
        const std::string text = edr.Config("dedup_period_ms", "10000");
        uint64_t dedup_period_ms = 0;
        if (!nano_edr::FieldTraits<uint64_t>::Parse(text, &dedup_period_ms)) {
            std::print(stderr, "[WARN] dedup_period_ms не разбирается: {}\n", text);
            dedup_period_ms = 10000;
        }
        Agent agent(opts.quiet, dedup_period_ms);
        FanOutSink targets;
        targets.Add(ConsoleSink());
        if (!opts.out.empty()) {
            targets.Add(FileSink(opts.out));
        }
        targets.Add(EdrSink(&edr));
        agent.Subscribe(std::move(targets));
        std::unique_ptr<IEventSource> source;
        if (opts.use_file) {
            source = std::make_unique<FileSource>(opts.path);
        } else {
            source = std::make_unique<OsSource>(opts.path);
        }
        source->Run(&agent);

        if (edr.Flush() != EDR_OK) {
            std::print(stderr, "[WARN] детекты не отправлены\n");
        }
        agent.PrintSummary();
        return 0;
    } catch (const std::exception& e) {
        std::print(stderr, "ошибка: {}\n", e.what());
        return 1;
    }
}