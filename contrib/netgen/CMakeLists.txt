# libnetgen

find_package(ZLIB REQUIRED)

set(SRC
    netgen/libsrc/core/archive.cpp
    netgen/libsrc/core/bitarray.cpp
    netgen/libsrc/core/exception.cpp
    netgen/libsrc/core/localheap.cpp
    netgen/libsrc/core/logging.cpp
    netgen/libsrc/core/flags.cpp
    netgen/libsrc/core/paje_trace.cpp
    netgen/libsrc/core/profiler.cpp
    netgen/libsrc/core/table.cpp
    netgen/libsrc/core/taskmanager.cpp
    netgen/libsrc/core/utils.cpp
    netgen/libsrc/core/version.cpp
    netgen/libsrc/core/ng_mpi_wrapper.cpp
    #
    netgen/libsrc/general/gzstream.cpp
    netgen/libsrc/general/hashtabl.cpp
    netgen/libsrc/general/mystring.cpp
    netgen/libsrc/general/ngbitarray.cpp
    netgen/libsrc/general/optmem.cpp
    netgen/libsrc/general/parthreads.cpp
    netgen/libsrc/general/seti.cpp
    netgen/libsrc/general/sort.cpp
    netgen/libsrc/general/spbita2d.cpp
    netgen/libsrc/general/table.cpp
    #
    netgen/libsrc/gprim/adtree.cpp
    netgen/libsrc/gprim/geom2d.cpp
    netgen/libsrc/gprim/geom3d.cpp
    netgen/libsrc/gprim/geomfuncs.cpp
    netgen/libsrc/gprim/geomtest3d.cpp
    netgen/libsrc/gprim/spline.cpp
    netgen/libsrc/gprim/splinegeometry.cpp
    netgen/libsrc/gprim/transform3d.cpp
    #
    netgen/libsrc/linalg/bfgs.cpp
    netgen/libsrc/linalg/densemat.cpp
    netgen/libsrc/linalg/linopt.cpp
    netgen/libsrc/linalg/linsearch.cpp
    netgen/libsrc/linalg/polynomial.cpp
    #
    netgen/libsrc/meshing/adfront2.cpp
    netgen/libsrc/meshing/adfront3.cpp
    netgen/libsrc/meshing/bisect.cpp
    netgen/libsrc/meshing/boundarylayer.cpp
    netgen/libsrc/meshing/clusters.cpp
    netgen/libsrc/meshing/curvedelems.cpp
    netgen/libsrc/meshing/delaunay.cpp
    netgen/libsrc/meshing/delaunay2d.cpp
    netgen/libsrc/meshing/geomsearch.cpp
    netgen/libsrc/meshing/global.cpp
    netgen/libsrc/meshing/hprefinement.cpp
    netgen/libsrc/meshing/improve2.cpp
    netgen/libsrc/meshing/improve2gen.cpp
    netgen/libsrc/meshing/improve3.cpp
    netgen/libsrc/meshing/localh.cpp
    netgen/libsrc/meshing/meshclass.cpp
    netgen/libsrc/meshing/meshfunc.cpp
    netgen/libsrc/meshing/meshfunc2d.cpp
    netgen/libsrc/meshing/meshing2.cpp
    netgen/libsrc/meshing/meshing3.cpp
    netgen/libsrc/meshing/meshtool.cpp
    netgen/libsrc/meshing/meshtype.cpp
    netgen/libsrc/meshing/msghandler.cpp
    netgen/libsrc/meshing/netrule2.cpp
    netgen/libsrc/meshing/netrule3.cpp
    netgen/libsrc/meshing/parser2.cpp
    netgen/libsrc/meshing/parser3.cpp
    netgen/libsrc/meshing/refine.cpp
    netgen/libsrc/meshing/ruler2.cpp
    netgen/libsrc/meshing/ruler3.cpp
    netgen/libsrc/meshing/secondorder.cpp
    netgen/libsrc/meshing/smoothing2.5.cpp
    netgen/libsrc/meshing/smoothing2.cpp
    netgen/libsrc/meshing/smoothing3.cpp
    netgen/libsrc/meshing/specials.cpp
    netgen/libsrc/meshing/topology.cpp
    netgen/libsrc/meshing/validate.cpp
    netgen/libsrc/meshing/bcfunctions.cpp
    netgen/libsrc/meshing/parallelmesh.cpp
    netgen/libsrc/meshing/paralleltop.cpp
    netgen/libsrc/meshing/basegeom.cpp
    netgen/libsrc/meshing/surfacegeom.cpp
    netgen/libsrc/meshing/debugging.cpp
    netgen/libsrc/meshing/fieldlines.cpp
    netgen/libsrc/meshing/visual_interface.cpp
    netgen/libsrc/meshing/boundarylayer2d.cpp
    netgen/libsrc/meshing/boundarylayer_interpolate.cpp
    #
    netgen/libsrc/rules/rule_hexrules.cpp
    netgen/libsrc/rules/rule_prismrules2.cpp
    netgen/libsrc/rules/rule_pyramidrules.cpp
    netgen/libsrc/rules/rule_pyramidrules2.cpp
    netgen/libsrc/rules/rule_quadrules.cpp
    netgen/libsrc/rules/rule_tetrules.cpp
    netgen/libsrc/rules/rule_triarules.cpp
    #
    netgen/libsrc/occ/Partition_Inter2d.cxx
    netgen/libsrc/occ/Partition_Inter3d.cxx
    netgen/libsrc/occ/Partition_Loop.cxx
    netgen/libsrc/occ/Partition_Loop2d.cxx
    netgen/libsrc/occ/Partition_Loop3d.cxx
    netgen/libsrc/occ/Partition_Spliter.cxx
    netgen/libsrc/occ/occgenmesh.cpp
    netgen/libsrc/occ/occgeom.cpp
    netgen/libsrc/occ/occmeshsurf.cpp
    netgen/libsrc/occ/occ_face.cpp
    netgen/libsrc/occ/occ_edge.cpp
    netgen/libsrc/occ/occ_vertex.cpp
    netgen/libsrc/occ/occ_utils.cpp
    #
    netgen/libsrc/stlgeom/meshstlsurface.cpp
    netgen/libsrc/stlgeom/stlgeom.cpp
    netgen/libsrc/stlgeom/stlgeomchart.cpp
    netgen/libsrc/stlgeom/stlgeommesh.cpp
    netgen/libsrc/stlgeom/stlline.cpp
    netgen/libsrc/stlgeom/stltool.cpp
    netgen/libsrc/stlgeom/stltopology.cpp
    #
    netgen/libsrc/geom2d/csg2d.cpp
    netgen/libsrc/geom2d/genmesh2d.cpp
    netgen/libsrc/geom2d/geometry2d.cpp
    #
    netgen/netgen.cpp
)

add_library(libnetgen SHARED)

target_sources(
    libnetgen
    PRIVATE
        ${SRC}
)

target_compile_definitions(
    libnetgen
    PUBLIC
        -DOCCGEOMETRY
)

if(NOT MSVC)
    set_target_properties(libnetgen PROPERTIES OUTPUT_NAME krado-netgen)
endif()

target_include_directories(
    libnetgen
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${OpenCASCADE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${ZLIB_INCLUDE_DIRS}>
)

target_link_libraries(
    libnetgen
    PRIVATE
        ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
        ${OpenCASCADE_DataExchange_LIBRARIES}
        ${ZLIB_LIBRARIES}
)

configure_file(netgen_config.hpp.in netgen_config.hpp)
configure_file(netgen_version.hpp.in netgen_version.hpp)

install(
    TARGETS libnetgen
    EXPORT krado-targets
)
