//
// Created by adminstrator on 2022/6/1.
//

#pragma once

#include <cgt/cgtmodel.h>
#include <cgt/cgtproj.h>
#include <spdlog/spdlog.h>

#include <osg/Node>
#include <osg/PagedLOD>
#include <osg/NodeVisitor>
#include <osg/Geometry>

#include <string>
#include <functional>

namespace scially {
    using vec3_transform = std::function<osg::Vec3(osg::Vec3)>;

    class geom_visitor : public osg::NodeVisitor {
    public:
        geom_visitor(const std::string &lod_path, std::function<osg::Vec3(osg::Vec3)> algorithm)
            : algorithm_(algorithm), lod_path_(lod_path){
            setTraversalMode(TRAVERSE_ALL_CHILDREN);
        }
        virtual ~geom_visitor() {}

        virtual void apply(osg::Geometry& geom) override;
        virtual void apply(osg::PagedLOD& lod) override;
    private:
        std::function<osg::Vec3(osg::Vec3)> algorithm_;
        std::string lod_path_;
    };
  
    class node_operator {
    public:
        void read(const std::string& path);
        void write(const std::string& dir);

        void apply(const std::string&, vec3_transform);

    private:
        osg::ref_ptr<osg::Node> node_;
        std::string   node_name_;
    };

    class osg_transform{
    public:
        osg_transform(const std::string& source_dir, const std::string& target_dir);
        void set_source_metadata(const osg_modeldata &modeldata) { source_metadata_ = modeldata; }
        void set_target_metadata(const osg_modeldata &modeldata) { target_metadata_ = modeldata; }
        void run(uint32_t max_thread = 0);
    private:
        vec3_transform init_transform();
        void write_metadata();
        void detect_target_metadata();

        cgt_proj proj_;
        std::string source_dir_;
        std::string target_dir_;

        osg_modeldata source_metadata_;
        osg_modeldata target_metadata_;
    };
}
