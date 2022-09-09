//
// Created by adminstrator on 2022/9/6.
//

#pragma once

#include <proj.h>
#include <string>

namespace scially {
    struct cgt_proj_info {
        std::string id;                /* Name of the projection in question                       */
        std::string description;       /* Description of the projection                            */
        std::string definition;        /* Projection definition                                    */
        int         has_inverse;        /* 1 if an inverse mapping exists, 0 otherwise              */
        double      accuracy;           /* Expected accuracy of the transformation. -1 if unknown.  */
    };

    class cgt_proj {
    public:
        cgt_proj() {
            proj_context_ = proj_context_create();
        }

        cgt_proj(const cgt_proj&) = delete;

        bool create_crs_to_crs(const std::string &source_crs, const std::string &target_crs){
            proj_instance_ = proj_create_crs_to_crs(proj_context_,
                                                    source_crs.c_str(),
                                                    target_crs.c_str(),
                                                    nullptr);
            return proj_instance_ != nullptr;
        }

        PJ_COORD trans (PJ_DIRECTION direction, PJ_COORD coord) const{
            return proj_trans(proj_instance_, direction, coord);
        }

        cgt_proj_info pj_info(const std::string& srs){
            PJ* proj_srs = proj_create(proj_context_, srs.c_str());
            PJ_PROJ_INFO proj_info = proj_pj_info(proj_srs);
            cgt_proj_info proj_info_wrapper{
                    proj_info.id,
                    proj_info.description,
                    proj_info.definition,
                    proj_info.has_inverse,
                    proj_info.accuracy
            };
            proj_destroy(proj_srs);
            return proj_info_wrapper;
        }

        virtual ~cgt_proj(){
            if(proj_instance_)
                proj_destroy(proj_instance_);

            proj_context_destroy(proj_context_);
        }
    private:
        pj_ctx *proj_context_ = nullptr;
        PJ* proj_instance_ = nullptr;
    };
}