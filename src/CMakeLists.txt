file(GLOB SOURCES "*.cpp")

add_executable(faccha ${SOURCES})
target_link_libraries(faccha ${OpenCV_LIBS})