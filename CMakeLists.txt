cmake_minimum_required(VERSION 3.22.1)
project(ModbusStack C)

set(CMAKE_C_STANDARD 11)

# -- adresář s hlavičkami
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# -- zdrojové soubory
set(SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ModbusTCP.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ModbusAP.c
)

# -- vytvoř knihovnu
add_library(modbus STATIC ${SRC})

# -- nastav, kam se bude ukládat
set_target_properties(modbus PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
