//
// Created by adminstrator on 2022/9/19.
//

#include <cgt/cgtvisitor.h>
#include <osgDB/WriteFile>

namespace scially {
    void geom_visitor::apply(osg::Geode& geode) {
        for(uint32_t i = 0 ; i < geode.getNumDrawables(); i ++){
            osg::Geometry* geom= dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if(geom == nullptr){
                continue;
            }
            osg::Vec3Array *vertex = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
            if(vertex == nullptr){
                continue;
            }
            for (size_t i = 0; i < vertex->size(); i++) {
                osg::Vec3 vert = algorithm_((*vertex)[i]);
                vertex->at(i) = vert;
            }
            geom->dirtyBound();
        }
        traverse(geode);
    }

    void geom_visitor::apply(osg::PagedLOD& lod) {
        osg::Vec3 vert = algorithm_(lod.getCenter());
        lod.setCenter(vert);
        lod.setDatabasePath(base_path_);
        traverse(lod);
    }
}