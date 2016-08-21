cmake_minimum_required(VERSION 3.2)
project(test_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCE_FILES main.cpp serialization.h Player.cpp Player.h command.cpp command.h Projectile.cpp Projectile.h SFUtils.h ResourceHolder.h ResourceHolder.cpp Area.cpp Area.h Entity.cpp Entity.h BoundingBox.cpp BoundingBox.h Wall.cpp Wall.h Menu.cpp Menu.h ClientSocket.cpp ClientSocket.h ServerSocket.cpp ServerSocket.h World.cpp World.h)
add_executable(test_client ${SOURCE_FILES} main.cpp)
TARGET_LINK_LIBRARIES(test_client pthread)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(test_client ${SFML_LIBRARIES})
endif()