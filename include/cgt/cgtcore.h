//
// Created by adminstrator on 2022/6/1.
//

#pragma once

#include <cgt/cgtmodel.h>
#include <cgt/cgtproj.h>
#include <cgt/cgtcommon.h>
#include <spdlog/spdlog.h>

#include <osg/Node>
#include <osg/PagedLOD>
#include <osg/NodeVisitor>
#include <osg/Geometry>

#include <string>
#include <functional>

namespace scially {
    using vec3_transform = std::function<osg::Vec3(osg::Vec3)>;

    class CGTLIBRARY node_operator {
    public:
        void read(const std::string &path);

        void write(const std::string &dir);

        void apply(const std::string &base_path, const vec3_transform &);

    private:
        osg::ref_ptr<osg::Node> node_;
        std::string node_name_;
    };

    class CGTLIBRARY osg_base {
    public:
        static constexpr double DOUBLE_EPS = 1e-5;

        osg_base(const std::string &source_dir, const std::string &target_dir) :
                source_dir_(source_dir), target_dir_(target_dir) {

        }

        virtual ~osg_base() {}

        static std::string get_root_name(const std::string &tile_path);

        static std::string get_data_name(const std::string &tile_path);

        void set_source_metadata(const osg_modeldata &modeldata) { source_metadata_ = modeldata; }

        void set_target_metadata(const osg_modeldata &modeldata) { target_metadata_ = modeldata; }

        void run(uint32_t max_thread = 0);

        virtual bool root_process(cgt_proj &proj, const std::string &tile_path) = 0;

        virtual bool tile_process(cgt_proj &proj, const std::string &tile_path) = 0;

        virtual bool end_process() { return true; }

    protected:
        osg::BoundingBox detect_target_metadata();

    protected:
        std::string source_dir_;
        std::string target_dir_;
        osg_modeldata source_metadata_;
        osg_modeldata target_metadata_;
    };
}
