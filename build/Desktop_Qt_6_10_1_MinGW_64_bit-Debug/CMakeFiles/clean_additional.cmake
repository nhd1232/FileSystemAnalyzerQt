# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\FileSystemAnalyzerQt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\FileSystemAnalyzerQt_autogen.dir\\ParseCache.txt"
  "FileSystemAnalyzerQt_autogen"
  )
endif()
