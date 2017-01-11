cmake_minimum_required(VERSION 3.6)
project(raytracer)

set(CMAKE_CXX_STANDARD 14)

set(LIB_FILES
        stb/stb_image.h
        stb/stb_image_write.h)

set(CORE_FILES
        core/Color.cpp core/Color.h
        core/Vec3.h core/Vec3.cpp
        core/GeometricObj.h core/GeometricObj.cpp
        MeshObj.h MeshObj.cpp
        PhotonUtil.h PhotonUtil.cpp
        Scene.h Scene.cpp
        core/SimpleObject.h core/SimpleObject.cpp
        Tracer.h Tracer.cpp)

# Copy Resources to Build Dir
set(RESOURCE_DIR resources)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_DIR})
file(GLOB RESOURCES ${CMAKE_SOURCE_DIR}/${RESOURCE_DIR}/*)
add_custom_target(COPY_RESOURCES)
foreach(res ${RESOURCES})
    add_custom_command(TARGET COPY_RESOURCES PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E
            copy ${res} ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_DIR}/)
endforeach()

set(SOURCE_FILES
        main.cpp
        ${CORE_FILES}
        ${LIB_FILES})

add_executable(raytracer ${SOURCE_FILES})
add_dependencies(raytracer COPY_RESOURCES)