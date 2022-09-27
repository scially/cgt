//
// Created by adminstrator on 2022/9/19.
//

#pragma once

#include <cgt/cgtcore.h>

namespace scially {
    class osg_transform : public osg_base {
    public:
        osg_transform(const std::string &source_dir, const std::string &target_dir)
                : osg_base(source_dir, target_dir) {
        }

        virtual bool root_process(cgt_proj &proj, const std::string &tile_path) override {
            return true;
        }

        virtual bool tile_process(cgt_proj &proj, const std::string &tile_path) override;

        virtual bool end_process() override {
            target_metadata_.write(target_dir_ + "/metadata.xml");
            return true;
        }
    };
}