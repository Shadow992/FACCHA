file(GLOB SOURCES "*.cpp")

add_executable(faccha ${SOURCES})
target_link_libraries(faccha ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES})