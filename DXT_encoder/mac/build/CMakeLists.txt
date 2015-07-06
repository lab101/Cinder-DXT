cmake_minimum_required(VERSION 2.8)

set(bd ${CMAKE_CURRENT_LIST_DIR}/../)
set(id ${bd}/include)
set(sd ${bd}/src)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(debug_flag "_debug")
endif()

find_package(PNG REQUIRED)

include_directories(
  ${id}
  ${sd}
  ${sd}/poly
  ${PNG_INCLUDE_DIR}
  )

message(STATUS ${PNG_INCLUDE_DIR})

set(lib_sources
  ${sd}/poly/VideoDxtCreator.cpp
  ${sd}/poly/Log.cpp
  )

set(app_libs
  ${PNG_LIBRARY}
  -lz
  )

add_library(dxt ${lib_sources})

macro(create name)
  add_executable(${name}${debug_flag} ${sd}/${name}.cpp)
  add_dependencies(${name}${debug_flag} dxt)
  target_link_libraries(${name}${debug_flag} dxt ${app_libs})
  install(TARGETS ${name}${debug_flag} DESTINATION bin/test)
endmacro()

create("dxt_creator")
