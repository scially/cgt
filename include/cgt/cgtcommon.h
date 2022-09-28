//
// Created by adminstrator on 2022/9/28.
//

#pragma once

#include <osgDB/FileNameUtils>

#if defined(_WIN32)
#define U8TEXT(s) osgDB::convertStringFromCurrentCodePageToUTF8(s)

#if defined(CGTEXPORT)
#define CGTLIBRARY __declspec(dllexport)
#else
#define CGTLIBRARY __declspec(dllimport)
#endif
#else
#define U8TEXT(s) s
#define CGTLIBRAR
#endif


namespace scially {
    std::string get_process_filepath();
}