//
// Created by adminstrator on 2022/9/6.
//

#pragma once

#include <cgt/cgtmodel.h>
#include <osg/Node>
#include <osg/CoordinateSystemNode>

#include <gdal_frmts.h>
#include <ogrsf_frmts.h>
#include <string>
#include <memory>
#include <functional>

namespace scially {

    class cgt_proj {
    public:
        cgt_proj(const osg_modeldata& source_modeldata,  const osg_modeldata& target_modeldata);
        osg::Vec3 transfrom(osg::Vec3 vert) const;
        void transform(OGRGeometry* geom) const;

    private:
        OGRSpatialReference import(const std::string& srs) const;
        std::string crs_to_proj(const std::string &source_crs) const noexcept;
        osg::Vec3 pj_transorm (osg::Vec3 vert, bool forward = true) const;
        osg::Vec3 pj_transorm (double x, double y, double z, bool forward = true) const;

        osg_modeldata source_metadata_;
        osg_modeldata target_metadata_;

        std::function<osg::Vec3(osg::Vec3)> transformer_;
        OGRSpatialReference source_srs_;
        OGRSpatialReference target_srs_;
        std::unique_ptr<OGRCoordinateTransformation> transform_srs_;
    };
}