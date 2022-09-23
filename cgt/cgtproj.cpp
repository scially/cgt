//
// Created by adminstrator on 2022/9/16.
//

#include <cgt/cgtproj.h>
#include <osg/CoordinateSystemNode>
#include <osg/Math>

namespace scially {
    class OGRPointOffsetVisitor: public OGRDefaultGeometryVisitor{
    public:
        OGRPointOffsetVisitor(double x, double y, double z = 0): x_(x), y_(y), z_(z){}
        void visit(OGRPoint* point) override {
            point->setX(point->getX() + x_);
            point->setY(point->getY() + y_);
            point->setZ(point->getY() + z_);
        }
    private:
        double x_;
        double y_;
        double z_;
    };

    cgt_proj::cgt_proj(const scially::osg_modeldata& source_metadata, const scially::osg_modeldata &target_metadata)
        : source_metadata_(source_metadata), target_metadata_(target_metadata) {
        if(target_metadata_.is_valid() && target_metadata_.srs() != source_metadata_.srs()) {
            std::string source_srs, target_srs;
            crs_to_proj(source_metadata_.srs(),
                        source_srs, source_topcenteric_, source_srs_is_topcenteric_);
            crs_to_proj(target_metadata_.srs(),
                        target_srs, target_topcenteric_, target_srs_is_topcenteric_);
            source_srs_ = import(source_srs);
            target_srs_ = import(target_srs);

            transform_srs_.reset(OGRCreateCoordinateTransformation(&source_srs_, &target_srs_));
            if (transform_srs_ == nullptr)
                throw cgt_exception("could transfrom from " + source_srs + "to " + target_srs);

            if (source_srs_is_topcenteric_) {
                double lat_r = osg::DegreesToRadians(source_topcenteric_.y());
                double lon_r = osg::DegreesToRadians(source_topcenteric_.x());
                double h = source_topcenteric_.z();
                ellipsoid_.computeLocalToWorldTransformFromLatLongHeight(lat_r, lon_r, h,
                                                                         source_local_to_world_);
            }
            if (target_srs_is_topcenteric_) {
                double lat_r = osg::DegreesToRadians(target_topcenteric_.y());
                double lon_r = osg::DegreesToRadians(target_topcenteric_.x());
                double h = target_topcenteric_.z();
                osg::Matrixd local_to_world;
                ellipsoid_.computeLocalToWorldTransformFromLatLongHeight(lat_r, lon_r, h,
                                                                         local_to_world);
                target_world_to_local_ = osg::Matrixd::inverse(local_to_world);
            }
        }
    }


    OGRSpatialReference cgt_proj::import(const std::string& srs) const{
        OGRSpatialReference osr;
        OGRErr err = OGRERR_FAILURE;
        if(srs.find("EPSG:") != std::string::npos){
            err = osr.importFromEPSG(std::atoi(srs.substr(5).c_str()));
        }else if(srs.find("+proj") != std::string::npos){
            err = osr.importFromProj4(srs.c_str());
        } else {
            err = osr.importFromWkt(srs.c_str());
        }
        if (err != OGRERR_NONE)
            throw cgt_exception("could not recognize srs: " + srs);
        osr.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        return osr;
    }

    void cgt_proj::crs_to_proj(const std::string &source_crs,
                               std::string &proj_crs, osg::Vec3 &topcenteric, bool &is_topcenteric) const noexcept {
        if (source_crs.find("ENU:") != std::string::npos) {
            proj_crs = "EPSG:4326";
            is_topcenteric = true;
            std::vector<std::string> coords = split(source_crs.substr(4), ",");
            topcenteric.x() = std::atof(coords[1].c_str());
            topcenteric.y() = std::atof(coords[0].c_str());
            topcenteric.z() = 0;
            if (coords.size() == 3) {
                topcenteric.z() = std::atoi(coords[2].c_str());
            }
        } else {
            proj_crs = source_crs;
            is_topcenteric = false;
        }

    }

    osg::Vec3 cgt_proj::transfrom(osg::Vec3 vert) const {
       vert += source_metadata_.origin();
       osg::Vec3 result = pj_transorm(vert);
       return result - target_metadata_.origin();
    }

    void cgt_proj::transform(OGRGeometry* geom) const{
        OGRPointOffsetVisitor visitor(source_metadata_.origin().x(),
                                      source_metadata_.origin().y(),
                                      source_metadata_.origin().z());
        geom->accept(&visitor);
        OGRErr err = geom->transform(transform_srs_.get());
        if(err != OGRERR_NONE){
            throw cgt_exception("transfrom failed");
        }
    }

    osg::Vec3 cgt_proj::pj_transorm (osg::Vec3 vert, bool forward) const {
        OGRPoint point(vert.x(), vert.y(), vert.z());
        if (source_srs_is_topcenteric_) {
            double lat_r, lon_r, h;
            vert = vert * source_local_to_world_;
            ellipsoid_.convertXYZToLatLongHeight(vert.x(), vert.y(), vert.z(),
                                                 lat_r, lon_r, h);
            double lat_d = osg::RadiansToDegrees(lat_r);
            double lon_d = osg::RadiansToDegrees(lon_r);
            point = OGRPoint(lon_d, lat_d, h);
        }
        OGRErr err = point.transform(transform_srs_.get());
        if (err != OGRERR_NONE)
            throw cgt_exception("transform fail");
        if (target_srs_is_topcenteric_) {
            double lat_r = osg::DegreesToRadians(point.getY());
            double lon_r = osg::DegreesToRadians(point.getX());
            double h = point.getZ();
            double x, y, z;
            ellipsoid_.convertLatLongHeightToXYZ(lat_r, lon_r, h,
                                                 x, y, z);
            vert.x() = x;
            vert.y() = y;
            vert.z() = z;
            vert = vert * target_world_to_local_;
            point = OGRPoint(vert.x(), vert.y(), vert.z());

        }
        return osg::Vec3(point.getX(), point.getY(), point.getZ());
    }

    osg::Vec3 cgt_proj::pj_transorm (double x, double y, double z, bool forward) const {
        return pj_transorm(osg::Vec3(x, y, z), forward);
    }
}