//
// Created by adminstrator on 2022/6/1.
//

#include <cgt/cgtcore.h>
#include <cgt/cgtproj.h>
#include <BS_thread_pool/BS_thread_pool.hpp>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/CoordinateSystemNode>
#include <osg/ComputeBoundsVisitor>
#include <osg/Math>

#include <filesystem>
#include <cmath>
#include <chrono>

namespace fs = std::filesystem;

namespace scially {
   
	void geom_visitor::apply(osg::Geometry& geom) {
		osg::Vec3Array *vertex = static_cast<osg::Vec3Array*>(geom.getVertexArray());
		for (size_t i = 0; i < vertex->size(); i++) {
            osg::Vec3 vert = algorithm_((*vertex)[i]);
            (*vertex)[i].x() = vert.x();
            (*vertex)[i].y() = vert.y();
            (*vertex)[i].z() = vert.z();
		}
        traverse(geom);
	}

    void geom_visitor::apply(osg::PagedLOD& lod) {
        lod.setDatabasePath(lod_path_);
    }

	void node_operator::read(const std::string& path) {
		node_name_ = std::filesystem::path(path).stem().string();
		node_ = osgDB::readRefNodeFile(path);
	}

	void node_operator::write(const std::string& dir) {
        std::string tile_outpath = (fs::path(dir) / node_name_ ).string() + ".osgb";
        osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
        //options->setOptionString("Compressor=zlib");
        options->setOptionString("WriteImageHint=IncludeFile");

        osgDB::writeNodeFile(*node_, dir + "/" + node_name_ + ".osgb", options);
	}

	void node_operator::apply(const std::string& dir, std::function<osg::Vec3(osg::Vec3)> algorithm) {
        geom_visitor visitor(dir, algorithm);
        node_->accept(visitor);
	}

    osg_transform::osg_transform(const std::string &source_dir, const std::string &target_dir)
                            : source_dir_(source_dir), target_dir_(target_dir){

    }

    void osg_transform::run(uint32_t max_thread){
        auto start = std::chrono::high_resolution_clock::now();

        proj_.create_crs_to_crs(source_metadata_.srs(), target_metadata_.srs());
        detect_target_metadata();

        auto this_algorithm = init_transform();

        max_thread = max_thread == 0 ?  std::thread::hardware_concurrency() - 1 : 1;
        BS::thread_pool thread_pool(max_thread);
        spdlog::info("cgt set thread to {}", max_thread);

        for(const auto &tile_dir : fs::directory_iterator(source_dir_,
                                                   ~fs::directory_options::follow_directory_symlink
                                                   | fs::directory_options::skip_permission_denied)){
            if(!fs::is_directory(tile_dir.path()))
                continue;

            std::string tile_name = tile_dir.path().stem().string();
            fs::path tile_target_dir = fs::path(target_dir_) / tile_name;
            if(!fs::exists(tile_target_dir))
                fs::create_directories(tile_target_dir);

            for(const auto &tile : fs::directory_iterator(tile_dir.path(),
                                                          ~fs::directory_options::follow_directory_symlink
                                                          | fs::directory_options::skip_permission_denied)){

                thread_pool.push_task([this_algorithm](
                        const osg_modeldata& source_metadata,
                        const osg_modeldata &target_metadata,
                        const std::string &tile_path,
                        const std::string &out_dir){
                    spdlog::info("start process {}", fs::path(tile_path).stem().string());
                    node_operator tile_processor;
                    tile_processor.read(tile_path);
                    tile_processor.apply(out_dir, this_algorithm);
                    tile_processor.write(out_dir);
                }, source_metadata_, target_metadata_, tile.path().string(), tile_target_dir.string());
            }
        }

        thread_pool.wait_for_tasks();
        write_metadata();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        spdlog::info("Processing is completed, it takes {}s", duration.count() / 1000.0);
    }


    vec3_transform osg_transform::init_transform(){
        osg::EllipsoidModel ellipsoid;

        double ellipsoid_x = 0, ellipsoid_y = 0, ellipsoid_z = 0;
        double ellipsoid_x_t = 0, ellipsoid_y_t = 0, ellipsoid_z_t = 0;

        if(source_metadata_.srs() == "EPSG:4326"){
            ellipsoid.convertLatLongHeightToXYZ(
                    osg::DegreesToRadians(source_metadata_.x()),
                    osg::DegreesToRadians(source_metadata_.y()),
                    osg::DegreesToRadians(source_metadata_.z()),
                    ellipsoid_x,ellipsoid_y,ellipsoid_z
                    );
        }

        if(target_metadata_.srs() == "EPSG:4326"){
            ellipsoid.convertLatLongHeightToXYZ(
                    osg::DegreesToRadians(target_metadata_.x()),
                    osg::DegreesToRadians(target_metadata_.y()),
                    osg::DegreesToRadians(target_metadata_.z()),
                    ellipsoid_x_t,ellipsoid_y_t,ellipsoid_z_t
            );
        }

        return [ellipsoid,
                ellipsoid_x, ellipsoid_y, ellipsoid_z,
                ellipsoid_x_t, ellipsoid_y_t, ellipsoid_z_t,
                this]
                (osg::Vec3 vert){

            double x = 0, y = 0, z = 0, lat = 0, lon = 0, height = 0;
            if(source_metadata_.srs() == "EPSG:4326"){
                x = ellipsoid_x + vert.x();
                y = ellipsoid_y + vert.y();
                z = ellipsoid_z + vert.z();

                ellipsoid.convertXYZToLatLongHeight(x, y, z, lat, lon, height);
                x = lon;
                y = lat;
                z = height;
            }else{
                x = vert.x() + source_metadata_.x();
                y = vert.y() + source_metadata_.y();
                z = vert.z();
            }


            PJ_COORD coord = proj_coord(x, y, 0, 0);
            PJ_COORD trans_coord = proj_.trans(PJ_FWD, coord);

            osg::Vec3 result;

            if(target_metadata_.srs() == "EPSG:4326"){
                double x_vert = 0, y_vert = 0, z_vert = 0;
                ellipsoid.convertLatLongHeightToXYZ(osg::DegreesToRadians(trans_coord.xy.x),
                                                    osg::DegreesToRadians(trans_coord.xy.y),
                                                    z,
                                                    x_vert, y_vert, z_vert);
                result.x() = x_vert - ellipsoid_x_t;
                result.y() = y_vert - ellipsoid_y_t;
                result.z() = z_vert - ellipsoid_z_t;

            }else{
                result.x() = trans_coord.xy.x - target_metadata_.x();
                result.y() = trans_coord.xy.y - target_metadata_.y();
                result.z() = z + source_metadata_.z() - target_metadata_.z();
            }

            return result;
        };

    }

    void osg_transform::write_metadata() {
        target_metadata_.write(target_dir_ + "/metadata.xml");
    }

    void osg_transform::detect_target_metadata(){
        if(source_metadata_.srs() != "EPSG:4326"
            && ( std::abs(source_metadata_.x()) < 1e-3 && std::abs(source_metadata_.y() < 1e-3))){
            osg::BoundingBox tile_bbox;
            tile_bbox.init();
            for(const auto &tile_dir : fs::directory_iterator(source_dir_,
                                                              ~fs::directory_options::follow_directory_symlink
                                                              | fs::directory_options::skip_permission_denied)) {
                if(!fs::is_directory(tile_dir.path()))
                    continue;
                fs::path tile_name = tile_dir.path().filename();
                std::string tile_path = (tile_dir.path() / tile_name).string() + ".osgb";
                osg::ref_ptr<osg::Node> root = osgDB::readRefNodeFile(tile_path);
                osg::ComputeBoundsVisitor cbv;
                root->accept(cbv);
                osg::BoundingBox bb = cbv.getBoundingBox(); // in local coords.
                tile_bbox.expandBy(bb);
            }

            int32_t tile_center_x = tile_bbox.center().x();
            int32_t tile_center_y = tile_bbox.center().y();
            int32_t tile_center_z = 0;
            PJ_COORD tile_center = proj_coord(tile_center_x, tile_center_y, tile_center_z, 0);
            PJ_COORD tile_center_proj = proj_.trans(PJ_FWD, tile_center);

            target_metadata_.set_x(tile_center_proj.xy.x);
            target_metadata_.set_y(tile_center_proj.xy.y);
            target_metadata_.set_z(0);
        }
    }
}