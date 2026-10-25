cmake_minimum_required(VERSION 3.20)
project(QuickKineticFramework VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Common library for QKF core (lexer, parser, ast, codegen, native_gen, gpq config)
set(QKF_CORE_SOURCES
    src/compiler/lexer.cpp
    src/compiler/parser.cpp
    src/compiler/ast.cpp
    src/compiler/codegen.cpp
    src/compiler/native_gen.cpp
    src/compiler/driver.cpp
    src/config/config_gpq.cpp
    src/cloner/cloner.cpp
)

add_library(qkf_core STATIC ${QKF_CORE_SOURCES})

# Target 1: qkf.exe (The main compiler & runner CLI)
add_executable(qkf src/main_qkf.cpp)
target_link_libraries(qkf PRIVATE qkf_core)

# Target 2: qkf-cloner.exe (Dedicated cloner and project scaffolder)
add_executable(qkf-cloner src/main_cloner.cpp)
target_link_libraries(qkf-cloner PRIVATE qkf_core)

# Output directory: bin/
set_target_properties(qkf PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(qkf-cloner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
