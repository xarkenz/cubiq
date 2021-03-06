cmake_minimum_required(VERSION 3.21)
project(cubiq)

if(APPLE)
	set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")

	set(CMAKE_C_COMPILER clang)
	set(CMAKE_CXX_COMPILER clang++)
	set(OPENMP_LIBRARIES "/opt/homebrew/opt/llvm/lib")
	set(OPENMP_INCLUDES "/opt/homebrew/opt/llvm/include")

	if(CMAKE_C_COMPILER_ID MATCHES "Clang")
		set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
		set(OpenMP_C_LIB_NAMES "omp")
		set(OpenMP_omp_LIBRARY omp)
	endif()
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
		set(OpenMP_CXX_LIB_NAMES "omp")
		set(OpenMP_omp_LIBRARY omp)
	endif()

elseif(UNIX)
	set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu")
elseif (WIN32)
	set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")	 # Needed for threading on some systems

find_package(Qt5 COMPONENTS Core Gui Widgets Svg REQUIRED)
find_package(OpenMP REQUIRED)

if (OPENMP_FOUND AND APPLE)
	include_directories("${OPENMP_INCLUDES}")
	link_directories("${OPENMP_LIBRARIES}")
endif()

qt5_add_resources(RESOURCES resources.qrc)

file(GLOB SOURCES src/*.cpp src/core/*.cpp src/equations/*.cpp)

include_directories(BEFORE src)
add_executable(cubiq ${RESOURCES} ${SOURCES})
target_link_libraries(cubiq PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Svg)
target_link_libraries(cubiq PUBLIC OpenMP::OpenMP_CXX)