project(kpisrla CXX)

add_library(kpisrla SHARED
    kpisrla.cpp
    kpisrla.h
    kpisrla.def
    dllmain.cpp
    framework.h
    pch.cpp
    pch.h
    KpiSRLADecoderModule.cpp
    KpiSRLADecoderModule.h
    KpiSRLADecoder.cpp
    KpiSRLADecoder.h
    SampleHolder.cpp
    SampleHolder.h
)
target_link_libraries(kpisrla PRIVATE srladec)
target_include_directories(kpisrla PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../srla/include)
set_target_properties(kpisrla 
    PROPERTIES
        CXX_STANDARD 20
        SUFFIX ".kpi"
)
add_dependencies(kpisrla srladec)
