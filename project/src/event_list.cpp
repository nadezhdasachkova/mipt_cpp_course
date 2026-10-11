#include "event_list.h"
namespace nano_edr {
EventList::~EventList() {
    Clear();
}
void EventList::PushBack(const Event& event) {
    if (capacity_ != 0 && size_ >= capacity_) {
        PopFront();
    }
    EventNode* node = new EventNode(event);
    if (tail_ == nullptr) {
        head_ = node;
    } else {
        tail_->next =node;
    }
    tail_ = node;
    ++size_;
}
void EventList::PopFront() {
    if (head_ == nullptr) {
        return;
    }
    EventNode* old = head_;
    head_ = old->next;
    if (head_ == nullptr) {
        tail_ = nullptr;
    }
    delete old;
    --size_;
}
void EventList::Clear() {
    EventNode* cur = head_;
    while (cur != nullptr) {
        EventNode* next = cur->next;
        delete cur;
        cur = next;
    }
    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}
} 