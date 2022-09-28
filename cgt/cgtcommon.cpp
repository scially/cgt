//
// Created by adminstrator on 2022/9/28.
//

#include <cgt/cgtcommon.h>
#include <filesystem>

#if defined(_WIN32)

#include <Windows.h>

#else
#include <unistd.h>
#endif

namespace scially {
    namespace fs = std::filesystem;

    std::string get_process_filepath() {
#if defined(_WIN32)
        char path[MAX_PATH];
        ::GetModuleFileName(nullptr, path, MAX_PATH);
        return path;
#else
        char path[1024];
        size_t pathNameSize = readlink("/proc/self/exe", pathName, pathNameCapacity - 1);
        pathName[pathNameSize] = '\0';
        return pathNameSize;
#endif
    }
}
