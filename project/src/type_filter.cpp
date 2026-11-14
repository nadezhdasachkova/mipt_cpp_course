#include "type_filter.h"
#include <stdexcept>
namespace nano_edr {
TypeFilter::TypeFilter(IEventHandler* next, EventType type)
    : next_(next), type_(type) {
        if (next_ == nullptr) {
            throw std::invalid_argument("TypeFilter: next is null");
        }
    }
void TypeFilter::HandleEvent(const Event& event) {
    if (event.event_type() == type_) {
        next_->HandleEvent(event);
    } else {
        ++filtered_;
    }
}
void TypeFilter::OnSkipped() {
    next_->OnSkipped();
}
void TypeFilter::OnBroken() {
    next_->OnBroken();
}
void TypeFilter::OnTick() {
    next_->OnTick();
}
}