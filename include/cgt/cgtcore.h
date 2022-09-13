//
// Created by adminstrator on 2022/6/1.
//

#pragma once

#include <cgt/cgtmodel.h>
#include <cgt/cgtproj.h>
#include <cgt/gdalcpp.hpp>
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
        geom_visitor( std::function<osg::Vec3(osg::Vec3)> algorithm)
            : algorithm_(algorithm){
            setTraversalMode(TRAVERSE_ALL_CHILDREN);
        }
        virtual ~geom_visitor() {}

        virtual void apply(osg::Geometry& geom) override;
        virtual void apply(osg::PagedLOD& lod) override;
    private:
        std::function<osg::Vec3(osg::Vec3)> algorithm_;

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

    class osg_export{
    public:
        osg_export(const std::string& source_dir, const std::string& target_dir);
        void set_source_metadata(const osg_modeldata &modeldata) { source_metadata_ = modeldata; }
        void set_is_copy(bool is_copy) { is_copy_ = is_copy; }
        void set_extent(const std::string& shp);
        void run(uint32_t max_thread = 0);

        ~osg_export() {
            if(geometry_)
                delete geometry_;
        }
    private:
        bool is_intersect(const std::string &tile_path);

        bool is_copy_ = false;
        std::string source_dir_;
        std::string target_dir_;
        std::string shpfile_;
        osg_modeldata source_metadata_;
        OGRGeometry *geometry_ = nullptr;
    };
}
