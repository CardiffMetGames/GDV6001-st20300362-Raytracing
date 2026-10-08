cmake_minimum_required ( VERSION 3.1.0...4.0.0 )

project ( Raytracing LANGUAGES CXX )

# Set to C++11
set ( CMAKE_CXX_STANDARD          11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

# Source

set ( EXTERNAL
  src/external/stb_image.h
)
	
set ( SOURCE_RAYTRACER
  src/main.cpp
  src/vec3.cpp
  src/vec3.h
  src/colour.h
  src/ray.h
  src/ray.cpp
  src/hittable.h
  src/sphere.h
  src/sphere.cpp
  src/hittable_list.h
  src/hittable_list.cpp
  src/common.h
  src/interval.h
  src/interval.cpp
  src/camera.h
  src/camera.cpp
)

include_directories(src)

add_executable(Raytracer      ${EXTERNAL} ${SOURCE_RAYTRACER})