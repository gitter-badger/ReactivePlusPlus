#                  ReactivePlusPlus library
#
#          Copyright Aleksey Loginov 2022 - present.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/victimsnino/ReactivePlusPlus
#

file(GLOB_RECURSE FILES "*.h")

if(${CMAKE_VERSION} VERSION_LESS "3.19.0")
  add_library(rpp INTERFACE)
else()
  add_library(rpp INTERFACE ${FILES})
endif()

target_sources(rpp INTERFACE ${FILES})
target_include_directories(rpp INTERFACE .)
target_link_libraries(rpp INTERFACE coverage_config Threads::Threads)

foreach(FILE ${FILES}) 
  get_filename_component(PARENT_DIR "${FILE}" PATH)
  file(RELATIVE_PATH REL_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/rpp" ${PARENT_DIR})
  set(REL_PARENT_DIR "Header Files\\${REL_PARENT_DIR}")

  string(REPLACE "/" "\\" GROUP ${REL_PARENT_DIR})
  source_group("${GROUP}" FILES "${FILE}")
endforeach()
