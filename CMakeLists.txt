cmake_minimum_required(VERSION 3.20)
project(Minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(Mineswapers MACOSX_BUNDLE WIN32 main.cpp
        Cell.h
        Game.cpp
        Game.h
        Cell.cpp
        Mineswapers.h
        Mineswapers.cpp
        Services/Text.hpp
        MainMenu.h
        MainMenu.cpp)

target_link_libraries(Mineswapers PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)