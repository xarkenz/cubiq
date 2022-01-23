cmake_minimum_required(VERSION 3.21)
project(cubiq)

if(APPLE)
	set(CMAKE_PREFIX_PATH "/usr/local/cellar/qt@5/5.15.2_1") # Qt 5.15.2 installed with Homebrew
elseif(UNIX)
	set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(cubiq src/main.cpp src/main_window.cpp src/graph_view.cpp)
target_link_libraries(cubiq
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

