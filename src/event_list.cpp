#include "event_list.h"
namespace nano_edr {
EventList::~EventList() {
    EventNode* cur = head;
    while (cur != nullptr) {
        EventNode* next = cur->next;
        delete cur;
        cur = next;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}
void ListPushBack(EventList* list, const Event* event) {
    if (list == nullptr || event == nullptr) return;
    EventNode* node = new EventNode;
    node->event = *event;
    node->next = nullptr;
    if (list->tail == nullptr) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    ++list->size;
    if (list->capacity != 0 && list->size > list->capacity) {
        ListPopFront(list);
    }
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