# ########################################################################
# Copyright (c) 2019-2021 Advanced Micro Devices, Inc.
# ########################################################################

# This is incremented when the ABI to the library changes
set( rocsolver_SOVERSION 0.1 )

# This option only works for make, nmake and ninja, but no reason it shouldn't be on all the time
# It creates a compile_commands.json file for use with clang tooling or vim
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Print out compiler flags for viewing/debug
if( BUILD_VERBOSE )
  message( STATUS "\t==>CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )
  message( STATUS "\t==>BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}" )
  message( STATUS "\t==>CMAKE_INSTALL_PREFIX link: " ${CMAKE_INSTALL_PREFIX} )
  message( STATUS "\t==>CMAKE_MODULE_PATH link: " ${CMAKE_MODULE_PATH} )
  message( STATUS "\t==>CMAKE_PREFIX_PATH link: " ${CMAKE_PREFIX_PATH} )
  message( STATUS "==============" )
  message( STATUS "\t==>CMAKE_CXX_COMPILER: " ${CMAKE_CXX_FLAGS} )
  message( STATUS "\t==>CMAKE_CXX_COMPILER debug: " ${CMAKE_CXX_FLAGS_DEBUG} )
  message( STATUS "\t==>CMAKE_CXX_COMPILER release: " ${CMAKE_CXX_FLAGS_RELEASE} )
  message( STATUS "\t==>CMAKE_CXX_COMPILER relwithdebinfo: " ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} )
  message( STATUS "\t==>CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS} )
  message( STATUS "\t==>CMAKE_EXE_LINKER_FLAGS_RELEASE: " ${CMAKE_EXE_LINKER_FLAGS_RELEASE} )
  message( STATUS "\t==>CMAKE_SHARED_LINKER_FLAGS: " ${CMAKE_SHARED_LINKER_FLAGS} )
  message( STATUS "\t==>CMAKE_SHARED_LINKER_FLAGS_RELEASE: " ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} )
  message( STATUS "==============" )
  message( STATUS "\t==>CMAKE_SHARED_LIBRARY_C_FLAGS: ${CMAKE_SHARED_LIBRARY_C_FLAGS}" )
  message( STATUS "\t==>CMAKE_SHARED_LIBRARY_CXX_FLAGS: ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}" )
  message( STATUS "\t==>CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}" )
  message( STATUS "\t==>CMAKE_SHARED_LINKER_FLAGS_DEBUG: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" )
  message( STATUS "\t==>CMAKE_SHARED_LINKER_FLAGS_RELEASE: ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" )
endif( )

# Create version header from templated .in file using CMake info
configure_file( include/rocsolver-version.h.in "${PROJECT_BINARY_DIR}/include/rocsolver-version.h" )

set( rocsolver_headers_public
  include/rocsolver.h
  include/rocsolver-extra-types.h
  include/rocsolver-aliases.h
  include/rocsolver-functions.h
  ${PROJECT_BINARY_DIR}/include/rocsolver-version.h
  ${PROJECT_BINARY_DIR}/include/rocsolver-export.h
)

# For IDE integration
source_group( "Header Files\\Public" FILES ${rocsolver_headers_public} )

include( GNUInstallDirs )

set( BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR} )
set( LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR} )
set( INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} )

# Build into subdirectories
add_subdirectory( src )

# Package specific CPACK vars
string( TOLOWER "${HIP_RUNTIME}" HIP_RUNTIME_LOWER )
if( HIP_RUNTIME_LOWER STREQUAL "rocclr" )
  set( RPM_REQUIREMENTS "hip-rocclr >= 3.5.0" )
  set( CPACK_DEBIAN_PACKAGE_DEPENDS "hip-rocclr (>= 3.5.0)" )
elseif( HIP_RUNTIME STREQUAL "cuda")
  set( RPM_REQUIREMENTS "hip-nvcc >= 3.5.0" )
  set( CPACK_DEBIAN_PACKAGE_DEPENDS "hip-nvcc (>= 3.5.0)" )
else()
  set( RPM_REQUIREMENTS "rocm-dev >= 2.5.27" )
  set( CPACK_DEBIAN_PACKAGE_DEPENDS "rocm-dev (>= 2.5.27)" )
endif()

if( OS_ID_sles )
  set( RPM_REQUIREMENTS "${RPM_REQUIREMENTS}, libLLVM7 >= 7.0.1")
endif()
set( CPACK_RPM_PACKAGE_REQUIRES ${RPM_REQUIREMENTS} )

set( CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md" )

if( NOT CPACK_PACKAGING_INSTALL_PREFIX )
  set( CPACK_PACKAGING_INSTALL_PREFIX "${ROCM_PATH}" )
endif( )

set( CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
  "\${CPACK_PACKAGING_INSTALL_PREFIX}"
  "\${CPACK_PACKAGING_INSTALL_PREFIX}/include"
  "\${CPACK_PACKAGING_INSTALL_PREFIX}/lib"
)

# Give rocsolver compiled for CUDA backend a different name
if( CXX_VERSION_STRING MATCHES "clang" )
    set( package_name rocsolver )
else( )
    set( package_name rocsolver-alt )
endif( )

set( ROCSOLVER_CONFIG_DIR "\${CPACK_PACKAGING_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
  CACHE PATH "Path placed into ldconfig file" )

rocm_create_package(
    NAME ${package_name}
    DESCRIPTION "AMD ROCm SOLVER library"
    MAINTAINER "RocSOLVER maintainer <rocsolver-maintainer@amd.com>"
    LDCONFIG
    LDCONFIG_DIR ${ROCSOLVER_CONFIG_DIR}
)
