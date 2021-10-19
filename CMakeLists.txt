# Project information
cmake_minimum_required(VERSION 3.18)
project(COLittleVulkan)
# C++ configuration
set(CMAKE_CXX_STANDARD 20)
# TODO I don't really like throwing all the shaders in one folder
#  I'd prefer if they were bundled with the code that uses them, but the project isn't big enough
#  to warran those types of changes yet; plus this is easier.
function(add_shader TARGET SHADER)
    # Find GLSLC for building sprv shaders
    # TODO fail configure more loudly if glslc is not found
    find_program(GLSLC glslc)
    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER})
    set(current-output-path ${CMAKE_BINARY_DIR}/shaders/${SHADER}.spv)
    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})
    add_custom_command(
           OUTPUT ${current-output-path}
           COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
           DEPENDS ${current-shader-path}
           IMPLICIT_DEPENDS CXX ${current-shader-path}
           VERBATIM)
    # Make sure our build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_shader)
# External C++ packages
# 3rd party packages handled by conan see conanfile.txt
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
# Glob sources and create executable
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/first_app.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard_movement_controller.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_camera.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_device.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_game_object.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_model.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_pipeline.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_renderer.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_swap_chain.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/lve_window.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/simple_render_system.cpp)
add_executable(main ${SOURCES})
# Include directory for tinyObjectLoader
# TODO maybe replace this with something a bit more robust like AssImp
target_include_directories(main PRIVATE /libs/tinyObjectLoader/)
# TODO maybe glob these instead
# Add shaders to the build system
add_shader(main simple_shader.vert)
add_shader(main simple_shader.frag)
# Copy models to build directory
add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/models
                ${CMAKE_CURRENT_BINARY_DIR}/models)
# Link all external libs to main
target_link_libraries(main ${CONAN_LIBS})