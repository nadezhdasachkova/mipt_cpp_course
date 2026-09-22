#include "event_list.h"

namespace nano_edr {

EventList::~EventList() {
    ListClear(this);
}

void ListPushBack(EventList* list, const Event* event) {
    if (list == nullptr || event == nullptr) return;

    if (list->capacity != 0 && list->size >= list->capacity) {
        ListPopFront(list);
    }

    EventNode* node = new EventNode;
    node->event = *event;
    node->next = nullptr;

    if (list->tail == nullptr) {
        list->head = node;
    } else {
        list->tail->next = node;
    }

    list->tail = node;
    ++list->size;
}

void ListPopFront(EventList* list) {
    if (list == nullptr || list->head == nullptr) return;

    EventNode* old = list->head;
    list->head = old->next;

    if (list->head == nullptr) {
        list->tail = nullptr;
    }

    delete old;
    --list->size;
}

void ListClear(EventList* list) {
    if (list == nullptr) return;

    EventNode* cur = list->head;
    while (cur != nullptr) {
        EventNode* next = cur->next;
        delete cur;
        cur = next;
    }

    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

} 