#ifndef __ANONYMOUS_MAPPED_MEMORY_H__
#define __ANONYMOUS_MAPPED_MEMORY_H__

#include "Subsystem.h"

class AnonymousMappedMemory : public Subsystem<AnonymousMappedMemory> {
 private:
    void *_mapping = NULL;
    size_t _size = 0;

 public:
    void Init(size_t length, int prot);
    void Teardown();

    operator uint8_t *() const {
        return reinterpret_cast<uint8_t *>(_mapping);
    }

    operator void *() const {
        return reinterpret_cast<void *>(_mapping);
    }

    void *Ptr() const {
        return reinterpret_cast<void *>(_mapping);
    }

    size_t Size() const {
        return _size;
    }
};

#endif  // __MAPPED_MEMORY_H__
