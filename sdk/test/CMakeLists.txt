#pkg_search_module(pkg-openssl REQUIRED openssl)
#message(STATUS "openssl include path: ${pkg-openssl_INCLUDE_DIRS}")
#message(STATUS "openssl library path: ${pkg-openssl_STATIC_LDFLAGS}")

file( GLOB CloudStorageTest-SOURCES "CSTest*.cpp")
add_executable(CloudStorageTest)
target_sources(CloudStorageTest PRIVATE
    ${CloudStorageTest-SOURCES})
target_include_directories(CloudStorageTest PRIVATE
    "${pkg-openssl_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(CloudStorageTest PUBLIC
    ${pkg-openssl_STATIC_LDFLAGS}
    Elastos.SDK.CloudStorage)
