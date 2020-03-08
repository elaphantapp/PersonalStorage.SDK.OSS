pkg_search_module(pkg-apr REQUIRED apr-1)
pkg_search_module(pkg-aprutil REQUIRED apr-util-1)
pkg_search_module(pkg-mxml REQUIRED mxml)
pkg_search_module(pkg-curl REQUIRED libcurl)
find_library(ALIOSS_LIBRARY NAMES oss_c_sdk PATHS ${CMAKE_INSTALL_PREFIX}/lib)


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

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-apr_INCLUDE_DIRS}")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-apr_STATIC_LDFLAGS}")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-aprutil_INCLUDE_DIRS}")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-aprutil_STATIC_LDFLAGS}")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-mxml_INCLUDE_DIRS}")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${pkg-mxml_STATIC_LDFLAGS}")
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
