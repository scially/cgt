//
// Created by adminstrator on 2022/9/19.
//

#include <cgt/cgttransform.h>

#include <filesystem>

namespace scially {
    namespace fs = std::filesystem;

    bool osg_transform::tile_process(scially::cgt_proj &proj, const std::string &tile_path) {
        std::string root_name = get_root_name(tile_path);
        std::string data_name = get_data_name(tile_path);

        node_operator tile_processor;
        tile_processor.read(tile_path);
        std::string base_path = (fs::path(target_dir_) / root_name).string();
        tile_processor.apply(base_path, [&proj](osg::Vec3 vert) {
            return proj.transfrom(vert);
        });

        std::string out_dir = (fs::path(target_dir_) / data_name / root_name).string();
        tile_processor.write(out_dir);
        return true;
    }
}