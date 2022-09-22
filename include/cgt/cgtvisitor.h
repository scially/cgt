//
// Created by adminstrator on 2022/9/19.
//

#pragma once

#include <cgt/cgtcore.h>
#include <osg/NodeVisitor>

namespace scially {
    class geom_visitor : public osg::NodeVisitor {
    public:
        geom_visitor(const std::string base_path, const vec3_transform& algorithm)
                : base_path_(base_path), algorithm_(algorithm){
            setTraversalMode(TRAVERSE_ALL_CHILDREN);
        }
        virtual ~geom_visitor() {}

        virtual void apply(osg::Geode& geode) override;
        virtual void apply(osg::PagedLOD& lod) override;

    private:
        vec3_transform algorithm_;
        std::string base_path_;
    };

}