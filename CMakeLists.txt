CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(YourProjectName)

# Set C++ standard
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../)

# Add include directories
INCLUDE_DIRECTORIES(include)

# to easily compile multiple singular executable
MACRO(COMPILE name)
    ADD_EXECUTABLE(${name} ${ARGN})
    IF(WIN32) 
        target_link_libraries(${name}
            ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
            ${CMAKE_SOURCE_DIR}/lib/glew32s.lib
            # ${CMAKE_SOURCE_DIR}/libassimp-5.dll
            # glu32
            opengl32
            winmm
            gdi32
        )
    ELSE()
        target_link_libraries(${name}
            GL
            GLU
            glfw
            GLEW
            m
            Xrandr
            Xi
            X11
            Xxf86vm
            pthread
        )
    ENDIF()
ENDMACRO()

# Add source files
# Jika anda menambah file source, silahkan update variable ini
set(SOURCES
    src/main.cpp
)

# Compile source files
#          substitute here
#             VVVVVVVVV
COMPILE(main ${SOURCES})

# multiple singular project
#     name of exe   the source file
#       VVVVV    VVVVVVVVVVVVVVV
# COMPILE(sandbox  src/sandbox.cpp)


ADD_DEFINITIONS(-DGLEW_STATIC)