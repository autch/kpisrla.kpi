project(srlaenc CXX)

add_executable(srlaenc
    srlaenc.cpp
    srlaenc.h
    parse_waveformat.cpp
    WaveReader.h
    Deinterleaver.h
)
target_link_libraries(srlaenc PRIVATE srlacodec)
target_include_directories(srlaenc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../srla/include)
set_target_properties(srlaenc PROPERTIES CXX_STANDARD 20)
add_dependencies(srlaenc srlacodec)
