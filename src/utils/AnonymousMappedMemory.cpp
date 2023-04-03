#include "AnonymousMappedMemory.h"
#include <unistd.h>
#include <sys/mman.h>

void AnonymousMappedMemory::Init(size_t length, int prot) {
    _mapping = mmap(NULL, length, prot, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (_mapping == MAP_FAILED) {
        _mapping = NULL;
    } else {
        _size = length;
    }

    Subsystem<AnonymousMappedMemory>::Init();
}

void AnonymousMappedMemory::Teardown() {
    Subsystem<AnonymousMappedMemory>::Teardown();

    if (_mapping != NULL) {
        munmap();
    }
}
