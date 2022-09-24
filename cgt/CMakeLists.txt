configure_file(
		${PROJECT_SOURCE_DIR}/include/cgt/config.h.in
		${PROJECT_SOURCE_DIR}/include/cgt/config.h)

add_library(cgtcore 
	"cgtcore.cpp"
	"cgtmodel.cpp"  
	"tinyxml2.cpp"
	"cgtproj.cpp"
	"cgtvisitor.cpp"
	"cgttransform.cpp"
		"cgtexport.cpp")

add_executable(cgt
		"main.cpp")

target_link_libraries(cgtcore
		${GDAL_LIBRARIES}
		${OPENSCENEGRAPH_LIBRARIES})

target_link_libraries(cgt cgtcore)

add_custom_command(TARGET cgt
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources" "${EXECUTABLE_OUTPUT_PATH}"
		VERBATIM
		)