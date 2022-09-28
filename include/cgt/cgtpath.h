//
// Created by adminstrator on 2022/9/28.
//

#pragma once

#include <osgDB/FileNameUtils>

#if defined(_WIN32)
#define U8TEXT(s) osgDB::convertStringFromCurrentCodePageToUTF8(s)
#else
#define U8TEXT(s) s
#endif

namespace scially {
    std::string get_process_filepath();
}