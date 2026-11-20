#ifndef NANO_EDR_TYPE_FILTER_H
#define NANO_EDR_TYPE_FILTER_H
#include <cstddef>

#include "event.h"
#include "event_source.h"

namespace nano_edr {
class TypeFilter : public IEventHandler {
 public:
    TypeFilter(IEventHandler* next, EventType type);
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    std::size_t filtered() const { return filtered_; }

 private:
    IEventHandler* next_;
    EventType type_;
    std::size_t filtered_ = 0;
};
}  // namespace nano_edr
#endif