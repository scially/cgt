#pragma once

#include <cgt/cgtcommon.h>

#include <stdexcept>
#include <string>

namespace scially {

    class CGTLIBRARY cgt_exception : public std::runtime_error {
    public:
        cgt_exception(const std::string &err)
                : err(err),
                  std::runtime_error(err.data()) {

        }

        virtual const char *what() const noexcept override {
            return err.c_str();
        }

    private:
        std::string err;
    };
}
