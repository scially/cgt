INCLUDE_DIRECTORIES(${FBX_INCLUDE_DIR})

SET(TARGET_SRC
    fbxRAnimation.cpp
    fbxRCamera.cpp
    fbxRLight.cpp
    fbxRMesh.cpp
    fbxRNode.cpp
    ReaderWriterFBX.cpp
    WriterCompareTriangle.cpp
    WriterNodeVisitor.cpp
    fbxMaterialToOsgStateSet.cpp
)

SET(TARGET_H
    fbxMaterialToOsgStateSet.h
    fbxReader.h
    ReaderWriterFBX.h
    WriterCompareTriangle.h
    WriterNodeVisitor.h
)

IF(CMAKE_COMPILER_IS_GNUCXX)

    SET(CMAKE_CXX_FLAGS "-w")

ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    REMOVE_CXX_FLAG(-pedantic)
    REMOVE_CXX_FLAG(-Wall)
    REMOVE_CXX_FLAG(-Wunused)
    REMOVE_CXX_FLAG(-Wextra)

ENDIF()

SET(TARGET_LIBRARIES_VARS FBX_LIBRARY)
IF(WIN32)
    OPTION(FBX_SHARED OFF)
    IF(FBX_SHARED)
        ADD_DEFINITIONS(-DFBXSDK_SHARED)
    ENDIF(FBX_SHARED)
    SET(TARGET_EXTERNAL_LIBRARIES wininet)
ENDIF(WIN32)

add_library(osgdb_fbx MODULE ${TARGET_H} ${TARGET_SRC})
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(osgdb_fbx 
        ${FBX_LIBRARY_DEBUG} 
        ${FBX_XML2_LIBRARY_DEBUG} 
        ${FBX_ZLIB_LIBRARY_DEBUG} 
        ${OPENSCENEGRAPH_LIBRARIES})
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(osgdb_fbx 
        ${FBX_LIBRARY} 
        ${FBX_XML2_LIBRARY} 
        ${FBX_ZLIB_LIBRARY} 
        ${OPENSCENEGRAPH_LIBRARIES})
ENDIF()
