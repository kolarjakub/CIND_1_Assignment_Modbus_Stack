cmake_minimum_required(VERSION 3.22.1)
project(ModbusStack C)

set(CMAKE_C_STANDARD 11)

# -- adresář s hlavičkami
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# -- zdrojové soubory knihovny
set(MODBUS_LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ModbusTCP.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ModbusAP.c
)

# -- vytvoř knihovnu (statickou)
add_library(modbus STATIC ${MODBUS_LIB_SRC})

# -- hlavní klient (spustitelný program)
add_executable(modbus_client
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ModbusClient.c
)

# -- přilinkuj knihovnu k programu
target_link_libraries(modbus_client PRIVATE modbus)

# -- nastav, kam se bude ukládat
set_target_properties(modbus PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
set_target_properties(modbus_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
