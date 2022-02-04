cmake_minimum_required(VERSION 3.21)
project(cubiq)

if(APPLE)
	set(CMAKE_PREFIX_PATH "/usr/local/cellar/qt@5/5.15.2_1") # Qt 5.15.2 installed with Homebrew

	set(CMAKE_C_COMPILER clang)
	set(CMAKE_CXX_COMPILER clang++)
	set(OPENMP_LIBRARIES "/usr/local/Cellar/llvm/13.0.0_2/lib")
	set(OPENMP_INCLUDES "/usr/local/Cellar/llvm/13.0.0_2/include")

	if(CMAKE_C_COMPILER_ID MATCHES "Clang")
		set(OpenMP_C "${CMAKE_C_COMPILER}")
		set(OpenMP_C_LIB_NAMES "omp")
		set(OpenMP_omp_LIBRARY omp)
	endif()
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
		set(OpenMP_CXX_LIB_NAMES "omp")
		set(OpenMP_omp_LIBRARY omp)
	endif()

elseif(UNIX)
	set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu")
elseif (WIN32)
	set(CMAKE_PREFIX_PATH "C:/Qt/Qt-5.12.12")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")	 # Needed for threading on some systems

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
	include_directories("${OPENMP_INCLUDES}")
	link_directories("${OPENMP_LIBRARIES}")
endif(OPENMP_FOUND)

qt5_add_resources(RESOURCES resources.qrc)

file(GLOB SOURCES src/*.cpp src/core/*.cpp src/equations/*.cpp)

include_directories(BEFORE src)
add_executable(cubiq ${RESOURCES} ${SOURCES})
target_link_libraries(cubiq PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(cubiq PUBLIC OpenMP::OpenMP_CXX)