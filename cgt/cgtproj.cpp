//
// Created by adminstrator on 2022/9/16.
//

#include <cgt/cgtproj.h>
#include <osg/CoordinateSystemNode>

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
        if(target_metadata_.is_valid() && target_metadata_.srs() != source_metadata_.srs()){
            std::string source_srs = crs_to_proj(source_metadata_.srs());
            std::string target_srs = crs_to_proj(target_metadata_.srs());
            source_srs_ = import(source_srs);
            target_srs_ = import(target_srs);

            transform_srs_.reset(OGRCreateCoordinateTransformation(&source_srs_, &target_srs_));
            if(transform_srs_ == nullptr)
                throw cgt_exception("could transfrom from " + source_srs + "to " + target_srs);
        }
    }


    OGRSpatialReference cgt_proj::import(const std::string& srs) const{
        OGRSpatialReference osr;
        OGRErr err = OGRERR_FAILURE;
        if(srs.find("EPSG:") != std::string::npos){
            err = osr.importFromEPSG(std::atoi(srs.substr(5).c_str()));
        }else if(srs.find("+proj") != std::string::npos){
            err = osr.importFromProj4(srs.c_str());
        }else{
            err = osr.importFromWkt(srs.c_str());
        }
        if(err != OGRERR_NONE)
            throw cgt_exception("could not recognize srs: " + srs);
        osr.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        return osr;
    }

    std::string cgt_proj::crs_to_proj(const std::string &source_crs) const noexcept{
        if(source_crs.find("ENU:") != std::string::npos ){
            std::stringstream ss;
            std::vector<std::string> coords = split(source_crs.substr(4), ",");
            // https://proj.org/operations/conversions/topocentric.html
            ss << "+proj=topocentric" << " +ellps=WGS84 "
               << " +lon_0=" <<  coords[0] << " +lat_0=" << coords[1] ;
            if(coords.size() == 3)
                ss << " +h_0=" << coords[2];
            return ss.str();
        }

        return source_crs;
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
        OGRErr err = point.transform(transform_srs_.get());
        if(err != OGRERR_NONE)
            throw cgt_exception("transfrom fail");

        return osg::Vec3(point.getX(), point.getY(), point.getZ());
    }

    osg::Vec3 cgt_proj::pj_transorm (double x, double y, double z, bool forward) const {
        return pj_transorm(osg::Vec3(x, y, z), forward);
    }
}