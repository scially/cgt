configure_file(
		${PROJECT_SOURCE_DIR}/include/cgt/config.h.in
		${PROJECT_SOURCE_DIR}/include/cgt/config.h)

add_library(cgtcore 
	"cgtcore.cpp"
	"cgtmodel.cpp"  
	"tinyxml2.cpp")

add_executable(cgt 	
	"main.cpp"
	)


target_link_libraries(cgtcore
		PROJ::proj
		${OPENSCENEGRAPH_LIBRARIES})

target_link_libraries(cgt cgtcore)