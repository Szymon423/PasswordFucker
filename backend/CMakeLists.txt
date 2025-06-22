cmake_minimum_required(VERSION 3.15)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message("VCPKG: $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
else() 
    message("VCPKG: Not found")
endif()

project(PasswordFuckerBackend VERSION 1.0.0 LANGUAGES CXX)

# Ustaw standard C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opcje kompilacji
option(BUILD_TESTS "Build tests" OFF)

# Diagnostyka
message("System: ${CMAKE_SYSTEM_NAME}")
message("Generator: ${CMAKE_GENERATOR}")
message("VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")

# Znajdź odpowiednie pakiety
find_package(spdlog CONFIG REQUIRED)
find_package(Poco REQUIRED COMPONENTS Util Net)
find_package(SQLiteCpp CONFIG REQUIRED)

# Dodaj exe
add_executable(PasswordFuckerBackend main.cpp)

# Dodaj podkatalog src
add_subdirectory(src)
include_directories(${PROJECT_SOURCE_DIR}/src)

# Linkowanie 
target_link_libraries(PasswordFuckerBackend PRIVATE 
    PasswordFucker_lib    
    spdlog::spdlog
    Poco::Net
	Poco::Util
    ws2_32
    kernel32
    SQLiteCpp
)

target_include_directories(PasswordFuckerBackend PUBLIC 
    src
)

# Konfiguracja testów
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Ustaw flagi kompilatora
add_compile_options(-Wall -Wextra -Wpedantic)
