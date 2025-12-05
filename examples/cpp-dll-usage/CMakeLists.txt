cmake_minimum_required(VERSION 3.10)
project(TicDllExample)

# Require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Tic library using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(TIC REQUIRED libpololu-tic-1)

# Create the example executable
add_executable(example example.cpp)

# Link against the Tic library
target_include_directories(example PRIVATE ${TIC_INCLUDE_DIRS})
target_link_libraries(example ${TIC_LIBRARIES})
target_compile_options(example PRIVATE ${TIC_CFLAGS_OTHER})

# On Windows, copy the DLL to the output directory
# Note: This is a best-effort approach. The DLL location may vary.
# If this doesn't work, manually copy the DLL to the executable directory.
if(WIN32)
  # Try to find the DLL in common locations
  find_file(TIC_DLL pololu-tic-1.dll
    PATHS ${TIC_LIBRARY_DIRS} ${TIC_LIBRARY_DIRS}/../bin
    NO_DEFAULT_PATH
  )
  if(TIC_DLL)
    add_custom_command(TARGET example POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${TIC_DLL}"
      $<TARGET_FILE_DIR:example>
      COMMENT "Copying Tic DLL to output directory"
    )
  else()
    message(WARNING "Could not find pololu-tic-1.dll. You may need to manually copy it to the executable directory.")
  endif()
endif()
