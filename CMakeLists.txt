cmake_minimum_required(VERSION 3.22.1)
project(Caesar-Cipher C CXX)

# Установка стандартов C и C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Устанавливаем флаги для различных конфигураций сборки
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O3 ") # -fsanitize=address,undefined -fprofile-arcs -ftest-coverage
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -g -O0 -fsanitize=address,undefined -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O2 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

# Источники для C и C++
file(GLOB C_SOURCES "src/*.c")
file(GLOB CPP_SOURCES "src/*.cpp")

# Создание целей для C и C++
if(C_SOURCES)
    add_executable(Caesar-Cipher_C ${C_SOURCES})
    target_include_directories(Caesar-Cipher_C PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

if(CPP_SOURCES)
    add_executable(Caesar-Cipher_CPP ${CPP_SOURCES})
    target_include_directories(Caesar-Cipher_CPP PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

find_library(MICROHTTPD_LIBRARY NAMES microhttpd)
if (NOT MICROHTTPD_LIBRARY)
    message(FATAL_ERROR "libmicrohttpd не найдена. Установите libmicrohttpd-dev.")
endif()

target_link_libraries(Caesar-Cipher_C ${MICROHTTPD_LIBRARY})


# Включаем CTest для тестирования
include(CTest)
enable_testing()

# Добавление тестов
if(TARGET Caesar-Cipher_C)
    add_test(NAME Caesar-Cipher_C_Test COMMAND Caesar-Cipher_C)
endif()

if(TARGET Caesar-Cipher_CPP)
    add_test(NAME Caesar-Cipher_CPP_Test COMMAND Caesar-Cipher_CPP)
endif()

# Покрытие кода с использованием lcov и gcovr
find_program(LCOV lcov)
find_program(GCOVR gcovr)

if (LCOV AND GCOVR)
    add_custom_target(coverage
        COMMAND ${LCOV} --capture --directory . --output-file coverage.info
        COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
        COMMAND ${LCOV} --list coverage.info
        COMMAND ${GCOVR} --root . --html --html-details -o coverage.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
        VERBATIM
    )
    message(STATUS "Цель 'coverage' доступна")
else()
    message(WARNING "Инструменты 'lcov' и/или 'gcovr' не найдены, цель 'coverage' недоступна.")
endif()

# Статический анализ с cppcheck
find_program(CPPCHECK cppcheck)
if (CPPCHECK)
    add_custom_target(static_analysis
        COMMAND ${CPPCHECK} --enable=all --inconclusive --quiet ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis with cppcheck"
        VERBATIM
    )
    message(STATUS "Цель 'static_analysis' доступна")
else()
    message(WARNING "Инструмент 'cppcheck' не найден, цель 'static_analysis' недоступна.")
endif()

# Форматирование кода с clang-format
find_program(CLANG_FORMAT clang-format)
if (CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${C_SOURCES} ${CPP_SOURCES}
        COMMENT "Formatting source code with clang-format"
        VERBATIM
    )
    message(STATUS "Цель 'format' доступна")
else()
    message(WARNING "Инструмент 'clang-format' не найден, цель 'format' недоступна.")
endif()

# Проверка памяти с Valgrind
find_program(VALGRIND valgrind)
if (VALGRIND)
    if(TARGET Caesar-Cipher_C)
        add_custom_target(memory_check_c
            COMMAND ${VALGRIND} --leak-check=full --track-origins=yes ./Caesar-Cipher_C
            COMMENT "Running memory check on C executable with Valgrind"
            VERBATIM
        )
    endif()

    if(TARGET Caesar-Cipher_CPP)
        add_custom_target(memory_check_cpp
            COMMAND ${VALGRIND} --leak-check=full --track-origins=yes ./Caesar-Cipher_CPP
            COMMENT "Running memory check on C++ executable with Valgrind"
            VERBATIM
        )
    endif()

    message(STATUS "Цели 'memory_check_c' и 'memory_check_cpp' доступны")
else()
    message(WARNING "Инструмент 'valgrind' не найден, цели 'memory_check_c' и 'memory_check_cpp' недоступны.")
endif()
