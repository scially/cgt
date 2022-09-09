#pragma once

#include <cgt/cgtexception.h>

#include <vector>
#include <cstdint>
#include <string>
#include <string>

namespace scially {
    std::vector<std::string> split(std::string_view str, std::string_view delimiters);

    class osg_modeldata {
    public:
        void load(const std::string& srs, const std::string& srs_origin);
        void load_from_file(const std::string& input);
        void write(const std::string &output);

        double x() const { return x_; }
        double y() const { return y_; }
        double z() const { return z_; }
        std::string srs() const { return srs_; }

        void set_x(double x) { x_ = x;}
        void set_y(double y) { y_ = y;}
        void set_z(double z) { z_ = z;}
        void set_srs(const std::string& srs) { srs_ = srs; }
    private:
        void parse_origin(const std::string& srs_origin);
        void parse_srs_origin(const std::string& srs, const std::string&  srs_origin);

        double x_ = 0;
        double y_ = 0;
        double z_ = 0;
        // GZ2000 => EPSG:0
        // ENU => EPSG:4326
        std::string srs_;
    };
}
