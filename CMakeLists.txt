cmake_minimum_required(VERSION 3.5)
project(main)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(SOURCE_FILES
        glad.c
        Image.cpp
        Player.cpp
        Enemy.cpp
        main.cpp)

set(ADDITIONAL_INCLUDE_DIRS
        dependencies/include/GLAD)
set(ADDITIONAL_LIBRARY_DIRS
        dependencies/lib)   
set(ADDITIONAL_RUNTIME_LIBRARY_DIRS
        dependencies/bin)   

set (CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG}")

if(WIN32)
  set(ADDITIONAL_INCLUDE_DIRS 
        ${ADDITIONAL_INCLUDE_DIRS}
        dependencies/include)
  link_directories(${ADDITIONAL_LIBRARY_DIRS})
else()
	find_package(glfw3 REQUIRED)
endif()

include_directories(${ADDITIONAL_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

add_executable(main ${SOURCE_FILES})

target_include_directories(main PRIVATE ${OPENGL_INCLUDE_DIR})

if(WIN32)
  add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/dependencies/bin" $<TARGET_FILE_DIR:main>)
  set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  target_compile_options(main PRIVATE)
  target_link_libraries(main LINK_PUBLIC ${OPENGL_gl_LIBRARY} glfw3dll)
else()
  target_compile_options(main PRIVATE -Wnarrowing)
  target_link_libraries(main LINK_PUBLIC ${OPENGL_gl_LIBRARY} glfw rt dl)
endif()

