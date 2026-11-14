#include "dedup_sink.h"

#include <utility>
namespace nano_edr {
DedupSink::DedupSink(AlertSink inner, uint64_t window_ms, std::size_t* suppressed)
    : inner_(std::move(inner)), window_ms_(window_ms), suppressed_(suppressed) {}
void DedupSink::operator()(const Detection& detection) {
    if (window_ms_ == 0) {
        if (inner_) {
            inner_(detection);
        }
        return;
    }
    const uint64_t now = detection.ts.ms;
    for (auto& seen : seen_) {
        if (seen.rule == detection.rule && seen.pid == detection.pid) {
            if (now >= seen.first_ts && now - seen.first_ts <= window_ms_) {
                if (suppressed_) {
                    ++(*suppressed_);
                }
                return;
            } else {
                seen.first_ts = now;
                if (inner_) {
                    inner_(detection);
                }
                return;
            }
        }
    }
    seen_.push_back({detection.rule, detection.pid, now});
    if (inner_) {
        inner_(detection);
    }
}
}  // namespace nano_edr
