cmake_minimum_required(VERSION 3.21)
project(cubiq)

if(APPLE)
	set(CMAKE_PREFIX_PATH "/usr/local/cellar/qt@5/5.15.2_1") # Qt 5.15.2 installed with Homebrew
elseif(UNIX)
	set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu")
elseif (WIN32)
	set(CMAKE_PREFIX_PATH "C:/Qt/Qt-5.12.12")
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

include_directories(BEFORE src)
add_executable(cubiq src/core/main.cpp src/core/main_window.cpp src/core/graph_view.cpp src/core/graph.cpp src/equations/function.cpp)
target_link_libraries(cubiq
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

