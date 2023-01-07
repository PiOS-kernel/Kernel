#include "../testing.h"
#include "../../kernel/kernel.h"

bool test_event_memory_allocation() {
    uint32_t initial_heap_size = available_space(&HEAP);

    // Using a bool as message type to check word-alignment
    EventHandle event = NEW_EVENT(bool);
    ASSERT(event != NULL);

    uint32_t expected_heap_size = (initial_heap_size - (sizeof(Event) + sizeof(bool))) & ~0x3;
    ASSERT(available_space(&HEAP) == expected_heap_size);

    delete_event(event);
    ASSERT(available_space(&HEAP) == initial_heap_size);
    
    return true;
}