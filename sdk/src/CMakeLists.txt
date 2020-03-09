pkg_search_module(pkg-openssl REQUIRED openssl)
message(STATUS "openssl include path: ${pkg-openssl_INCLUDE_DIRS}")
message(STATUS "openssl library path: ${pkg-openssl_STATIC_LDFLAGS}")
pkg_search_module(pkg-curl REQUIRED libcurl)
message(STATUS "curl include path: ${pkg-curl_INCLUDE_DIRS}")
message(STATUS "curl library path: ${pkg-curl_STATIC_LDFLAGS}")
find_library(ALIOSS_LIBRARY NAMES alibabacloud-oss-cpp-sdk PATHS ${CMAKE_INSTALL_PREFIX}/lib)
message(STATUS "alioss library path: ${ALIOSS_LIBRARY}")


file( GLOB ${CMAKE_PROJECT_NAME}-SOURCES "*.cpp" )
file( GLOB ${CMAKE_PROJECT_NAME}-HEADERS "*.hpp" "../include/*.hpp")

if((NOT "${CFG_TARGET_PLATFORM}" STREQUAL "Android") AND (NOT "${CFG_TARGET_PLATFORM}" STREQUAL "iOS"))
file( GLOB ${CMAKE_PROJECT_NAME}-DEPHEADERS "../crosspl/*.hpp" )
endif()

add_library(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${${CMAKE_PROJECT_NAME}-SOURCES}
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "../include"
    "../crosspl"
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    common
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-openssl_INCLUDE_DIRS}")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-openssl_STATIC_LDFLAGS}")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-curl_INCLUDE_DIRS}")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-curl_STATIC_LDFLAGS}")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${ALIOSS_LIBRARY}")

if("${CFG_TARGET_PLATFORM}" STREQUAL "Android")
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE log)
elseif("${CFG_TARGET_PLATFORM}" STREQUAL "iOS")
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "-framework CFNetwork")
endif()

file( GLOB ${CMAKE_PROJECT_NAME}-PUBLIC_HEADER ${${CMAKE_PROJECT_NAME}-HEADERS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${CMAKE_PROJECT_NAME}-PUBLIC_HEADER}")
install(TARGETS ${CMAKE_PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/${CMAKE_PROJECT_NAME})
