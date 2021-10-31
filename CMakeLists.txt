cmake_minimum_required(VERSION 3.15)
project(Float_IEEE_CPP VERSION 0.1)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
add_executable(FloatToIEEE Main.cc Float.cc)
target_link_libraries(Float_IEEE m)
