cmake_minimum_required(VERSION 3.15)
project(CompiladoresRust LANGUAGES CXX)

# Usar estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar diagnósticos útiles
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DPROJECT_ROOT="${CMAKE_SOURCE_DIR}/")

# Crear ejecutable
add_executable(CompiladoresRust
    main.cpp
    lectura.cpp
    LexerRust.cpp
    ParserRust.cpp
    Logger.cpp
    NodoAST.cpp
)

# Enlazar filesystem si es necesario
# Esto ya no suele ser obligatorio en g++ 10 o superior, pero puede evitar errores:
target_link_libraries(CompiladoresRust stdc++fs)
