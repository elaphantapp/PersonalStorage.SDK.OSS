cmake_minimum_required(VERSION 3.12)

include(../config/cmake/CMakeLists.txt)

include_directories("${CMAKE_INSTALL_PREFIX}/include")

project(Elastos.SDK.CloudStorage)

add_subdirectory(common)
add_subdirectory(src)

if(DEFINED CFG_WITH_TEST)
	add_subdirectory(test)
endif()

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
