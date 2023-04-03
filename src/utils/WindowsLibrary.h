#ifndef __WINDOWS_LIBRARY_H__
#define __WINDOWS_LIBRARY_H__

#include "Subsystem.h"
#include "AnonymousMappedMemory.h"

#include <filesystem>

class WindowsLibrary : public Subsystem<WindowsLibrary> {
 private:
    AnonymousMappedMemory _mapping;

 public:
    void Init(const std::string &dll_path);
    void Teardown();
};

#endif  // __WINDOWS_LIBRARY_H__
