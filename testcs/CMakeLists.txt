cmake_minimum_required(VERSION 3.5)
project(testcs)

add_compile_options(-std=c++14)
set(CMAKE_BUILD_TYPE "Debug") 

include_directories(
  include
)
add_definitions(${PCL_DEFINITIONS})
add_definitions(${EIGEN3_DEFINITIONS})
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON)



link_directories(${CMAKE_BINARY_DIR}) 

add_executable(http_server http_server.cpp)
add_executable(http_client http_client.cpp)
target_link_libraries(http_server ${Poco_LIBRARIES})
target_link_libraries(http_client ${Poco_LIBRARIES})



install(TARGETS http_server DESTINATION lib/${PROJECT_NAME})
install(TARGETS http_client DESTINATION lib/${PROJECT_NAME})