#include <print>
#include <stdexcept>
#include <string>
#include <utility>

#include "alert_sink.h"
#include "dedup_sink.h"
#include "detection.h"

namespace nano_edr {

// --- ConsoleSink -----------------------------------------------------------

void ConsoleSink::operator()(const Detection& detection) const {
    std::print("{}\n", ToString(detection));
}

// --- FileSink --------------------------------------------------------------

FileSink::FileSink(const std::string& path) : out_(path) {
    if (!out_) {
        throw std::runtime_error("не удалось открыть файл: " + path);
    }
}

void FileSink::operator()(const Detection& detection) {
    out_ << ToString(detection) << '\n';
}

// --- CountingSink ----------------------------------------------------------

void CountingSink::operator()(const Detection& detection) const {
    if (counter_) {
        ++counter_->total;
        counter_->seen.push_back(detection);
    }
}

// --- FanOutSink ------------------------------------------------------------

void FanOutSink::Add(AlertSink sink) {
    sinks_.push_back(std::move(sink));
}

void FanOutSink::operator()(const Detection& detection) const {
    for (auto& sink : sinks_) {
        sink(detection);
    }
}
}  // namespace nano_edr
